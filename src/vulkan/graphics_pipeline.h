// Copyright 2018 The Amber Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_VULKAN_GRAPHICS_PIPELINE_H_
#define SRC_VULKAN_GRAPHICS_PIPELINE_H_

#include <memory>
#include <vector>

#include "amber/result.h"
#include "amber/value.h"
#include "amber/vulkan_header.h"
#include "src/buffer_data.h"
#include "src/format.h"
#include "src/vulkan/frame_buffer.h"
#include "src/vulkan/index_buffer.h"
#include "src/vulkan/pipeline.h"
#include "src/vulkan/vertex_buffer.h"

namespace amber {

class ProbeCommand;

namespace vulkan {

class CommandPool;

class GraphicsPipeline : public Pipeline {
 public:
  GraphicsPipeline(Device* device,
                   const VkPhysicalDeviceProperties& properties,
                   const VkPhysicalDeviceMemoryProperties& memory_properties,
                   VkFormat color_format,
                   VkFormat depth_stencil_format,
                   uint32_t fence_timeout_ms,
                   const std::vector<VkPipelineShaderStageCreateInfo>&);
  ~GraphicsPipeline() override;

  Result Initialize(uint32_t width,
                    uint32_t height,
                    CommandPool* pool,
                    VkQueue queue);

  Result SetIndexBuffer(const std::vector<Value>& values);

  Result Clear();
  Result ClearBuffer(const VkClearValue& clear_value,
                     VkImageAspectFlags aspect);
  VkFormat GetColorFormat() const { return color_format_; }
  VkFormat GetDepthStencilFormat() const { return depth_stencil_format_; }

  Result SetClearColor(float r, float g, float b, float a);
  Result SetClearStencil(uint32_t stencil);
  Result SetClearDepth(float depth);

  Result Draw(const DrawArraysCommand* command, VertexBuffer* vertex_buffer);

  const FrameBuffer* GetFrame() const { return frame_.get(); }

  uint32_t GetWidth() const { return frame_width_; }
  uint32_t GetHeight() const { return frame_height_; }

  void SetPatchControlPoints(uint32_t points) {
    patch_control_points_ = points;
  }

  // Pipeline
  void Shutdown() override;
  Result ProcessCommands() override;

 private:
  enum class RenderPassState : uint8_t {
    kActive = 0,
    kInactive,
  };

  Result CreateVkGraphicsPipeline(const PipelineData* pipeline_data,
                                  VkPrimitiveTopology topology,
                                  const VertexBuffer* vertex_buffer,
                                  const VkPipelineLayout& pipeline_layout,
                                  VkPipeline* pipeline);

  Result CreateRenderPass();
  Result ActivateRenderPassIfNeeded();
  void DeactivateRenderPassIfNeeded();

  Result SendVertexBufferDataIfNeeded(VertexBuffer* vertex_buffer);

  // TODO(jaebaek): Implement image/ssbo probe.
  Result SubmitProbeCommand();
  Result VerifyPixels(const uint32_t x,
                      const uint32_t y,
                      const uint32_t width,
                      const uint32_t height,
                      const ProbeCommand* command);

  VkPipelineDepthStencilStateCreateInfo GetPipelineDepthStencilInfo(
      const PipelineData* pipeline_data);
  VkPipelineColorBlendAttachmentState GetPipelineColorBlendAttachmentState(
      const PipelineData* pipeline_data);

  VkRenderPass render_pass_ = VK_NULL_HANDLE;
  RenderPassState render_pass_state_ = RenderPassState::kInactive;

  std::unique_ptr<FrameBuffer> frame_;
  VkFormat color_format_;
  VkFormat depth_stencil_format_;
  std::unique_ptr<IndexBuffer> index_buffer_;

  uint32_t frame_width_ = 0;
  uint32_t frame_height_ = 0;

  float clear_color_r_ = 0;
  float clear_color_g_ = 0;
  float clear_color_b_ = 0;
  float clear_color_a_ = 0;
  uint32_t clear_stencil_ = 0;
  float clear_depth_ = 1.0f;
  uint32_t patch_control_points_ = 3;
};

}  // namespace vulkan
}  // namespace amber

#endif  // SRC_VULKAN_GRAPHICS_PIPELINE_H_
