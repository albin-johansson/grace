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

#include "triangle_example.hpp"

#include <cstddef>    // offsetof
#include <stdexcept>  // runtime_error

#include <glm/gtc/matrix_transform.hpp>

namespace grace::examples {

TriangleExample::TriangleExample()
    : Example {"Triangle Example"}
{
  mFunctions.vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(
      vkGetDeviceProcAddr(mDevice, "vkCmdPushDescriptorSetKHR"));
  if (!mFunctions.vkCmdPushDescriptorSetKHR) {
    throw std::runtime_error {"Could not load vkCmdPushDescriptorSetKHR function"};
  }

  VkResult result = VK_ERROR_UNKNOWN;

  mDescriptorSetLayout =
      DescriptorSetLayoutBuilder {mDevice}
          .use_push_descriptors()
          .descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
          .build(&result);
  if (!mDescriptorSetLayout) {
    throw std::runtime_error {"Could not create descriptor set layout"};
  }

  mPipelineLayout =
      PipelineLayoutBuilder {mDevice}
          .descriptor_set_layout(mDescriptorSetLayout)
          .push_constant(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants))
          .build(&result);
  if (!mPipelineLayout) {
    throw std::runtime_error {"Could not create pipeline layout"};
  }

  mPipeline =
      GraphicsPipelineBuilder {mDevice}
          .with_render_pass(mRenderPass, 0)
          .with_layout(mPipelineLayout)
          .with_cache(mPipelineCache)
          .vertex_shader("shaders/triangle.vert.spv")
          .fragment_shader("shaders/triangle.frag.spv")
          .vertex_input_binding(0, sizeof(Vertex))
          .vertex_attribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position))
          .vertex_attribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color))
          .rasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE)
          .depth_test(true)
          .depth_write(true)
          .dynamic_state(VK_DYNAMIC_STATE_VIEWPORT)
          .dynamic_state(VK_DYNAMIC_STATE_SCISSOR)
          .color_blend_attachment(false)
          .build(&result);
  if (!mPipeline) {
    throw std::runtime_error {"Could not create pipeline"};
  }

  mMatrixBuffers.reserve(kMaxFramesInFlight);
  for (usize i = 0; i < kMaxFramesInFlight; ++i) {
    auto matrix_buffer = Buffer::for_uniforms(mAllocator, sizeof(Matrices), &result);
    if (!matrix_buffer) {
      throw std::runtime_error {"Could not create matrix buffer"};
    }

    mMatrixBuffers.push_back(std::move(matrix_buffer));
  }

  const CommandContext cmd_context {mDevice, mGraphicsQueue, mGraphicsCommandPool};

  const Vertex vertices[] = {
      {.position {1.0f, 1.0f, 0.0f}, .color = {1.0f, 0.0f, 0.0f}},
      {.position {-1.0f, 1.0f, 0.0f}, .color = {0.0f, 1.0f, 0.0f}},
      {.position {0.0f, -1.0f, 0.0f}, .color = {0.0f, 0.0f, 1.0f}},
  };

  const uint32 indices[] = {0, 1, 2};

  mTriangleVertexBuffer = Buffer::on_gpu(cmd_context,
                                         mAllocator,
                                         vertices,
                                         sizeof vertices,
                                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                         &result);
  if (!mTriangleVertexBuffer) {
    throw std::runtime_error {"Could not create triangle vertex buffer"};
  }

  mTriangleIndexBuffer = Buffer::on_gpu(cmd_context,
                                        mAllocator,
                                        indices,
                                        sizeof indices,
                                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                        &result);
  if (!mTriangleIndexBuffer) {
    throw std::runtime_error {"Could not create triangle index buffer"};
  }
}

void TriangleExample::record_commands()
{
  const auto& swapchain_info = mSwapchain.info();
  auto& frame = mFrames.at(mFrameIndex);

  const auto aspect_ratio = static_cast<float>(swapchain_info.image_extent.width) /
                            static_cast<float>(swapchain_info.image_extent.height);
  mMatrices.projection =
      glm::perspective(glm::radians(mFOV), aspect_ratio, mNearPlane, mFarPlane);
  mMatrices.view = glm::lookAt(mCameraPos, mCameraPos + mCameraDir, mWorldUp);

  auto& matrix_buffer = mMatrixBuffers.at(mFrameIndex);
  matrix_buffer.set_data(&mMatrices, sizeof mMatrices);

  vkCmdBindPipeline(frame.cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

  const auto matrix_buffer_info =
      make_descriptor_buffer_info(matrix_buffer, sizeof(Matrices));

  const VkWriteDescriptorSet descriptor_writes[] = {
      make_buffer_descriptor_write(VK_NULL_HANDLE,
                                   0,
                                   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   1,
                                   &matrix_buffer_info),
  };

  mFunctions.vkCmdPushDescriptorSetKHR(frame.cmd_buffer,
                                       VK_PIPELINE_BIND_POINT_GRAPHICS,
                                       mPipelineLayout,
                                       0,
                                       u32_size(descriptor_writes),
                                       descriptor_writes);

  mPushConstants.model_matrix = glm::mat4 {1.0f};
  vkCmdPushConstants(frame.cmd_buffer,
                     mPipelineLayout,
                     VK_SHADER_STAGE_VERTEX_BIT,
                     0,
                     sizeof mPushConstants,
                     &mPushConstants);

  mTriangleVertexBuffer.bind_as_vertex_buffer(frame.cmd_buffer);
  mTriangleIndexBuffer.bind_as_index_buffer(frame.cmd_buffer, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(frame.cmd_buffer, 3, 1, 0, 0, 0);
}

}  // namespace grace::examples
