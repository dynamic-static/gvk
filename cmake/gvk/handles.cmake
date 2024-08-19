
include_guard(GLOBAL)

set(GVK_HANDLES_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/cppgen)
include(gvk/format-info)
include(gvk/reference)
include(gvk/runtime)
include(gvk/string)
include(gvk/structures)
include(external/VulkanMemoryAllocator)

add_subdirectory(gvk-handles)
