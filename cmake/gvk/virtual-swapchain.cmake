
include_guard(GLOBAL)

set(GVK_VIRTUAL_SWAPCHAIN_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/handles)

add_subdirectory(gvk-virtual-swapchain)
