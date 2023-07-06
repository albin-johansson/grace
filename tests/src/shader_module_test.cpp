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

#include "grace/shader_module.hpp"

#include <gtest/gtest.h>

#include "test_utils.hpp"

using namespace grace;

static_assert(WrapperType<ShaderModule, VkShaderModule>);

GRACE_TEST_FIXTURE(ShaderModuleFixture);

TEST_F(ShaderModuleFixture, Defaults)
{
  ShaderModule shader_module;
  EXPECT_FALSE(shader_module);
  EXPECT_EQ(shader_module.get(), VK_NULL_HANDLE);
  EXPECT_EQ(shader_module.device(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkShaderModule>(shader_module), VK_NULL_HANDLE);
  EXPECT_NO_THROW(shader_module.destroy());
}

TEST_F(ShaderModuleFixture, ReadWithNonExistentShader)
{
  VkResult result = VK_ERROR_UNKNOWN;
  auto shader_module = ShaderModule::read(mDevice, "missing.vert.spv", &result);

  ASSERT_NE(result, VK_SUCCESS);
  EXPECT_FALSE(shader_module);
  EXPECT_EQ(shader_module.device(), VK_NULL_HANDLE);
  EXPECT_EQ(shader_module.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkShaderModule>(shader_module), VK_NULL_HANDLE);
}

TEST_F(ShaderModuleFixture, ReadWithValidShader)
{
  VkResult result = VK_ERROR_UNKNOWN;
  auto shader_module =
      ShaderModule::read(mDevice, "assets/shaders/test.vert.spv", &result);

  ASSERT_EQ(result, VK_SUCCESS);
  EXPECT_TRUE(shader_module);
  EXPECT_EQ(shader_module.device(), mDevice);
  EXPECT_NE(shader_module.get(), VK_NULL_HANDLE);
  EXPECT_EQ(static_cast<VkShaderModule>(shader_module), shader_module.get());

  shader_module.destroy();
  EXPECT_FALSE(shader_module);
  EXPECT_EQ(shader_module.device(), mDevice);
  EXPECT_EQ(shader_module.get(), VK_NULL_HANDLE);
}
