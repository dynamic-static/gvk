
################################################################################
# gvk-system
set(linkLibraries
    gvk-reference
    gvk-string
)
if(ANDROID)
    # NOTE : find_package(game-activity) only resolves during a CMake configure that
    #   Gradle itself launches (Prefab injects the necessary find paths at that point) --
    #   gvk-system's Android backend can only be built as part of such a configure, not
    #   cross-compiled standalone the way gvk-handles/gvk-spirv were.  See kaiju
    #   session-notes, plan step 5.
    find_package(game-activity REQUIRED CONFIG)
    list(APPEND linkLibraries android log game-activity::game-activity_static)
else()
    list(APPEND linkLibraries glfw)
endif()
set(includeDirectory "${CMAKE_CURRENT_LIST_DIR}/include/")
set(includePath "${includeDirectory}/gvk-system/")
set(sourcePath "${CMAKE_CURRENT_LIST_DIR}/source/gvk-system/")
gvk_add_static_library(
    TARGET
        gvk-system
    FOLDER
        "gvk-system/"
    LINK_LIBRARIES
        ${linkLibraries}
    INCLUDE_DIRECTORIES
        "${includeDirectory}"
    INCLUDE_FILES
        "${includePath}/button-set.hpp"
        "${includePath}/entry-point.hpp"
        "${includePath}/input.hpp"
        "${includePath}/keyboard.hpp"
        "${includePath}/mouse.hpp"
        "${includePath}/surface.hpp"
        "${includePath}/time.hpp"
        "${includePath}/updateable.hpp"
        "${includeDirectory}/gvk-system.hpp"
    SOURCE_FILES
        "${sourcePath}/android-game-activity-surface.cpp"
        "${sourcePath}/android-game-activity-surface.hpp"
        "${sourcePath}/glfw-surface.cpp"
        "${sourcePath}/glfw-surface.hpp"
)
if(ANDROID)
    # NOTE : game-activity_static's JNI implementation of GameActivity.initializeNativeCode()
    #   is otherwise stripped by the linker, since nothing calls it directly -- only the JVM
    #   looks it up dynamically by name at runtime.  Without this, every consumer crashes at
    #   launch with UnsatisfiedLinkError.  PUBLIC so it propagates to whatever links gvk-system.
    target_link_options(
        gvk-system
        PUBLIC
        "-u" "Java_com_google_androidgamesdk_GameActivity_initializeNativeCode"
    )
endif()

################################################################################
# gvk-system
if(gvk-system_INSTALL_ARTIFACTS)
    gvk_install_artifacts(TARGET gvk-system)
endif()
if(gvk-system_INSTALL_HEADERS)
    gvk_install_headers(TARGET gvk-system)
endif()
