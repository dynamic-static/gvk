
VERSION 2024.3.2
--
-   CHANGED : gvk handle types now use same reference counting mechanism as handles generated from vk.xml
-   CHANGED : gvk dispatchable handle types now have generated member functions for API entry points

VERSION 2024.3.1
--
-   CHANGED : VK_LAYER_INTEL_gvk_virtual_swapchain stability and performance improvements
-   NEW : gvk_result_scope callbacks

VERSION 2024.3.0
--
-   CHANGED : gvk-log-parser improvmements for readability and usability
-   CHANGED : Improved install, targets now enable their dependencies recursively
-   NEW : VK_LAYER_INTEL_gvk_state_tracker preliminary support for VKRT workloads
-   NEW : VK_LAYER_INTEL_gvk_restore_point preliminary support for VKRT workloads

VERSION 2024.1.0
--
-   NEW : VK_LAYER_INTEL_gvk_restore_point
-   NEW : VK_LAYER_INTEL_gvk_virtual_swapchain

VERSION 2023.3.0
--
-   CHANGED : Handle accessors now return `const T&`

VERSION 2023.2.3.1
--
-   NEW : Preliminary support for VkShaderEXT

VERSION 2023.2.3.0
--
-   NEW : gvk-xml updated to be able to select "vulkan" or "vulkansc", default is "vulkan" (VulkanSC is currently unsupported)
-   REMOVED : gvk no longer configures install via CMake, consuming projects must handle installation if required

VERSION 2023.1.0
--
-   NEW : Basic Ubuntu 20.04 support
-   NEW : Basic VK_KHR_xlib_surface support
-   NEW : gvk-layer provides utilities for creating a custom Vulkan layer
-   NEW : VK_LAYER_INTEL_gvk_state_tracker
-   NEW : gvk-gui with ImGui integration
-   NEW : Sample demonstrating ImGui integration
-   NEW : Sample demonstrating custom layer usage
-   NEW : SPIRV-Tools integration
-   NEW : gvk-command-structures provides serialization/stringification and comparison operators for Vulkan commands
-   NEW : GvkFormatInfo provides detailed information regarding VkFormat

VERSION 2022.3.0
--
-   Initial release
