
include_guard(GLOBAL)

set(GVK_SPIRV_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/handles)
include(external/glslang)
include(external/SPIRV-Cross)
include(external/SPIRV-Headers)
include(external/SPIRV-Tools)

add_subdirectory(gvk-spirv)
