
include_guard(GLOBAL)

set(GVK_RUNTIME_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/cppgen)
include(gvk/string)
include(external/Vulkan)

add_subdirectory(gvk-runtime)
