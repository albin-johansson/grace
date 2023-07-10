/*
 * MIT License
 *
 * Copyright (c) 2023 Albin Johansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "imgui_example.hpp"

#include <cstddef>    // offsetof
#include <stdexcept>  // runtime_error

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

namespace grace::examples {

ImGuiExample::ImGuiExample()
    : Example {"Dear ImGui Example"}
{
  const VkDescriptorPoolSize pool_sizes[] = {
      VkDescriptorPoolSize {
          .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          .descriptorCount = 4,
      },
  };

  mDescriptorPool = DescriptorPool::make(mDevice, 10, pool_sizes, u32_size(pool_sizes));
  if (!mDescriptorPool) {
    throw std::runtime_error {"Could not create ImGui descriptor pool"};
  }

  ImGui::CreateContext();
  ImGui::StyleColorsClassic();

  if (!ImGui_ImplSDL2_InitForVulkan(mWindow)) {
    throw std::runtime_error {"Could not initialize ImGui SDL2 backend"};
  }

  ImGui_ImplVulkan_InitInfo vulkan_info = {
      .Instance = mInstance,
      .PhysicalDevice = mGPU,
      .Device = mDevice,
      .QueueFamily = get_queue_family_indices(mGPU, mSurface).graphics.value(),
      .Queue = mGraphicsQueue,
      .PipelineCache = mPipelineCache,
      .DescriptorPool = mDescriptorPool,
      .Subpass = 0,
      .MinImageCount = mSwapchain.info().min_image_count,
      .ImageCount = mSwapchain.get_image_count(),
      .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
      .Allocator = nullptr,
      .CheckVkResultFn = nullptr,
  };
  if (!ImGui_ImplVulkan_Init(&vulkan_info, mRenderPass)) {
    throw std::runtime_error {"Could not initialize ImGui Vulkan backend"};
  }

  const auto font_scale = static_cast<float>(mWindow.get_size_in_pixels().width) /
                          static_cast<float>(mWindow.get_size().width);

  ImFontConfig font_cfg = {};
  font_cfg.SizePixels = 13.0f * font_scale;

  auto& io = ImGui::GetIO();
  io.FontGlobalScale = 1.0f / font_scale;
  io.Fonts->AddFontDefault(&font_cfg);
  io.Fonts->Build();
  ImGui::GetStyle().ScaleAllSizes(1.0f);

  const CommandContext cmd_context = {mDevice, mGraphicsQueue, mGraphicsCommandPool};
  execute_now(cmd_context, [](VkCommandBuffer cmd_buffer) {
    ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
  });

  mDescriptorSetLayout = DescriptorSetLayoutBuilder {mDevice}
                             .descriptor(0,
                                         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         VK_SHADER_STAGE_FRAGMENT_BIT)
                             .build();
  if (!mDescriptorSetLayout) {
    throw std::runtime_error {"Could not create descriptor set layout"};
  }

  mPipelineLayout =
      PipelineLayoutBuilder {mDevice}
          .descriptor_set_layout(mDescriptorSetLayout)
          .push_constant(VK_SHADER_STAGE_VERTEX_BIT, 0, 2 * sizeof(glm::vec2))
          .build();
  if (!mPipelineLayout) {
    throw std::runtime_error {"Could not create pipeline layout"};
  }

  mPipeline =
      GraphicsPipelineBuilder {mDevice}
          .with_render_pass(mRenderPass, 0)
          .with_layout(mPipelineLayout)
          .with_cache(mPipelineCache)
          .vertex_shader("shaders/imgui.vert.spv")
          .fragment_shader("shaders/imgui.frag.spv")
          .vertex_input_binding(0, sizeof(ImDrawVert))
          .vertex_attribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, pos))
          .vertex_attribute(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(ImDrawVert, uv))
          .vertex_attribute(0, 2, VK_FORMAT_R8G8B8A8_UNORM, offsetof(ImDrawVert, col))
          .primitive_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
          .rasterization(VK_POLYGON_MODE_FILL,
                         VK_CULL_MODE_NONE,
                         VK_FRONT_FACE_COUNTER_CLOCKWISE)
          .depth_test(false)
          .dynamic_state(VK_DYNAMIC_STATE_VIEWPORT)
          .dynamic_state(VK_DYNAMIC_STATE_SCISSOR)
          .color_blend_attachment(true)
          .build();
  if (!mPipeline) {
    throw std::runtime_error {"Could not create pipeline"};
  }
}

ImGuiExample::~ImGuiExample() noexcept
{
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiExample::record_commands()
{
  auto& frame = mFrames.at(mFrameIndex);
  const auto& swapchain_image_extent = mSwapchain.info().image_extent;

  auto& io = ImGui::GetIO();
  io.DisplaySize.x = static_cast<float>(swapchain_image_extent.width);
  io.DisplaySize.y = static_cast<float>(swapchain_image_extent.height);

  ImGui::NewFrame();
  ImGui_ImplSDL2_NewFrame(mWindow);
  ImGui_ImplVulkan_NewFrame();

  _record_ui();

  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                  frame.cmd_buffer,
                                  mUseCustomPipeline ? mPipeline : VK_NULL_HANDLE);
}

void ImGuiExample::_record_ui()
{
  if (ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    const auto& io = ImGui::GetIO();
    ImGui::Text("FPS: %.1f", io.Framerate);
    ImGui::Text("Frame: %.3f ms", io.DeltaTime * 1'000.0f);

    ImGui::Separator();

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Theme:");
    ImGui::SameLine();
    if (ImGui::Combo("##Theme", &mThemeIndex, "Classic\0Dark\0Light\0\0")) {
      if (mThemeIndex == 0) {
        ImGui::StyleColorsClassic();
      }
      else if (mThemeIndex == 1) {
        ImGui::StyleColorsDark();
      }
      else {
        ImGui::StyleColorsLight();
      }
    }

    ImGui::Checkbox("Use custom pipeline", &mUseCustomPipeline);
    ImGui::Checkbox("Show demo window", &mShowDemoWindow);
  }
  ImGui::End();

  if (mShowDemoWindow) {
    ImGui::ShowDemoWindow();
  }
}

void ImGuiExample::on_event(const SDL_Event& event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);
}

}  // namespace grace::examples
