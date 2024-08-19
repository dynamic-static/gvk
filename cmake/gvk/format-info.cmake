
include_guard(GLOBAL)

set(GVK_FORMAT_INFO_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/cppgen)
include(gvk/runtime)
include(gvk/structures)

add_subdirectory(gvk-format-info)
