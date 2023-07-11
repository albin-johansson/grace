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

#include <vector>  // vector

#include <glm/glm.hpp>
#include <grace/grace.hpp>
#include <vulkan/vulkan.h>

#include "example.hpp"

namespace grace::examples {

class TriangleExample final : public Example {
  struct Vertex final {
    glm::vec3 position {};
    glm::vec3 color {};
  };

  struct Matrices final {
    glm::mat4 projection {1.0f};
    glm::mat4 view {1.0f};
  };

  struct PushConstants final {
    glm::mat4 model_matrix {1.0f};
  };

 public:
  TriangleExample();

 protected:
  void record_commands() override;

 private:
  DescriptorSetLayout mDescriptorSetLayout;
  PipelineLayout mPipelineLayout;
  GraphicsPipeline mPipeline;
  std::vector<Buffer> mMatrixBuffers;
  Buffer mTriangleVertexBuffer;
  Buffer mTriangleIndexBuffer;
  Matrices mMatrices;
  PushConstants mPushConstants;
  glm::vec3 mCameraPos {0, 0, -2.5f};
  glm::vec3 mCameraDir {0, 0, 1};
  glm::vec3 mWorldUp {0, 1, 0};
  float mFOV {60};
  float mNearPlane {0.1f};
  float mFarPlane {10'000};
};

}  // namespace grace::examples

GRACE_EXAMPLE_DEFINE_MAIN(grace::examples::TriangleExample)
