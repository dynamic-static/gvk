
include_guard(GLOBAL)

set(GVK_SYSTEM_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/reference)
include(external/glfw)

add_subdirectory(gvk-system)
