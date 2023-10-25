
include_guard()

include(FetchContent)

set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(glfw_VERSION 7482de6071d21db77a7236155da44c172a7f6c9e) # 3.3.8
FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG ${glfw_VERSION}
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glfw)
set(folder "${GVK_IDE_FOLDER}/external/glfw/")
set_target_properties(glfw PROPERTIES FOLDER "${folder}")
set_target_properties(update_mappings PROPERTIES FOLDER "${folder}")
