
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

#pragma once

#include "gvk-cppgen.hpp"
#include "gvk-string.hpp"

#include <cassert>

namespace gvk {
namespace cppgen {

class StateTrackedHandleGenerator final
    : public BasicHandleGenerator
{
public:
    StateTrackedHandleGenerator(const xml::Manifest& manifest, const xml::Handle& handle)
        : BasicHandleGenerator(manifest, handle)
    {
        generate_ctors(false, false);

        if (handle.name == "VkPhysicalDevice") {
            add_member(MemberInfo("VkInstance", "mVkInstance", "Instance"));
        }
        if (handle.name == "VkDevice") {
            add_member(MemberInfo("ObjectTracker<Queue>", "mQueueTracker"));
        }
        if (handle.name == "VkQueue") {
            add_member(MemberInfo("VkDevice", "mVkDevice", "Device", "mReference.get_obj().mVkDevice"));
            add_member(MemberInfo("gvk::Auto<VkDeviceQueueCreateInfo>", "mDeviceQueueCreateInfo", "VkDeviceQueueCreateInfo"));
        }
        if (handle.name == "VkPipeline") {
            add_member(MemberInfo("std::vector<ShaderModule>", "mShaderModules"));
        }
        if (handle.name == "VkBuffer") {
            add_member(MemberInfo("std::set<VkDeviceMemory>", "mVkDeviceMemoryBindings"));
            add_member(MemberInfo("gvk::Auto<VkBindBufferMemoryInfo>", "mBindBufferMemoryInfo", "VkBindBufferMemoryInfo"));
        }
        if (handle.name == "VkImage") {
            add_member(MemberInfo("VkSwapchainKHR", "mVkSwapchainKHR", "SwapchainKHR", "SwapchainKHR({ mReference.get_obj().mDevice, mReference.get_obj().mVkSwapchainKHR })"));
            add_member(MemberInfo("std::set<VkDeviceMemory>", "mVkDeviceMemoryBindings"));
            add_member(MemberInfo("gvk::Auto<VkBindImageMemoryInfo>", "mBindImageMemoryInfo", "VkBindImageMemoryInfo"));
            add_member(MemberInfo("ImageLayoutTracker", "mImageLayoutTracker", "const ImageLayoutTracker&"));
        }
        if (handle.name == "VkDeviceMemory") {
            add_member(MemberInfo("std::set<VkBuffer>", "mVkBufferBindings"));
            add_member(MemberInfo("std::set<VkImage>", "mVkImageBindings"));
        }
        if (handle.name == "VkDescriptorSet") {
            add_member(MemberInfo("std::map<uint32_t, Descriptor>", "mDescriptors"));
        }
        if (handle.name == "VkDescriptorSetLayout") {
            add_member(MemberInfo("std::map<uint32_t, std::vector<Sampler>>", "mImmutableSamplers"));
        }
        if (handle.name == "VkCommandBuffer") {
            add_member(MemberInfo("gvk::Auto<VkCommandBufferBeginInfo>", "mCommandbufferBeginInfo"));
            add_member(MemberInfo("CmdTracker", "mCmdTracker"));
        }
        if (handle.name == "VkSwapchainKHR") {
            add_member(MemberInfo("ObjectTracker<Image>", "mImages"));
        }

        for (const auto& child : handle.children) {
            const auto& childHandleItr = manifest.handles.find(child);
            assert(childHandleItr != manifest.handles.end());
            auto type = string::replace("ObjectTracker<{handleType}>", "{handleType}", string::strip_vk(child));
            auto name = "m" + string::strip_vk(child) + "Tracker";
            add_member({ type, name, std::string(), std::string(), childHandleItr->second.compileGuards });
        }

        std::vector<string::Replacement> replacements {
            { "{vkHandleType}", handle.name },
            { "{dispatchableVkHandleType}", handle.get_dispatchable_handle(manifest) },
        };

        xml::Parameter pfnCallbackParameter;
        pfnCallbackParameter.type = "PFN_gvkEnumerateStateTrackedObjectsCallback";
        pfnCallbackParameter.name = "pfnCallback";
        xml::Parameter pUserDataParameter;
        pUserDataParameter.type = "void*";
        pUserDataParameter.name = "pUserData";

        // enumerate() method
        {
            MethodInfo methodInfo;
            methodInfo.method.name = "enumerate";
            std::stringstream strStrm;
            strStrm << string::replace(
R"({
    assert(pfnCallback);
    if (mReference) {
        GvkStateTrackedObject stateTrackedObject { };
        stateTrackedObject.type = get<VkObjectType>();
        stateTrackedObject.handle = (uint64_t)get<{vkHandleType}>();
        stateTrackedObject.dispatchableHandle = (uint64_t)get<{dispatchableVkHandleType}>();
        pfnCallback(&stateTrackedObject, nullptr, pUserData);
)", replacements);
            bool controlBlockDeclared = false;
            for (const auto& memberInfo : get_members()) {
                if (string::contains(memberInfo.storageType, "ObjectTracker")) {
                    if (!controlBlockDeclared) {
                        strStrm << "        const auto& controlBlock = mReference.get_obj();" << std::endl;
                        controlBlockDeclared = true;
                    }
                    CompileGuardGenerator compileGuardGenerator(strStrm, memberInfo.compileGuards);
                    strStrm << "        controlBlock." << memberInfo.storageName << ".enumerate(pfnCallback, pUserData);" << std::endl;
                }
            }
            strStrm << string::replace(
R"(    }
}
)", replacements);
            methodInfo.body = strStrm.str();
            methodInfo.method.parameters.push_back(pfnCallbackParameter);
            methodInfo.method.parameters.push_back(pUserDataParameter);
            add_method(methodInfo);
        }

        // enumerate_dependencies() method
        {
            MethodInfo methodInfo;
            methodInfo.method.name = "enumerate_dependencies";
            std::stringstream strStrm;
            strStrm << string::replace(
R"({
    assert(pfnCallback);
    if (mReference) {
        GvkStateTrackedObject stateTrackedObject { };
        stateTrackedObject.type = get<VkObjectType>();
        stateTrackedObject.handle = (uint64_t)get<{vkHandleType}>();
        stateTrackedObject.dispatchableHandle = (uint64_t)get<{dispatchableVkHandleType}>();
        pfnCallback(&stateTrackedObject, nullptr, pUserData);
)", replacements);
            for (const auto& memberInfo : get_members()) {
                if (memberInfo.storageType != get_handle().name) {
                    if (manifest.handles.count("Vk" + string::remove(string::remove(memberInfo.storageType, "std::vector<"), ">"))) {
                        CompileGuardGenerator compileGuardGenerator(strStrm, memberInfo.compileGuards);
                        if (string::contains(memberInfo.storageType, "std::vector")) {
                            strStrm << "        for (const auto& dependency : mReference.get_obj()." << memberInfo.storageName << ") {" << std::endl;
                            strStrm << "            dependency.enumerate_dependencies(pfnCallback, pUserData);" << std::endl;
                            strStrm << "        }" << std::endl;
                        } else {
                            strStrm << "        mReference.get_obj()." << memberInfo.storageName << ".enumerate_dependencies(pfnCallback, pUserData);" << std::endl;
                        }
                    } else if (manifest.handles.count(memberInfo.storageType)) {
                        strStrm << "        get<" << string::strip_vk(memberInfo.accessorType) << ">().enumerate_dependencies(pfnCallback, pUserData);" << std::endl;
                    }
                }
            }
            strStrm << string::replace(
R"(    }
}
)", replacements);
            methodInfo.body = strStrm.str();
            methodInfo.method.parameters.push_back(pfnCallbackParameter);
            methodInfo.method.parameters.push_back(pUserDataParameter);
            if (handle.name == "VkDisplayModeKHR" ||
                handle.name == "VkSwapchainKHR"
            ) {
                methodInfo.manuallyImplemented = true;
            }
            add_method(methodInfo);
        }
        add_member({ "GvkStateTrackedObjectInfo", "mStateTrackedObjectInfo", "GvkStateTrackedObjectInfo" });
        add_private_declaration("friend class BasicStateTracker");
        add_private_declaration("friend class StateTracker");
    }
};

class StateTrackedHandlesGenerator final
{
public:
    static void generate(const xml::Manifest& manifest)
    {
        ModuleGenerator module(
            GVK_STATE_TRACKER_GENERATED_INCLUDE_PATH,
            GVK_STATE_TRACKER_GENERATED_INCLUDE_PREFIX,
            GVK_STATE_TRACKER_GENERATED_SOURCE_PATH,
            "state-tracked-handles"
        );
        std::vector<StateTrackedHandleGenerator> generators;
        for (const auto& handleItr : manifest.handles) {
            generators.emplace_back(manifest, handleItr.second);
        }
        generate_forward_declarations(generators);
        generate_header(module.header, manifest, generators);
        generate_source(module.source, manifest, generators);
    }

private:
    static void generate_forward_declarations(const std::vector<StateTrackedHandleGenerator>& generators)
    {
        FileGenerator file(GVK_STATE_TRACKER_GENERATED_INCLUDE_PATH "/forward-declarations.inl");
        file << std::endl;
        file << "#include \"gvk-defines.hpp\"" << std::endl;
        file << std::endl;
        NamespaceGenerator namespaceGenerator(file, "gvk::state_tracker");
        file << std::endl;
        for (const auto& generator : generators) {
            CompileGuardGenerator compileGuardGenerator(file, generator.get_handle().compileGuards);
            file << "class " << generator.get_handle_name() << ";" << std::endl;
        }
        file << std::endl;
    }

    static void generate_header(
        FileGenerator& file,
        const xml::Manifest& manifest,
        const std::vector<StateTrackedHandleGenerator>& generators
    )
    {
        file << "#include \"gvk-state-tracker/generated/forward-declarations.inl\"" << std::endl;
        file << "#include \"gvk-state-tracker/descriptor.hpp\"" << std::endl;
        file << "#include \"gvk-state-tracker/cmd-tracker.hpp\"" << std::endl;
        file << "#include \"gvk-state-tracker/object-tracker.hpp\"" << std::endl;
        file << "#include \"gvk-state-tracker/image-layout-tracker.hpp\"" << std::endl;
        file << "#include \"gvk-reference.hpp\"" << std::endl;
        file << "#include \"gvk-structures.hpp\"" << std::endl;
        file << "#include \"VK_LAYER_INTEL_gvk_state_tracker.h\"" << std::endl;
        file << std::endl;
        file << "#include <cassert>" << std::endl;
        file << "#include <map>" << std::endl;
        file << "#include <set>" << std::endl;
        file << "#include <type_traits>" << std::endl;
        file << "#include <vector>" << std::endl;
        file << std::endl;
        NamespaceGenerator namespaceGenerator(file, "gvk::state_tracker");
        for (const auto& generator : generators) {
            generator.generate_handle_declaration(file, manifest);
        }
        for (const auto& generator : generators) {
            generator.generate_control_block_declaration(file, manifest);
        }
        for (const auto& generator : generators) {
            generator.generate_accessors(file, manifest);
        }
        file << std::endl;
    }

    static void generate_source(
        FileGenerator& file,
        const xml::Manifest& manifest,
        const std::vector<StateTrackedHandleGenerator>& generators
    )
    {
        NamespaceGenerator namespaceGenerator(file, "gvk::state_tracker");
        for (const auto& generator : generators) {
            generator.generate_handle_definition(file, manifest);
        }
        for (const auto& generator : generators) {
            generator.generate_control_block_definition(file, manifest);
        }
        file << std::endl;
    }
};

} // namespace cppgen
} // namespace gvk
