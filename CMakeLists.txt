
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
project(gvk)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
include(gvk.build)

################################################################################
# Default option
option(gvk-default-enabling "" ON)

################################################################################
# Dependency options
option(gvk-asio-enabled                  "" ${gvk-default-enabling})
option(gvk-cereal-enabled                "" ${gvk-default-enabling})
option(gvk-glfw-enabled                  "" ${gvk-default-enabling})
option(gvk-glm-enabled                   "" ${gvk-default-enabling})
option(gvk-glslang-enabled               "" ${gvk-default-enabling})
option(gvk-imgui-enabled                 "" ${gvk-default-enabling})
option(gvk-SPIRV-Cross-enabled           "" ${gvk-default-enabling})
option(gvk-SPIRV-Headers-enabled         "" ${gvk-default-enabling})
option(gvk-SPIRV-Tools-enabled           "" ${gvk-default-enabling})
option(gvk-stb-enabled                   "" ${gvk-default-enabling})
option(gvk-tinyxml2-enabled              "" ${gvk-default-enabling})
option(gvk-Vulkan-enabled                "" ${gvk-default-enabling})
option(gvk-VulkanMemoryAllocator-enabled "" ${gvk-default-enabling})

################################################################################
# GVK options
set(GVK_IDE_FOLDER "" CACHE STRING "")
option(gvk-command-structures-enabled    "" ${gvk-default-enabling})
option(gvk-cppgen-enabled                "" ${gvk-default-enabling})
option(gvk-format_info-enabled           "" ${gvk-default-enabling})
option(gvk-gui-enabled                   "" ${gvk-default-enabling})
option(gvk-handles-enabled               "" ${gvk-default-enabling})
option(gvk-layer-enabled                 "" ${gvk-default-enabling})
option(gvk-math-enabled                  "" ${gvk-default-enabling})
option(gvk-reference-enabled             "" ${gvk-default-enabling})
option(gvk-restore_info-enabled          "" ${gvk-default-enabling})
option(gvk-restore_point-enabled         "" ${gvk-default-enabling})
option(gvk-runtime-enabled               "" ${gvk-default-enabling})
option(gvk-spirv-enabled                 "" ${gvk-default-enabling})
option(gvk-state_tracker-enabled         "" ${gvk-default-enabling})
option(gvk-string-enabled                "" ${gvk-default-enabling})
option(gvk-structures-enabled            "" ${gvk-default-enabling})
option(gvk-system-enabled                "" ${gvk-default-enabling})
option(gvk-virtual-swapchain-enabled     "" ${gvk-default-enabling})
option(gvk-xml-enabled                   "" ${gvk-default-enabling})
option(gvk-build-samples                 "" ${gvk-default-enabling})
option(gvk-build-tests                   "" ${gvk-default-enabling})
cmake_dependent_option(gvk-run-tests "Run tests as part of build and treat test failures as build failures" ON "gvk-build-tests" OFF)

################################################################################
# GVK include
function(gvk_include prefix name)
    if(${gvk-${name}-enabled})
        include(${prefix}/${name})
    endif()
endfunction()

################################################################################
# Include external dependencies
gvk_include(external asio)
gvk_include(external cereal)
gvk_include(external glfw)
gvk_include(external glm)
gvk_include(external glslang)
gvk_include(external imgui)
gvk_include(external SPIRV-Cross)
gvk_include(external SPIRV-Headers)
gvk_include(external SPIRV-Tools)
gvk_include(external stb)
gvk_include(external tinyxml2)
gvk_include(external Vulkan)
gvk_include(external VulkanMemoryAllocator)
if(gvk-build-tests)
    include(external/googletest)
endif()

################################################################################
# Include GVK libraries
gvk_include(gvk command-structures)
gvk_include(gvk cppgen)
gvk_include(gvk format-info)
gvk_include(gvk gui)
gvk_include(gvk handles)
gvk_include(gvk layer)
gvk_include(gvk math)
gvk_include(gvk reference)
gvk_include(gvk restore-info)
gvk_include(gvk restore-point)
gvk_include(gvk runtime)
gvk_include(gvk spirv)
gvk_include(gvk state-tracker)
gvk_include(gvk string)
gvk_include(gvk structures)
gvk_include(gvk system)
gvk_include(gvk virtual-swapchain)
gvk_include(gvk xml)

################################################################################
# GVK INTERFACE
add_library(gvk INTERFACE)
macro(gvk_add_interface gvkLibrary)
    if(${${gvkLibrary}-enabled})
        target_link_libraries(gvk INTERFACE ${gvkLibrary})
    endif()
endmacro()

gvk_add_interface(gvk-runtime)
gvk_add_interface(gvk-handles)
gvk_add_interface(gvk-format-info)
gvk_add_interface(gvk-gui)
gvk_add_interface(gvk-math)
gvk_add_interface(gvk-reference)
gvk_add_interface(gvk-spirv)
gvk_add_interface(gvk-string)
gvk_add_interface(gvk-structures)
gvk_add_interface(gvk-system)

################################################################################
# Samples
if(gvk-build-samples)
    add_subdirectory(samples)
endif()

################################################################################
# Install
gvk_install_package()
