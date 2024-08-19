
include_guard(GLOBAL)

set(GVK_STATE_TRACKER_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/command-structures)
include(gvk/cppgen)
include(gvk/reference)
include(gvk/runtime)

# TODO : If tests
include(gvk/handles)
include(gvk/math)
include(gvk/spirv)
include(gvk/system)

add_subdirectory(gvk-state-tracker)
