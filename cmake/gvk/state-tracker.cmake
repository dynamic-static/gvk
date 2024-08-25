
include_guard(GLOBAL)

gvk_enable_target(gvk-state-tracker)

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
