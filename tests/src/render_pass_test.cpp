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

#include "grace/render_pass.hpp"

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

GRACE_TEST_FIXTURE(RenderPassFixture);

TEST_F(RenderPassFixture, MakeAttachmentDescription)
{
  const auto format = VK_FORMAT_B8G8R8A8_UNORM;
  const auto initial_layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  const auto final_layout = VK_IMAGE_LAYOUT_GENERAL;
  const auto samples = VK_SAMPLE_COUNT_2_BIT;

  const auto description =
      make_attachment_description(format, initial_layout, final_layout, samples);

  EXPECT_EQ(description.format, format);
  EXPECT_EQ(description.initialLayout, initial_layout);
  EXPECT_EQ(description.finalLayout, final_layout);
  EXPECT_EQ(description.samples, samples);
  EXPECT_EQ(description.flags, 0);
  EXPECT_EQ(description.loadOp, VK_ATTACHMENT_LOAD_OP_CLEAR);
  EXPECT_EQ(description.storeOp, VK_ATTACHMENT_STORE_OP_STORE);
  EXPECT_EQ(description.stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
  EXPECT_EQ(description.stencilStoreOp, VK_ATTACHMENT_STORE_OP_DONT_CARE);
}

TEST_F(RenderPassFixture, MakeSubpassDependency)
{
  const uint32 src_subpass = 4;
  const uint32 dst_subpass = 8;
  const VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  const VkPipelineStageFlags dst_stages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  const VkAccessFlags src_access = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
  const VkAccessFlags dst_access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  const auto dependency = make_subpass_dependency(src_subpass,
                                                  dst_subpass,
                                                  src_stages,
                                                  dst_stages,
                                                  src_access,
                                                  dst_access);

  EXPECT_EQ(dependency.srcSubpass, src_subpass);
  EXPECT_EQ(dependency.dstSubpass, dst_subpass);
  EXPECT_EQ(dependency.srcStageMask, src_stages);
  EXPECT_EQ(dependency.dstStageMask, dst_stages);
  EXPECT_EQ(dependency.srcAccessMask, src_access);
  EXPECT_EQ(dependency.dstAccessMask, dst_access);
  EXPECT_EQ(dependency.dependencyFlags, 0);
}

TEST_F(RenderPassFixture, MakeAttachmentReference)
{
  const uint32 attachment = 42;
  const auto layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  const auto reference = make_attachment_reference(attachment, layout);

  EXPECT_EQ(reference.attachment, attachment);
  EXPECT_EQ(reference.layout, layout);
}

TEST_F(RenderPassFixture, MakeRenderPassInfo)
{
  const std::vector attachments = {VkAttachmentDescription {}};
  const std::vector subpasses = {VkSubpassDescription {}, VkSubpassDescription {}};
  const std::vector dependencies = {VkSubpassDependency {},
                                    VkSubpassDependency {},
                                    VkSubpassDependency {}};

  const auto info = make_render_pass_info(attachments, subpasses, dependencies);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);
  EXPECT_EQ(info.attachmentCount, u32_size(attachments));
  EXPECT_EQ(info.pAttachments, attachments.data());
  EXPECT_EQ(info.subpassCount, u32_size(subpasses));
  EXPECT_EQ(info.pSubpasses, subpasses.data());
  EXPECT_EQ(info.dependencyCount, u32_size(dependencies));
  EXPECT_EQ(info.pDependencies, dependencies.data());
}

TEST_F(RenderPassFixture, MakeRenderPassBeginInfo)
{
  VkRenderPass render_pass = make_fake_ptr<VkRenderPass>(1'024);
  VkFramebuffer framebuffer = make_fake_ptr<VkFramebuffer>(2'048);
  const VkRect2D render_area = {{10, 20}, {100, 150}};
  VkClearValue clear_values[1] {};
  clear_values[0].color = {.float32 = {0.0f, 0.0f, 0.0f, 1.0f}};

  const auto info =
      make_render_pass_begin_info(render_pass, framebuffer, render_area, clear_values, 1);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.renderPass, render_pass);
  EXPECT_EQ(info.framebuffer, framebuffer);
  EXPECT_EQ(info.renderArea.offset.x, render_area.offset.x);
  EXPECT_EQ(info.renderArea.offset.y, render_area.offset.y);
  EXPECT_EQ(info.renderArea.extent.width, render_area.extent.width);
  EXPECT_EQ(info.renderArea.extent.height, render_area.extent.height);
  EXPECT_EQ(info.clearValueCount, 1);
  EXPECT_EQ(info.pClearValues, clear_values);
}

TEST_F(RenderPassFixture, Defaults)
{
  RenderPass render_pass;
  EXPECT_FALSE(render_pass);
  EXPECT_EQ(render_pass.device(), VK_NULL_HANDLE);
  EXPECT_EQ(render_pass.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkRenderPass>(render_pass), VK_NULL_HANDLE);
  EXPECT_NO_THROW(render_pass.destroy());
}

TEST_F(RenderPassFixture, RenderPassBuilderMinimalRenderPass)
{
  VkResult result = VK_ERROR_UNKNOWN;
  auto render_pass = RenderPassBuilder {mDevice}  //
                         .begin_subpass()
                         .end_subpass()
                         .build(&result);

  ASSERT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(render_pass);
  EXPECT_EQ(render_pass.device(), mDevice);
  EXPECT_NE(render_pass.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkRenderPass>(render_pass), render_pass.get());

  render_pass.destroy();
  EXPECT_FALSE(render_pass);
  EXPECT_EQ(render_pass.device(), mDevice);
  EXPECT_EQ(render_pass.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkRenderPass>(render_pass), VK_NULL_HANDLE);
}

TEST_F(RenderPassFixture, RenderPassBuilderAdvancedRenderPass)
{
  const auto subpass_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  const auto main_subpass_access =
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  const auto dependency = make_subpass_dependency(VK_SUBPASS_EXTERNAL,
                                                  0,
                                                  subpass_stages,
                                                  subpass_stages,
                                                  0,
                                                  main_subpass_access);

  RenderPassBuilder builder {mDevice};

  // clang-format off
  VkResult result = VK_ERROR_UNKNOWN;
  auto render_pass = builder
                         .color_attachment(VK_FORMAT_B8G8R8A8_UNORM)      // 0
                         .color_attachment(VK_FORMAT_B8G8R8A8_UNORM)      // 1
                         .color_attachment(VK_FORMAT_B8G8R8A8_UNORM)      // 2
                         .depth_attachment(VK_FORMAT_D32_SFLOAT_S8_UINT)  // 3
                         .begin_subpass()
                           .use_color_attachment(0)
                           .use_color_attachment(1)
                           .use_depth_attachment(3)
                         .end_subpass()
                         .begin_subpass()
                           .use_color_attachment(2)
                           .use_depth_attachment(3)
                         .end_subpass()
                         .subpass_dependency(dependency)
                         .build(&result);
  // clang-format on

  ASSERT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(render_pass);

  const auto subpass_descriptions = builder.get_subpass_descriptions();
  const auto render_pass_info = builder.get_render_pass_info(subpass_descriptions);

  EXPECT_EQ(render_pass_info.sType, VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO);
  EXPECT_EQ(render_pass_info.pNext, nullptr);
  EXPECT_EQ(render_pass_info.flags, 0);
  EXPECT_EQ(render_pass_info.subpassCount, 2);
  EXPECT_EQ(render_pass_info.attachmentCount, 4);
  EXPECT_EQ(render_pass_info.dependencyCount, 1);
  EXPECT_EQ(render_pass_info.pSubpasses, subpass_descriptions.data());
  EXPECT_NE(render_pass_info.pAttachments, nullptr);
  EXPECT_NE(render_pass_info.pDependencies, nullptr);
}
