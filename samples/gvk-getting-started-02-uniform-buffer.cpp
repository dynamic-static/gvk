
/*******************************************************************************

MIT License

Copyright (c) Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "gvk-sample-utilities.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <vector>

VkResult create_mesh(const gvk::Context& context, gvk::Mesh* pMesh)
{
    assert(pMesh);
    gvk_result_scope_begin(VK_ERROR_INITIALIZATION_FAILED) {
        std::array<VertexPositionColor, 4> vertices {
            VertexPositionColor {{ -0.5f, 0.0f, -0.5f }, { gvk::math::Color::OrangeRed }},
            VertexPositionColor {{  0.5f, 0.0f, -0.5f }, { gvk::math::Color::BlueViolet }},
            VertexPositionColor {{  0.5f, 0.0f,  0.5f }, { gvk::math::Color::DodgerBlue }},
            VertexPositionColor {{ -0.5f, 0.0f,  0.5f }, { gvk::math::Color::Goldenrod }},
        };
        std::array<uint16_t, 6> indices {
            0, 1, 2,
            2, 3, 0,
        };
        gvk_result(pMesh->write(
            context.get<gvk::Devices>()[0],
            gvk::get_queue_family(context.get<gvk::Devices>()[0], 0).queues[0],
            context.get<gvk::CommandBuffers>()[0],
            VK_NULL_HANDLE,
            (uint32_t)vertices.size(),
            vertices.data(),
            (uint32_t)indices.size(),
            indices.data()
        ));
    } gvk_result_scope_end;
    return gvkResult;
}

int main(int, const char*[])
{
    gvk_result_scope_begin(VK_ERROR_INITIALIZATION_FAILED) {

        GvkSampleContext context = gvk::nullref;
        gvk_result(GvkSampleContext::create("Intel(R) GPA Utilities for Vulkan* - Getting Started - 02 - Uniform Buffer", &context));

        gvk::system::Surface systemSurface = gvk::nullref;
        gvk_result(gvk_sample_create_sys_surface(context, &systemSurface));

        gvk::wsi::Context wsiContext = gvk::nullref;
        gvk_result(gvk_sample_create_wsi_context(context, systemSurface, &wsiContext));

        auto vertexShaderInfo = gvk::get_default<gvk::spirv::ShaderInfo>();
        vertexShaderInfo.language = gvk::spirv::ShadingLanguage::Glsl;
        vertexShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderInfo.lineOffset = __LINE__;
        vertexShaderInfo.source = R"(
            #version 450

            /**
            This provides access to the data referenced by the VkDescriptorSet bound by the
                call to vkCmdBindDescriptorSets() during VkCommandBuffer recording.
            */
            layout(set = 0, binding = 0)
            uniform UniformBuffer
            {
                mat4 world;
                mat4 view;
                mat4 projection;
            } ubo;

            layout(location = 0) in vec3 vsPosition;
            layout(location = 1) in vec4 vsColor;
            layout(location = 0) out vec4 fsColor;

            out gl_PerVertex
            {
                vec4 gl_Position;
            };

            void main()
            {
                gl_Position = ubo.projection * ubo.view * ubo.world * vec4(vsPosition, 1);
                fsColor = vsColor;
            }
        )";
        auto fragmentShaderInfo = gvk::get_default<gvk::spirv::ShaderInfo>();
        fragmentShaderInfo.language = gvk::spirv::ShadingLanguage::Glsl;
        fragmentShaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderInfo.lineOffset = __LINE__;
        fragmentShaderInfo.source = R"(
            #version 450

            layout(location = 0) in vec4 fsColor;
            layout(location = 0) out vec4 fragColor;

            void main()
            {
                fragColor = fsColor;
            }
        )";
        gvk::Pipeline pipeline = VK_NULL_HANDLE;
        gvk_result(gvk_sample_create_pipeline<VertexPositionColor>(
            wsiContext.get<gvk::RenderPass>(),
            VK_CULL_MODE_BACK_BIT,
            vertexShaderInfo,
            fragmentShaderInfo,
            &pipeline
        ));

        gvk::Mesh mesh;
        gvk_result(create_mesh(context, &mesh));

        // We create a gvk::Buffer that will be used to hold uniform data.  The
        //  gvk::Buffer will be persistently mapped so we can write to it without
        //  mapping/unmapping each time we want to write...
        gvk::Buffer uniformBuffer = VK_NULL_HANDLE;
        gvk_result(gvk_sample_create_uniform_buffer<Uniforms>(context, &uniformBuffer));
        VmaAllocationInfo uniformBufferAllocationInfo{ };
        vmaGetAllocationInfo(context.get<gvk::Devices>()[0].get<VmaAllocator>(), uniformBuffer.get<VmaAllocation>(), &uniformBufferAllocationInfo);

        // Allocate a gvk::DescriptorSet...
        std::vector<gvk::DescriptorSet> descriptorSets;
        gvk_result(gvk_sample_allocate_descriptor_sets(pipeline, descriptorSets));
        assert(descriptorSets.size() == 1);
        auto descriptorSet = descriptorSets[0];

        // Then write the gvk::Buffer to the gvk::DescriptorSet...
        auto descriptorBufferInfo = gvk::get_default<VkDescriptorBufferInfo>();
        descriptorBufferInfo.buffer = uniformBuffer;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;
        auto writeDescriptorSet = gvk::get_default<VkWriteDescriptorSet>();
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        vkUpdateDescriptorSets(context.get<gvk::Devices>()[0], 1, &writeDescriptorSet, 0, nullptr);

        // Create a gvk::system::Clock, gvk::math::Camera, and gvk::math::Transform.
        //  These objects will be used to animate our gvk::Mesh...
        gvk::system::Clock clock;
        gvk::math::Camera camera;
        camera.transform.translation = { 0, 1.25f, 1.25f };
        gvk::math::Transform quadTransform;

        while (
            !(systemSurface.get<gvk::system::Input>().keyboard.down(gvk::system::Key::Escape)) &&
            !(systemSurface.get<gvk::system::Surface::StatusFlags>() & gvk::system::Surface::CloseRequested)) {
            gvk::system::Surface::update();

            // Update the gvk::system::Clock and gvk::math::Transform...
            clock.update();
            auto rotation = 90.0f * clock.elapsed<gvk::system::Seconds<float>>();
            quadTransform.rotation *= glm::angleAxis(glm::radians(rotation), glm::vec3 { 0, 1, 0 });

            // Populate a Uniforms object with gvk::math::Camera and gvk::math::Transform
            //  data then memcpy() it into our mapped gvk::Buffer...
            Uniforms uniforms{ };
            uniforms.object.world = quadTransform.world_from_local();
            uniforms.camera.view = camera.view(glm::vec3{ });
            uniforms.camera.projection = camera.projection();
            memcpy(uniformBufferAllocationInfo.pMappedData, &uniforms, sizeof(Uniforms));

            gvk::wsi::AcquiredImageInfo acquiredImageInfo{ };
            gvk::RenderTarget acquiredImageRenderTarget = VK_NULL_HANDLE;
            auto wsiStatus = wsiContext.acquire_next_image(UINT64_MAX, VK_NULL_HANDLE, &acquiredImageInfo, &acquiredImageRenderTarget);
            if (wsiStatus == VK_SUCCESS || wsiStatus == VK_SUBOPTIMAL_KHR) {
                const auto& device = context.get<gvk::Devices>()[0];
                auto extent = wsiContext.get<gvk::SwapchainKHR>().get<VkSwapchainCreateInfoKHR>().imageExtent;
                camera.set_aspect_ratio(extent.width, extent.height);

                gvk_result(vkBeginCommandBuffer(acquiredImageInfo.commandBuffer, &gvk::get_default<VkCommandBufferBeginInfo>()));
                const auto& renderPassBeginInfo = acquiredImageRenderTarget.get<VkRenderPassBeginInfo>();
                vkCmdBeginRenderPass(acquiredImageInfo.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                VkRect2D scissor { { }, renderPassBeginInfo.renderArea.extent };
                vkCmdSetScissor(acquiredImageInfo.commandBuffer, 0, 1, &scissor);
                VkViewport viewport { 0, 0, (float)scissor.extent.width, (float)scissor.extent.height, 0, 1 };
                vkCmdSetViewport(acquiredImageInfo.commandBuffer, 0, 1, &viewport);

                vkCmdBindPipeline(acquiredImageInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
                // Bind our gvk::DescriptorSet for use with the bound gvk::Pipeline...
                vkCmdBindDescriptorSets(acquiredImageInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get<gvk::PipelineLayout>(), 0, 1, &descriptorSet.get<VkDescriptorSet>(), 0, nullptr);

                mesh.record_cmds(acquiredImageInfo.commandBuffer);
                vkCmdEndRenderPass(acquiredImageInfo.commandBuffer);
                gvk_result(vkEndCommandBuffer(acquiredImageInfo.commandBuffer));

                const auto& queue = gvk::get_queue_family(device, 0).queues[0];
                gvk_result(vkQueueSubmit(queue, 1, &wsiContext.get<VkSubmitInfo>(acquiredImageInfo), acquiredImageInfo.fence));

                wsiStatus = wsiContext.queue_present(queue, &acquiredImageInfo);
                gvk_result((wsiStatus == VK_SUBOPTIMAL_KHR || wsiStatus == VK_ERROR_OUT_OF_DATE_KHR) ? VK_SUCCESS : wsiStatus);
            }
        }
        gvk_result(vkDeviceWaitIdle(context.get<gvk::Devices>()[0]));
    } gvk_result_scope_end;
    if (gvkResult) {
        std::cerr << gvk::to_string(gvkResult) << std::endl;
    }
    return (int)gvkResult;
}
