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

#include <fstream>   // ifstream
#include <ios>       // ios
#include <iterator>  // istreambuf_iterator

namespace grace {

auto make_shader_module_info(const void* code, const usize code_size)
    -> VkShaderModuleCreateInfo
{
  return {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .codeSize = code_size,
      .pCode = reinterpret_cast<const uint32*>(code),
  };
}

auto read_binary_file(const char* file_path, VkResult* result) -> std::string
{
  std::ifstream stream {file_path, std::ios::in | std::ios::binary};

  if (stream.is_open() && stream.good()) {
    std::string code {std::istreambuf_iterator {stream},
                      std::istreambuf_iterator<char> {}};

    if (result) {
      *result = VK_SUCCESS;
    }

    return code;
  }

  if (result) {
    *result = VK_ERROR_UNKNOWN;
  }

  return {};
}

ShaderModule::ShaderModule(VkDevice device, VkShaderModule shader_module) noexcept
    : mDevice {device},
      mShaderModule {shader_module}
{
}

ShaderModule::ShaderModule(ShaderModule&& other) noexcept
    : mDevice {other.mDevice},
      mShaderModule {other.mShaderModule}
{
  other.mDevice = VK_NULL_HANDLE;
  other.mShaderModule = VK_NULL_HANDLE;
}

auto ShaderModule::operator=(ShaderModule&& other) noexcept -> ShaderModule&
{
  if (this != &other) {
    destroy();

    mDevice = other.mDevice;
    mShaderModule = other.mShaderModule;

    other.mDevice = VK_NULL_HANDLE;
    other.mShaderModule = VK_NULL_HANDLE;
  }

  return *this;
}

ShaderModule::~ShaderModule() noexcept
{
  destroy();
}

void ShaderModule::destroy() noexcept
{
  if (mShaderModule != VK_NULL_HANDLE) {
    vkDestroyShaderModule(mDevice, mShaderModule, nullptr);
    mShaderModule = VK_NULL_HANDLE;
  }
}

auto ShaderModule::make(VkDevice device,
                        const VkShaderModuleCreateInfo& module_info,
                        VkResult* result) -> ShaderModule
{
  VkShaderModule shader_module = VK_NULL_HANDLE;
  const auto status = vkCreateShaderModule(device, &module_info, nullptr, &shader_module);

  if (result) {
    *result = status;
  }

  if (status == VK_SUCCESS) {
    return ShaderModule {device, shader_module};
  }

  return {};
}

auto ShaderModule::make(VkDevice device,
                        const void* code,
                        const usize code_size,
                        VkResult* result) -> ShaderModule
{
  const auto shader_module_info = make_shader_module_info(code, code_size);
  return ShaderModule::make(device, shader_module_info, result);
}

auto ShaderModule::read(VkDevice device, const char* code_path, VkResult* result)
    -> ShaderModule
{
  VkResult parse_result = VK_ERROR_UNKNOWN;
  const auto code = read_binary_file(code_path, &parse_result);

  if (parse_result != VK_SUCCESS) {
    if (result) {
      *result = parse_result;
    }

    return {};
  }

  const auto shader_module_info = make_shader_module_info(code.data(), code.size());
  return ShaderModule::make(device, shader_module_info, result);
}

}  // namespace grace
