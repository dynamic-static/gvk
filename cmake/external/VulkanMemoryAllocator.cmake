
include_guard(GLOBAL)

################################################################################
# TODO : Documentation
set(gvk-VulkanMemoryAllocator-enabled           ON CACHE BOOL "" FORCE)
set(gvk-install-VulkanMemoryAllocator-artifacts ON CACHE BOOL "")
set(gvk-install-VulkanMemoryAllocator-headers   ON CACHE BOOL "")

################################################################################
# TODO : Documentation
set(VMA_STATIC_VULKAN_FUNCTIONS OFF CACHE BOOL "" FORCE)
set(VulkanMemoryAllocator_VERSION 009ecd192c1289c7529bff248a16cfe896254816) # 3.1.0
FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
    GIT_TAG ${VulkanMemoryAllocator_VERSION}
    GIT_PROGRESS TRUE
)

################################################################################
# TODO : Documentation
FetchContent_Populate(VulkanMemoryAllocator)
FetchContent_GetProperties(VulkanMemoryAllocator SOURCE_DIR VulkanMemoryAllocator_SOURCE_DIR)
add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE "$<BUILD_INTERFACE:${VulkanMemoryAllocator_SOURCE_DIR}/include/>" $<INSTALL_INTERFACE:include>)

# ################################################################################
# # TODO : Documentation
# gvk_install_library(TARGET VulkanMemoryAllocator VERSION ${VulkanMemoryAllocator_VERSION})

################################################################################
# TODO : Documentation
if(gvk-install-VulkanMemoryAllocator-artifacts)
    gvk_install_artifacts(TARGET VulkanMemoryAllocator VERSION ${VulkanMemoryAllocator_VERSION})
endif()
if(gvk-install-VulkanMemoryAllocator-headers)
    gvk_install_headers(TARGET VulkanMemoryAllocator VERSION ${VulkanMemoryAllocator_VERSION})
endif()
