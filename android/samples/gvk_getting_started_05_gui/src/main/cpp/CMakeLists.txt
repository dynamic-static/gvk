
cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
project(gvk_getting_started_05_gui)

# NOTE : gvk-system's find_package(game-activity CONFIG) only resolves during a CMake
#   configure that Gradle itself launches (Prefab injects the necessary find paths at that
#   point), so gvk is pulled in as a live subdirectory of *this* configure rather than
#   consumed as a prebuilt install package the way desktop kaiju does.  Enables the same
#   module set gvk's own desktop samples link (see gvk/samples/CMakeLists.txt's
#   gvkCoreSampleLinkLibraries) -- see kaiju session-notes, plan step 5.
set(gvk-default_ENABLED OFF CACHE BOOL "" FORCE)
set(gvk-gui_ENABLED ON CACHE BOOL "" FORCE)
set(gvk-handles_ENABLED ON CACHE BOOL "" FORCE)
set(gvk-math_ENABLED ON CACHE BOOL "" FORCE)
set(gvk-runtime_ENABLED ON CACHE BOOL "" FORCE)
set(gvk-spirv_ENABLED ON CACHE BOOL "" FORCE)
set(gvk-structures_ENABLED ON CACHE BOOL "" FORCE)
set(gvk-system_ENABLED ON CACHE BOOL "" FORCE)
set(stb_ENABLED ON CACHE BOOL "" FORCE)
# NOTE : gvk-HOST_BINARY_DIR defaults to a path nested under this configure's own binary
#   directory, which under Gradle's already-deep .cxx/<config>/<hash>/<abi>/ tree pushes
#   the nested host build's own dependencies (eg. boost-multiprecision's FetchContent
#   subbuild) past Windows' 260-character MAX_PATH -- confirmed by a real
#   "lastbuildstate...exceeds the OS max path limit" build failure.  Redirecting it
#   somewhere short and stable also means the nested host-generate build is shared/reused
#   across Android app configures instead of being tied to one Gradle variant's path.
set(gvkSourceDir "${CMAKE_CURRENT_LIST_DIR}/../../../../../..")
set(gvk-HOST_BINARY_DIR "${gvkSourceDir}/build-android-host" CACHE PATH "" FORCE)
# NOTE : Same MAX_PATH problem, different victim: FETCHCONTENT_BASE_DIR (where the OUTER/
#   target build's own FetchContent subbuilds land, eg. VulkanMemoryAllocator) defaults to
#   under THIS configure's own (deep, Gradle-.cxx-nested) binary directory too.  Hit for
#   real on gvk_getting_started_02_uniform_buffer, whose longer target name was enough to
#   tip an already-marginal path over 260 characters.  Same fix, same reasoning as
#   gvk-HOST_BINARY_DIR above.
set(FETCHCONTENT_BASE_DIR "${gvkSourceDir}/build-android-deps" CACHE PATH "" FORCE)
add_subdirectory("${gvkSourceDir}" gvk-build)

# NOTE : gvk-getting-started-05-gui.cpp and its companion files are the SAME shared
#   source gvk's desktop build uses (gvk/samples/) -- not a separate Android copy.
#   android-main.cpp is the only file here written for this app specifically.
set(gvkSamplesDir "${gvkSourceDir}/samples")
add_library(
    gvk_getting_started_05_gui
    SHARED
    android-main.cpp
    "${gvkSamplesDir}/gvk-getting-started-05-gui.cpp"
    "${gvkSamplesDir}/gvk-sample-entry-points.cpp"
    "${gvkSamplesDir}/gvk-sample-entry-points.hpp"
    "${gvkSamplesDir}/gvk-sample-png.hpp"
    "${gvkSamplesDir}/gvk-sample-utilities.hpp"
)
target_include_directories(gvk_getting_started_05_gui PRIVATE "${gvkSamplesDir}")

target_link_libraries(
    gvk_getting_started_05_gui
    PUBLIC
    gvk-gui
    gvk-handles
    gvk-math
    gvk-runtime
    gvk-spirv
    gvk-structures
    gvk-system
    stb
)

if(VK_NO_PROTOTYPES)
    target_compile_definitions(gvk_getting_started_05_gui PUBLIC VK_NO_PROTOTYPES=1)
endif()
