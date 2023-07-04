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

#include "grace/common.hpp"

namespace grace {

auto to_string(VkResult result) -> const char*
{
#define RESULT_CASE(X) \
  case VK_##X:         \
    return #X

  switch (result) {
    RESULT_CASE(SUCCESS);
    RESULT_CASE(NOT_READY);
    RESULT_CASE(TIMEOUT);
    RESULT_CASE(EVENT_SET);
    RESULT_CASE(EVENT_RESET);
    RESULT_CASE(INCOMPLETE);
    RESULT_CASE(ERROR_OUT_OF_HOST_MEMORY);
    RESULT_CASE(ERROR_OUT_OF_DEVICE_MEMORY);
    RESULT_CASE(ERROR_INITIALIZATION_FAILED);
    RESULT_CASE(ERROR_DEVICE_LOST);
    RESULT_CASE(ERROR_MEMORY_MAP_FAILED);
    RESULT_CASE(ERROR_LAYER_NOT_PRESENT);
    RESULT_CASE(ERROR_EXTENSION_NOT_PRESENT);
    RESULT_CASE(ERROR_FEATURE_NOT_PRESENT);
    RESULT_CASE(ERROR_INCOMPATIBLE_DRIVER);
    RESULT_CASE(ERROR_TOO_MANY_OBJECTS);
    RESULT_CASE(ERROR_FORMAT_NOT_SUPPORTED);
    RESULT_CASE(ERROR_FRAGMENTED_POOL);
    RESULT_CASE(ERROR_UNKNOWN);
    RESULT_CASE(ERROR_OUT_OF_POOL_MEMORY);
    RESULT_CASE(ERROR_INVALID_EXTERNAL_HANDLE);
    RESULT_CASE(ERROR_FRAGMENTATION);
    RESULT_CASE(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
    RESULT_CASE(PIPELINE_COMPILE_REQUIRED);
    RESULT_CASE(SUBOPTIMAL_KHR);         // From swapchain extension.
    RESULT_CASE(ERROR_OUT_OF_DATE_KHR);  // From swapchain extension.
    default:
      return "???";
  }
}

}  // namespace grace