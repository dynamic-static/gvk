
include_guard(GLOBAL)

################################################################################
# TODO : Documentation
gvk_enable_target(glfw)

################################################################################
# TODO : Documentation
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(glfw_VERSION 7b6aead9fb88b3623e3b3725ebb42670cbe4c579) # 3.4
FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG ${glfw_VERSION}
    GIT_PROGRESS TRUE
)

################################################################################
# TODO : Documentation
FetchContent_MakeAvailable(glfw)
set(folder "${GVK_IDE_FOLDER}/external/glfw/")
set_target_properties(glfw PROPERTIES FOLDER "${folder}")
set_target_properties(update_mappings PROPERTIES FOLDER "${folder}")

################################################################################
# TODO : Documentation
if(gvk-glfw-install-artifacts)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        # TODO : Need to revisit cmake exports on Linux
        gvk_install_artifacts(TARGET glfw VERSION ${glfw_VERSION})
    endif()
endif()
