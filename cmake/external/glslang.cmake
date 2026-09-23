
include_guard(GLOBAL)
gvk_enable_external_module(SPIRV-Tools)

set(BUILD_TESTING           OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_INSTALL  OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_JS       OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_WEBMIN   OFF CACHE BOOL "" FORCE)
set(SKIP_GLSLANG_INSTALL    ON  CACHE BOOL "" FORCE)
set(glslang_VERSION b5782e52ee2f7b3e40bb9c80d15b47016e008bc9) # vulkan-sdk-1.4.335.0
FetchContent_Declare(
    glslang
    GIT_REPOSITORY "https://github.com/KhronosGroup/glslang.git"
    GIT_TAG ${glslang_VERSION}
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(glslang)

# HACK : glslang headers aren't being installed, but the export expects them to
#   be at `include/External`...this modifies the INSTALL_INTERFACE to `include`
#   to avoid an error for missing INTERFACE_INCLUDE_DIRECTORIES on import.
get_target_property(SPIRV_INTERFACE_INCLUDE_DIRECTORIES SPIRV INTERFACE_INCLUDE_DIRECTORIES)
string(REPLACE "$<INSTALL_INTERFACE:include/External>" "$<INSTALL_INTERFACE:include>" SPIRV_INTERFACE_INCLUDE_DIRECTORIES "${SPIRV_INTERFACE_INCLUDE_DIRECTORIES}")
set_target_properties(SPIRV PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SPIRV_INTERFACE_INCLUDE_DIRECTORIES}")

macro(gvk_setup_glslang_target glslangTarget)
    list(APPEND glslangLibraries ${glslangTarget})
    set_target_properties(${glslangTarget} PROPERTIES FOLDER "${GVK_IDE_FOLDER}/external/glslang/")
    if(glslang_INSTALL_ARTIFACTS)
        gvk_install_artifacts(TARGET ${glslangTarget} VERSION ${glslang_VERSION})
    endif()
    if(glslang_INSTALL_HEADERS)
        # TODO :
    endif()
endmacro()

gvk_setup_glslang_target(GenericCodeGen)
gvk_setup_glslang_target(glslang)
gvk_setup_glslang_target(glslang-default-resource-limits)
gvk_setup_glslang_target(MachineIndependent)
gvk_setup_glslang_target(OSDependent)
gvk_setup_glslang_target(SPIRV)
# NOTE : KNOWN ISSUE - `PARENT_SCOPE` only escapes one function call level.  When glslang
#   is enabled directly (eg. `-Dgvk-default_ENABLED=OFF -Dgvk-spirv_ENABLED=ON`, as when
#   selectively cross-compiling for Android), the enable chain is one function call deeper
#   than the default all-modules-on path (gvk_enable_module -> dependencies.cmake ->
#   gvk_enable_external_module -> this file), so `glslangLibraries` gets lost one level
#   before reaching the top-level/global scope that gvk-spirv/CMakeLists.txt reads it
#   from.  Confirmed 100% reproducible: a FRESH configure that selectively enables
#   gvk-spirv fails (`gvk-spirv`'s link line is missing every glslang library, causing
#   `glslang/Public/ResourceLimits.h` not found), but simply reconfiguring the SAME tree
#   again (no new flags needed) fixes it, every time.  The reason a bare reconfigure fixes
#   it isn't fully understood (normal variables shouldn't persist across separate cmake
#   process invocations; likely interacts with FetchContent's first-populate code path).
#   Workaround for now: configure twice.  MUST be fixed properly (eg. replace this
#   PARENT_SCOPE with a GLOBAL PROPERTY, the same technique used for
#   gvk-host-required-modules in gvk.build.cmake) before merging android-support to trunk.
#   See kaiju session-notes/2026_09_23_01_gvk-spirv-cross-compile.md for the full writeup.
set(glslangLibraries ${glslangLibraries} PARENT_SCOPE)
