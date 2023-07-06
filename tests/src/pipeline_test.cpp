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

#include "grace/pipeline.hpp"

#include <gtest/gtest.h>

#include "grace/descriptor_set_layout.hpp"
#include "grace/pipeline_layout.hpp"
#include "grace/render_pass.hpp"
#include "test_utils.hpp"

using namespace grace;

GRACE_TEST_FIXTURE(PipelineFixture);

TEST_F(PipelineFixture, MakeViewport)
{
  const float x = 123;
  const float y = 83;
  const float width = 332;
  const float height = 684;
  const float min_depth = 0.0f;
  const float max_depth = 1.0f;

  const auto viewport = make_viewport(x, y, width, height, min_depth, max_depth);

  EXPECT_EQ(viewport.x, x);
  EXPECT_EQ(viewport.y, y);
  EXPECT_EQ(viewport.width, width);
  EXPECT_EQ(viewport.height, height);
  EXPECT_EQ(viewport.minDepth, min_depth);
  EXPECT_EQ(viewport.maxDepth, max_depth);
}

TEST_F(PipelineFixture, MakeRect2D)
{
  const int32 x = -24;
  const int32 y = 812;
  const uint32 width = 54;
  const uint32 height = 281;

  const auto rect = make_rect_2d(x, y, width, height);

  EXPECT_EQ(rect.offset.x, x);
  EXPECT_EQ(rect.offset.y, y);
  EXPECT_EQ(rect.extent.width, width);
  EXPECT_EQ(rect.extent.height, height);
}

TEST_F(PipelineFixture, MakePipelineShaderStageInfo)
{
  const auto stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  const VkSpecializationInfo specialization_info = {};
  const char* entry_name = "custom_main";

  const auto info = make_pipeline_shader_stage_info(stage,
                                                    VK_NULL_HANDLE,
                                                    &specialization_info,
                                                    entry_name);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);
  EXPECT_EQ(info.stage, stage);
  EXPECT_EQ(info.module, nullptr);
  EXPECT_EQ(info.pSpecializationInfo, &specialization_info);
  EXPECT_STREQ(info.pName, entry_name);
}

TEST_F(PipelineFixture, MakePipelineVertexInputStateInfo)
{
  const VkVertexInputBindingDescription binding_desc = {
      .binding = 42,
      .stride = 128,
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

  const VkVertexInputAttributeDescription attribute_desc = {
      .location = 1,
      .binding = 2,
      .format = VK_FORMAT_R32G32B32A32_SFLOAT,
      .offset = 3,
  };

  const std::vector bindings = {binding_desc};
  const std::vector attributes = {attribute_desc};
  const auto info = make_pipeline_vertex_input_state_info(bindings, attributes);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);

  EXPECT_EQ(info.vertexBindingDescriptionCount, 1);
  EXPECT_EQ(info.pVertexAttributeDescriptions, attributes.data());

  EXPECT_EQ(info.vertexAttributeDescriptionCount, 1);
  EXPECT_EQ(info.pVertexBindingDescriptions, bindings.data());
}

TEST_F(PipelineFixture, MakePipelineInputAssemblyStateInfo)
{
  const auto topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  const auto info = make_pipeline_input_assembly_state_info(topology);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);
  EXPECT_EQ(info.topology, topology);
  EXPECT_EQ(info.primitiveRestartEnable, VK_FALSE);
}

TEST_F(PipelineFixture, MakePipelineTessellationStateInfo)
{
  const uint32 patch_control_points = 42;
  const auto info = make_pipeline_tessellation_state_info(patch_control_points);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);
  EXPECT_EQ(info.patchControlPoints, patch_control_points);
}

TEST_F(PipelineFixture, MakePipelineViewportStateInfo)
{
  const std::vector viewports = {VkViewport {}, VkViewport {}};
  const std::vector scissors = {VkRect2D {}, VkRect2D {}, VkRect2D {}};
  const auto info = make_pipeline_viewport_state_info(viewports, scissors);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);
  EXPECT_EQ(info.viewportCount, u32_size(viewports));
  EXPECT_EQ(info.pViewports, viewports.data());
  EXPECT_EQ(info.scissorCount, u32_size(scissors));
  EXPECT_EQ(info.pScissors, scissors.data());
}

TEST_F(PipelineFixture, MakePipelineColorBlendStateInfo)
{
  const auto enabled = VK_TRUE;
  const auto logic_op = VK_LOGIC_OP_AND;
  const std::vector attachments = {VkPipelineColorBlendAttachmentState {},
                                   VkPipelineColorBlendAttachmentState {}};
  const std::array blend_constants = {0.1f, 0.2f, 0.3f, 0.4f};

  const auto info = make_pipeline_color_blend_state_info(enabled,
                                                         logic_op,
                                                         attachments,
                                                         blend_constants);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);
  EXPECT_EQ(info.logicOpEnable, enabled);
  EXPECT_EQ(info.logicOp, logic_op);
  EXPECT_EQ(info.attachmentCount, u32_size(attachments));
  EXPECT_EQ(info.pAttachments, attachments.data());
  EXPECT_EQ(info.blendConstants[0], blend_constants[0]);
  EXPECT_EQ(info.blendConstants[1], blend_constants[1]);
  EXPECT_EQ(info.blendConstants[2], blend_constants[2]);
  EXPECT_EQ(info.blendConstants[3], blend_constants[3]);
}

TEST_F(PipelineFixture, MakePipelineDynamicStateInfo)
{
  const std::vector states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  const auto info = make_pipeline_dynamic_state_info(states);

  EXPECT_EQ(info.sType, VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
  EXPECT_EQ(info.pNext, nullptr);
  EXPECT_EQ(info.flags, 0);
  EXPECT_EQ(info.dynamicStateCount, u32_size(states));
  EXPECT_EQ(info.pDynamicStates, states.data());
}

TEST_F(PipelineFixture, PipelineDefaults)
{
  Pipeline pipeline;
  EXPECT_FALSE(pipeline);
  EXPECT_EQ(static_cast<VkPipeline>(pipeline), VK_NULL_HANDLE);
  EXPECT_EQ(pipeline.get(), VK_NULL_HANDLE);
  EXPECT_EQ(pipeline.device(), VK_NULL_HANDLE);
  EXPECT_NO_THROW(pipeline.destroy());
}

TEST_F(PipelineFixture, GraphicsPipelineBuilderDefaults)
{
  const auto builder = GraphicsPipelineBuilder {mDevice};

  {
    const auto vertex_input_state = builder.get_vertex_input_state_info();
    EXPECT_EQ(vertex_input_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO);
    EXPECT_EQ(vertex_input_state.pNext, nullptr);
    EXPECT_EQ(vertex_input_state.flags, 0);
    EXPECT_EQ(vertex_input_state.vertexBindingDescriptionCount, 0);
    EXPECT_EQ(vertex_input_state.vertexAttributeDescriptionCount, 0);
    EXPECT_EQ(vertex_input_state.pVertexBindingDescriptions, nullptr);
    EXPECT_EQ(vertex_input_state.pVertexAttributeDescriptions, nullptr);
  }

  {
    const auto input_assembly_state = builder.get_input_assembly_state_info();
    EXPECT_EQ(input_assembly_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO);
    EXPECT_EQ(input_assembly_state.pNext, nullptr);
    EXPECT_EQ(input_assembly_state.flags, 0);
    EXPECT_EQ(input_assembly_state.topology, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    EXPECT_EQ(input_assembly_state.primitiveRestartEnable, VK_FALSE);
  }

  {
    const auto tessellation_state = builder.get_tessellation_state_info();
    EXPECT_EQ(tessellation_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO);
    EXPECT_EQ(tessellation_state.pNext, nullptr);
    EXPECT_EQ(tessellation_state.flags, 0);
    EXPECT_EQ(tessellation_state.patchControlPoints, 0);
  }

  {
    const auto viewport_state = builder.get_viewport_state_info();
    EXPECT_EQ(viewport_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO);
    EXPECT_EQ(viewport_state.pNext, nullptr);
    EXPECT_EQ(viewport_state.flags, 0);
    EXPECT_EQ(viewport_state.pViewports, nullptr);
    EXPECT_EQ(viewport_state.pScissors, nullptr);
    EXPECT_EQ(viewport_state.viewportCount, 0);
    EXPECT_EQ(viewport_state.scissorCount, 0);
  }

  {
    const auto rasterization_state = builder.get_rasterization_state_info();
    EXPECT_EQ(rasterization_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO);
    EXPECT_EQ(rasterization_state.pNext, nullptr);
    EXPECT_EQ(rasterization_state.flags, 0);
    EXPECT_EQ(rasterization_state.polygonMode, VK_POLYGON_MODE_FILL);
    EXPECT_EQ(rasterization_state.cullMode, VK_CULL_MODE_BACK_BIT);
    EXPECT_EQ(rasterization_state.frontFace, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    EXPECT_EQ(rasterization_state.rasterizerDiscardEnable, VK_FALSE);
    EXPECT_EQ(rasterization_state.depthClampEnable, VK_FALSE);
    EXPECT_EQ(rasterization_state.depthBiasEnable, VK_FALSE);
    EXPECT_EQ(rasterization_state.depthBiasConstantFactor, 0);
    EXPECT_EQ(rasterization_state.depthBiasSlopeFactor, 0);
    EXPECT_EQ(rasterization_state.depthBiasClamp, 0);
    EXPECT_EQ(rasterization_state.lineWidth, 1.0f);
  }

  {
    const auto multisample_state = builder.get_multisample_state_info();
    EXPECT_EQ(multisample_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO);
    EXPECT_EQ(multisample_state.pNext, nullptr);
    EXPECT_EQ(multisample_state.flags, 0);
    EXPECT_EQ(multisample_state.rasterizationSamples, VK_SAMPLE_COUNT_1_BIT);
    EXPECT_EQ(multisample_state.pSampleMask, nullptr);
    EXPECT_EQ(multisample_state.minSampleShading, 0.0f);
    EXPECT_EQ(multisample_state.sampleShadingEnable, VK_FALSE);
    EXPECT_EQ(multisample_state.alphaToCoverageEnable, VK_FALSE);
    EXPECT_EQ(multisample_state.alphaToOneEnable, VK_FALSE);
  }

  {
    const auto depth_stencil_state = builder.get_depth_stencil_state_info();
    EXPECT_EQ(depth_stencil_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO);
    EXPECT_EQ(depth_stencil_state.pNext, nullptr);
    EXPECT_EQ(depth_stencil_state.flags, 0);
    EXPECT_EQ(depth_stencil_state.depthCompareOp, VK_COMPARE_OP_LESS);
    EXPECT_EQ(depth_stencil_state.depthTestEnable, VK_FALSE);
    EXPECT_EQ(depth_stencil_state.depthWriteEnable, VK_FALSE);
    EXPECT_EQ(depth_stencil_state.depthBoundsTestEnable, VK_FALSE);
    EXPECT_EQ(depth_stencil_state.stencilTestEnable, VK_FALSE);
    EXPECT_EQ(depth_stencil_state.minDepthBounds, 0.0f);
    EXPECT_EQ(depth_stencil_state.maxDepthBounds, 1.0f);

    EXPECT_EQ(depth_stencil_state.front.depthFailOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.front.failOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.front.passOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.front.depthFailOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.front.compareOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.front.compareMask, 0);
    EXPECT_EQ(depth_stencil_state.front.writeMask, 0);
    EXPECT_EQ(depth_stencil_state.front.reference, 0);

    EXPECT_EQ(depth_stencil_state.back.depthFailOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.back.failOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.back.passOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.back.depthFailOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.back.compareOp, VkStencilOp {});
    EXPECT_EQ(depth_stencil_state.back.compareMask, 0);
    EXPECT_EQ(depth_stencil_state.back.writeMask, 0);
    EXPECT_EQ(depth_stencil_state.back.reference, 0);
  }

  {
    const auto color_blend_state = builder.get_color_blend_state_info();
    EXPECT_EQ(color_blend_state.sType,
              VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO);
    EXPECT_EQ(color_blend_state.pNext, nullptr);
    EXPECT_EQ(color_blend_state.flags, 0);
    EXPECT_EQ(color_blend_state.logicOpEnable, VK_FALSE);
    EXPECT_EQ(color_blend_state.logicOp, VK_LOGIC_OP_NO_OP);
    EXPECT_EQ(color_blend_state.attachmentCount, 0);
    EXPECT_EQ(color_blend_state.pAttachments, nullptr);
    EXPECT_EQ(color_blend_state.blendConstants[0], 0);
    EXPECT_EQ(color_blend_state.blendConstants[1], 0);
    EXPECT_EQ(color_blend_state.blendConstants[2], 0);
    EXPECT_EQ(color_blend_state.blendConstants[3], 0);
  }

  {
    const auto dynamic_state = builder.get_dynamic_state_info();
    EXPECT_EQ(dynamic_state.sType, VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO);
    EXPECT_EQ(dynamic_state.pNext, nullptr);
    EXPECT_EQ(dynamic_state.flags, 0);
    EXPECT_EQ(dynamic_state.dynamicStateCount, 0);
    EXPECT_EQ(dynamic_state.pDynamicStates, nullptr);
  }
}

TEST_F(PipelineFixture, GraphicsPipelineBuilderMinimalPipeline)
{
  VkResult result = VK_ERROR_UNKNOWN;

  const auto subpass_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  auto render_pass =
      RenderPassBuilder {mDevice}
          .color_attachment(VK_FORMAT_B8G8R8A8_UNORM)
          .begin_subpass()
          .set_color_attachment(0)
          .end_subpass()
          .subpass_dependency(VK_SUBPASS_EXTERNAL,
                              0,
                              subpass_stages,
                              subpass_stages,
                              0,
                              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)
          .build(&result);

  ASSERT_EQ(result, VK_SUCCESS);
  ASSERT_TRUE(render_pass);

  auto descriptor_set_layout =
      DescriptorSetLayoutBuilder {mDevice}
          .use_push_descriptors()
          .descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
          .build(&result);

  ASSERT_EQ(result, VK_SUCCESS);
  ASSERT_TRUE(descriptor_set_layout);

  auto pipeline_layout =
      PipelineLayoutBuilder {mDevice}
          .descriptor_set_layout(descriptor_set_layout)
          .push_constant(VK_SHADER_STAGE_VERTEX_BIT, 0, 16 * sizeof(float))
          .build(&result);

  ASSERT_EQ(result, VK_SUCCESS);
  ASSERT_TRUE(pipeline_layout);

  auto pipeline =
      GraphicsPipelineBuilder {mDevice}
          .with_layout(pipeline_layout)
          .with_render_pass(render_pass, 0)
          .vertex_shader("assets/shaders/test.vert.spv")
          .fragment_shader("assets/shaders/test.frag.spv")
          .vertex_input_binding(0, 8 * sizeof(float))
          .vertex_attribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
          .vertex_attribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float))
          .vertex_attribute(0, 2, VK_FORMAT_R32G32_SFLOAT, 6 * sizeof(float))
          .color_blend_attachment(false)
          .viewport(0, 0, 800, 600)
          .scissor(0, 0, 800, 600)
          .build(&result);

  ASSERT_EQ(result, VK_SUCCESS);
  ASSERT_TRUE(pipeline);
}