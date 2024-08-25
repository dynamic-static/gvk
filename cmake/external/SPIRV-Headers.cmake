
include_guard(GLOBAL)

################################################################################
# TODO : Documentation
gvk_enable_target(SPIRV-Headers)

################################################################################
# TODO : Documentation
set(SPIRV_HEADERS_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SPIRV_HEADERS_SKIP_INSTALL  ON CACHE BOOL "" FORCE)
set(SPIRV-Headers_VERSION 4f7b471f1a66b6d06462cd4ba57628cc0cd087d7) # vulkan-sdk-1.3.283.0
FetchContent_Declare(
    SPIRV-Headers
    GIT_REPOSITORY "https://github.com/KhronosGroup/SPIRV-Headers.git"
    GIT_TAG ${SPIRV-Headers_VERSION}
    GIT_PROGRESS TRUE
)
################################################################################
# TODO : Documentation
FetchContent_MakeAvailable(SPIRV-Headers)

################################################################################
# TODO : Documentation
if(gvk-SPIRV-Headers-install-headers)
    gvk_install_artifacts(TARGET SPIRV-Headers VERSION ${SPIRV-Headers_VERSION})
endif()
