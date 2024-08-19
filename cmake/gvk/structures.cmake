
include_guard(GLOBAL)

set(GVK_STRUCTURES_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/cppgen)
include(gvk/runtime)
include(external/cereal)

add_subdirectory(gvk-structures)
