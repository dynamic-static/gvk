
include_guard(GLOBAL)

set(GVK_GUI_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/handles)
include(gvk/spirv)
include(gvk/system)
include(external/imgui)

add_subdirectory(gvk-gui)
