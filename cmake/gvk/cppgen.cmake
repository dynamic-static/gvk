
include_guard(GLOBAL)

set(GVK_CPPGEN_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/string)
include(gvk/xml)

add_subdirectory(gvk-cppgen)
