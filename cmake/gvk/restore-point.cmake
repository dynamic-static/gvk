
include_guard(GLOBAL)

set(GVK_RESTORE_POINT_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/cppgen)
include(gvk/command-structures)
include(gvk/format-info)
include(gvk/handles)
include(gvk/reference)
include(gvk/restore-info)
include(gvk/runtime)
include(gvk/state-tracker)
include(external/asio)
include(external/stb)

add_subdirectory(gvk-restore-point)
