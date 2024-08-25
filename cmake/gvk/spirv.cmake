
include_guard(GLOBAL)

message("Entered gvk-spirv.cmake")

gvk_enable_target(gvk-spirv)

include(gvk/handles)
include(external/glslang)
include(external/SPIRV-Cross)
include(external/SPIRV-Headers)
include(external/SPIRV-Tools)

add_subdirectory(gvk-spirv)

message("Leaving gvk-spirv.cmake")
