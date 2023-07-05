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

#pragma once

#include <string>  // string

#include <vulkan/vulkan.h>

#include "common.hpp"

namespace grace {

/**
 * Creates a shader module specification.
 *
 * \param code      the compiled shader code.
 * \param code_size the size of the code in bytes.
 *
 * \return a shader module specification.
 */
[[nodiscard]] auto make_shader_module_info(const void* code, usize code_size)
    -> VkShaderModuleCreateInfo;

/**
 * Attempts to read a binary file.
 *
 * \param      file_path the file path to a binary file.
 * \param[out] result    the resulting error code.
 *
 * \return a (potentially empty) string representing a character stream.
 */
[[nodiscard]] auto read_binary_file(const char* file_path, VkResult* result = nullptr)
    -> std::string;

class ShaderModule final {
 public:
  [[nodiscard]] static auto make(VkDevice device,
                                 const VkShaderModuleCreateInfo& module_info,
                                 VkResult* result = nullptr) -> ShaderModule;

  [[nodiscard]] static auto make(VkDevice device,
                                 const void* code,
                                 usize code_size,
                                 VkResult* result = nullptr) -> ShaderModule;

  [[nodiscard]] static auto read(VkDevice device,
                                 const char* code_path,
                                 VkResult* result = nullptr) -> ShaderModule;

  ShaderModule() noexcept = default;

  ShaderModule(VkDevice device, VkShaderModule shader_module) noexcept;

  ShaderModule(ShaderModule&& other) noexcept;
  ShaderModule(const ShaderModule& other) = delete;

  auto operator=(ShaderModule&& other) noexcept -> ShaderModule&;
  auto operator=(const ShaderModule& other) -> ShaderModule& = delete;

  ~ShaderModule() noexcept;

  void destroy() noexcept;

  [[nodiscard]] auto get() noexcept -> VkShaderModule { return mShaderModule; }

  [[nodiscard]] auto device() noexcept -> VkDevice { return mDevice; }

  [[nodiscard]] operator VkShaderModule() noexcept { return mShaderModule; }

  [[nodiscard]] explicit operator bool() const noexcept
  {
    return mShaderModule != VK_NULL_HANDLE;
  }

 private:
  VkDevice mDevice {VK_NULL_HANDLE};
  VkShaderModule mShaderModule {VK_NULL_HANDLE};
};

}  // namespace grace
