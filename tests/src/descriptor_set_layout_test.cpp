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

#include "grace/descriptor_set_layout.hpp"

#include <optional>  // optional

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

class DescriptorSetLayoutFixture : public testing::Test {
 public:
  static void SetUpTestSuite() { mCtx = make_test_context(); }

  static void TearDownTestSuite() { mCtx.reset(); }

 protected:
  inline static std::optional<TestContext> mCtx;
};

TEST_F(DescriptorSetLayoutFixture, MakeDescriptorSetLayoutBinding)
{
  const uint32 index = 12;
  const auto type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  const auto stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  const uint32 count = 4;

  const auto binding = make_descriptor_set_layout_binding(index, type, stages, count);

  EXPECT_EQ(binding.binding, index);
  EXPECT_EQ(binding.descriptorType, type);
  EXPECT_EQ(binding.stageFlags, stages);
  EXPECT_EQ(binding.descriptorCount, count);
  EXPECT_EQ(binding.pImmutableSamplers, nullptr);
}

TEST_F(DescriptorSetLayoutFixture, MakeDescriptorSetLayoutBindingFlagsInfo)
{
  const auto flag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
  const std::vector<VkDescriptorBindingFlags> flags(4, flag);

  const auto flags_info = make_descriptor_set_layout_binding_flags_info(flags);

  EXPECT_EQ(flags_info.sType,
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO);
  EXPECT_EQ(flags_info.pNext, nullptr);
  EXPECT_EQ(flags_info.pBindingFlags, flags.data());
  EXPECT_EQ(flags_info.bindingCount, u32_size(flags));
}

TEST_F(DescriptorSetLayoutFixture, MakeDescriptorSetLayoutInfo)
{
  const std::vector<VkDescriptorSetLayoutBinding> bindings = {
      {
          .binding = 42,
          .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
          .descriptorCount = 1,
          .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
          .pImmutableSamplers = nullptr,
      },
  };

  const auto layout_info = make_descriptor_set_layout_info(bindings);

  EXPECT_EQ(layout_info.sType, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
  EXPECT_EQ(layout_info.flags, 0);
  EXPECT_EQ(layout_info.pNext, nullptr);
  EXPECT_EQ(layout_info.pBindings, bindings.data());
  EXPECT_EQ(layout_info.bindingCount, u32_size(bindings));
}

TEST_F(DescriptorSetLayoutFixture, Defaults)
{
  DescriptorSetLayout layout;
  EXPECT_FALSE(layout);
  EXPECT_EQ(layout.get(), VK_NULL_HANDLE);
  EXPECT_EQ(layout.device(), VK_NULL_HANDLE);
  EXPECT_NO_THROW(layout.destroy());
}

TEST_F(DescriptorSetLayoutFixture, Make)
{
  const std::vector<VkDescriptorSetLayoutBinding> bindings = {};
  const auto layout_info = make_descriptor_set_layout_info(bindings);

  VkResult result = VK_ERROR_UNKNOWN;
  auto layout = DescriptorSetLayout::make(mCtx->device, layout_info, &result);

  ASSERT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(layout);
  EXPECT_NE(layout.get(), VK_NULL_HANDLE);
  EXPECT_NE(layout.device(), VK_NULL_HANDLE);

  layout.destroy();
  EXPECT_FALSE(layout);
  EXPECT_EQ(layout.get(), VK_NULL_HANDLE);
  EXPECT_NE(layout.device(), VK_NULL_HANDLE);
}

TEST_F(DescriptorSetLayoutFixture, Build)
{
  DescriptorSetLayoutBuilder builder {mCtx->device};

  VkResult result = VK_ERROR_UNKNOWN;
  auto layout =
      builder.reset()
          .use_push_descriptors()
          .allow_partially_bound_descriptors()
          .descriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
          .descriptor(1,
                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                      VK_SHADER_STAGE_FRAGMENT_BIT)
          .build(&result);

  ASSERT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(layout);
  EXPECT_NE(layout.get(), VK_NULL_HANDLE);
  EXPECT_EQ(layout.device(), mCtx->device.get());
}
