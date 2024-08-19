
cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
project(gvk)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
include(gvk.build)

################################################################################
# Dependency options
option(gvk-asio-enabled                  "" ON)
option(gvk-cereal-enabled                "" ON)
option(gvk-glfw-enabled                  "" ON)
option(gvk-glm-enabled                   "" ON)
option(gvk-glslang-enabled               "" ON)
option(gvk-imgui-enabled                 "" ON)
option(gvk-SPIRV-Cross-enabled           "" ON)
option(gvk-SPIRV-Headers-enabled         "" ON)
option(gvk-SPIRV-Tools-enabled           "" ON)
option(gvk-stb-enabled                   "" ON)
option(gvk-tinyxml2-enabled              "" ON)
option(gvk-Vulkan-enabled                "" ON)
option(gvk-VulkanMemoryAllocator-enabled "" ON)

################################################################################
# GVK options
set(GVK_IDE_FOLDER "" CACHE STRING "")
option(gvk-command-structures-enabled    "" ON)
option(gvk-cppgen-enabled                "" ON)
option(gvk-format_info-enabled           "" ON)
option(gvk-gui-enabled                   "" ON)
option(gvk-handles-enabled               "" ON)
option(gvk-layer-enabled                 "" ON)
option(gvk-math-enabled                  "" ON)
option(gvk-reference-enabled             "" ON)
option(gvk-restore_info-enabled          "" ON)
option(gvk-restore_point-enabled         "" ON)
option(gvk-runtime-enabled               "" ON)
option(gvk-spirv-enabled                 "" ON)
option(gvk-state_tracker-enabled         "" ON)
option(gvk-string-enabled                "" ON)
option(gvk-structures-enabled            "" ON)
option(gvk-system-enabled                "" ON)
option(gvk-virtual-swapchain-enabled     "" ON)
option(gvk-xml-enabled                   "" ON)
option(gvk-build-samples                 "" ON)
option(gvk-build-tests                   "" ON)
cmake_dependent_option(GVK_RUN_TESTS "Run tests as part of build and treat test failures as build failures" ON "GVK_BUILD_TESTS" OFF)

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
gvk_include(external googletest)

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
# TODO : Documentation
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
# if(GVK_BUILD_SAMPLES)
#     add_subdirectory(samples)
# endif()

################################################################################
# Install
gvk_install_package()






# ##############################################################################
# # Options
# include(CMakeDependentOption)
# set                   (GVK_IDE_FOLDER               "" CACHE STRING "")
# option                (GVK_ASIO-enabled             "" ON)
# option                (GVK_CEREAL-enabled           "" ON)
# option                (GVK_GLFW-enabled             "" ON)
# option                (GVK_GLM-enabled              "" ON)
# option                (GVK_GLSLANG-enabled          "" ON)
# option                (GVK_IMGUI-enabled            "" ON)
# option                (GVK_SPIRV_CROSS-enabled      "" ON)
# option                (GVK_SPIRV_HEADERS-enabled    "" ON)
# option                (GVK_SPIRV_TOOLS-enabled      "" ON)
# option                (GVK_STB-enabled              "" ON)
# option                (GVK_TINY_XML-enabled         "" ON)
# option                (GVK_VMA-enabled              "" ON)
# 
# 
# 
# 
# option                (GVK_BUILD_REFERENCE          "" ON)
# option                (GVK_BUILD_STRING             "" ON)
# cmake_dependent_option(GVK_BUILD_MATH               "" ON "GVK_GLM-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_XML                "" ON "GVK_BUILD_STRING;GVK_TINY_XML-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_CPPGEN             "" ON "GVK_BUILD_XML" OFF)
# cmake_dependent_option(GVK_BUILD_RUNTIME            "" ON "GVK_BUILD_CPPGEN" OFF)
# cmake_dependent_option(GVK_BUILD_STRUCTURES         "" ON "GVK_BUILD_RUNTIME;GVK_CEREAL-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_COMMAND_STRUCTURES "" ON "GVK_BUILD_STRUCTURES" OFF)
# cmake_dependent_option(GVK_BUILD_FORMAT_INFO        "" ON "GVK_BUILD_STRUCTURES" OFF)
# cmake_dependent_option(GVK_BUILD_HANDLES            "" ON "GVK_BUILD_FORMAT_INFO;GVK_BUILD_REFERENCE;GVK_VMA-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_LAYER              "" ON "GVK_BUILD_RUNTIME;GVK_BUILD_STRUCTURES" OFF)
# cmake_dependent_option(GVK_BUILD_SPIRV              "" ON "GVK_BUILD_HANDLES;GVK_GLSLANG-enabled;GVK_SPIRV_CROSS-enabled;GVK_SPIRV_HEADERS-enabled;GVK_SPIRV_TOOLS-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_STATE_TRACKER      "" ON "GVK_BUILD_COMMAND_STRUCTURES;GVK_BUILD_LAYER;GVK_BUILD_REFERENCE" OFF)
# cmake_dependent_option(GVK_BUILD_RESTORE_POINT      "" ON "GVK_BUILD_STATE_TRACKER" OFF)
# cmake_dependent_option(GVK_BUILD_VIRTUAL_SWAPCHAIN  "" ON "GVK_BUILD_HANDLES;GVK_BUILD_LAYER;GVK_BUILD_STRUCTURES" OFF)
# cmake_dependent_option(GVK_BUILD_SYSTEM             "" ON "GVK_BUILD_REFERENCE;GVK_GLFW-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_GUI                "" ON "GVK_BUILD_HANDLES;GVK_BUILD_SYSTEM;GVK_IMGUI-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_SAMPLES            "" ON "GVK_BUILD_GUI;GVK_BUILD_HANDLES;GVK_BUILD_MATH;GVK_BUILD_SPIRV;GVK_BUILD_SYSTEM;GVK_STB-enabled" OFF)
# cmake_dependent_option(GVK_BUILD_TOOLS              "" ON "GVK_BUILD_STRING" OFF)
# option                (GVK_BUILD_TESTS              "" ON)
# cmake_dependent_option(GVK_RUN_TESTS                "" ON "GVK_BUILD_TESTS" OFF)
# 
# ################################################################################
# # Dependencies
# find_package(Threads REQUIRED)
# include(Vulkan)
# 
# macro(gvk_include_depenedency enabled dependency)
#     if(${enabled})
#         include(${dependency})
#     endif()
# endmacro()
# 
# # NOTE : Include SPIRV-Headers and SPIRV-Tools first so targets are available
# #   when glslang and SPIRV-Cross are included
# gvk_include_depenedency(GVK_SPIRV_HEADERS-enabled SPIRV-Headers)
# gvk_include_depenedency(GVK_SPIRV_TOOLS-enabled   SPIRV-Tools)
# gvk_include_depenedency(GVK_BUILD_TESTS           googletest)
# gvk_include_depenedency(GVK_ASIO-enabled          asio)
# gvk_include_depenedency(GVK_CEREAL-enabled        cereal)
# gvk_include_depenedency(GVK_GLFW-enabled          glfw)
# gvk_include_depenedency(GVK_GLM-enabled           glm)
# gvk_include_depenedency(GVK_GLSLANG-enabled       glslang)
# gvk_include_depenedency(GVK_SPIRV_CROSS-enabled   SPIRV-Cross)
# gvk_include_depenedency(GVK_IMGUI-enabled         imgui)
# gvk_include_depenedency(GVK_STB-enabled           stb)
# gvk_include_depenedency(GVK_TINY_XML-enabled      tinyxml2)
# gvk_include_depenedency(GVK_VMA-enabled           VulkanMemoryAllocator)
# 
# gvk_include_depenedency(True VUlkan-Headers)
# 
# ################################################################################
# # Modules
# macro(gvk_add_subdirectory enabled subdirectory)
#     if(${enabled})
#         add_subdirectory(${subdirectory})
#     endif()
# endmacro()
# 
# gvk_add_subdirectory(GVK_BUILD_COMMAND_STRUCTURES gvk-command-structures)
# gvk_add_subdirectory(GVK_BUILD_CPPGEN             gvk-cppgen)
# gvk_add_subdirectory(GVK_BUILD_LAYER              gvk-layer)
# gvk_add_subdirectory(GVK_BUILD_RESTORE_POINT      gvk-restore-info)
# gvk_add_subdirectory(GVK_BUILD_RESTORE_POINT      gvk-restore-point)
# gvk_add_subdirectory(GVK_BUILD_STATE_TRACKER      gvk-state-tracker)
# gvk_add_subdirectory(GVK_BUILD_VIRTUAL_SWAPCHAIN  gvk-virtual-swapchain)
# gvk_add_subdirectory(GVK_BUILD_XML                gvk-xml)
# 
# add_library(gvk INTERFACE)
# macro(gvk_add_interface enabled module)
#     if(${enabled})
#         add_subdirectory(${module})
#         target_link_libraries(gvk INTERFACE ${module})
#     endif()
# endmacro()
# 
# gvk_add_interface(GVK_BUILD_RUNTIME     gvk-runtime)
# gvk_add_interface(GVK_BUILD_HANDLES     gvk-handles)
# gvk_add_interface(GVK_BUILD_FORMAT_INFO gvk-format-info)
# gvk_add_interface(GVK_BUILD_GUI         gvk-gui)
# gvk_add_interface(GVK_BUILD_MATH        gvk-math)
# gvk_add_interface(GVK_BUILD_REFERENCE   gvk-reference)
# gvk_add_interface(GVK_BUILD_SPIRV       gvk-spirv)
# gvk_add_interface(GVK_BUILD_STRING      gvk-string)
# gvk_add_interface(GVK_BUILD_STRUCTURES  gvk-structures)
# gvk_add_interface(GVK_BUILD_SYSTEM      gvk-system)
# 
# ################################################################################
# # Samples
# if(GVK_BUILD_SAMPLES)
#     add_subdirectory(samples)
# endif()
# 
# ################################################################################
# # Install
# gvk_install_package()
