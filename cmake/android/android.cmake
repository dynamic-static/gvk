
include_guard()

# NOTE : Generates an Android Gradle app (build.gradle, AndroidManifest.xml,
#   MainActivity.java, themes.xml, and a src/main/cpp/CMakeLists.txt + android-main.cpp
#   that pull gvk in as a live source subdirectory of Gradle's own CMake configure,
#   since find_package(game-activity CONFIG) only resolves during a configure Gradle
#   itself launches -- Prefab injects the necessary find paths at that point).
#   Regenerated every configure -- nothing under OUTPUT_DIR should be hand-edited, it
#   will be overwritten.  See gvk/samples/CMakeLists.txt's gvk_add_core_sample() for the
#   first real caller.
function(gvk_add_android_app)
    cmake_parse_arguments(
        ARGS
        ""
        "ANDROID_TARGET;PACKAGE;APP_LABEL;OUTPUT_DIR;GRADLE_MODULE_PATH;GVK_SOURCE_DIR;COMPILE_SDK;MIN_SDK;TARGET_SDK;NDK_VERSION;GRADLE_CMAKE_VERSION;ENTRY_POINT_FORWARD_DECLARATION;ENTRY_POINT_CALL"
        "LINK_LIBRARIES;SOURCE_FILES;INCLUDE_DIRECTORIES;EXTRA_INCLUDES"
        ${ARGN}
    )
    if(NOT ARGS_GVK_SOURCE_DIR)
        message(FATAL_ERROR "gvk_add_android_app(${ARGS_ANDROID_TARGET}) : GVK_SOURCE_DIR is required -- how the generated CMakeLists.txt finds gvk's source tree from its own location varies by caller (eg. a gvk sample vs. a different project entirely), so there's no sane default.")
    endif()
    if(NOT ARGS_APP_LABEL)
        set(ARGS_APP_LABEL "${ARGS_ANDROID_TARGET}")
    endif()
    if(NOT ARGS_COMPILE_SDK)
        set(ARGS_COMPILE_SDK 37)
    endif()
    if(NOT ARGS_MIN_SDK)
        set(ARGS_MIN_SDK 24) # NOTE : Vulkan's own Android floor -- see kaiju session notes.
    endif()
    if(NOT ARGS_TARGET_SDK)
        set(ARGS_TARGET_SDK 37)
    endif()
    if(NOT ARGS_NDK_VERSION)
        set(ARGS_NDK_VERSION "28.2.13676358")
    endif()
    if(NOT ARGS_GRADLE_CMAKE_VERSION)
        set(ARGS_GRADLE_CMAKE_VERSION "3.30.1")
    endif()

    # NOTE : configure_file(... @ONLY) only substitutes variables in ITS OWN scope, so
    #   these are named to match the @VAR@ tokens in the .in templates exactly.
    set(PACKAGE "${ARGS_PACKAGE}")
    set(ANDROID_TARGET "${ARGS_ANDROID_TARGET}")
    set(APP_LABEL "${ARGS_APP_LABEL}")
    set(COMPILE_SDK "${ARGS_COMPILE_SDK}")
    set(MIN_SDK "${ARGS_MIN_SDK}")
    set(TARGET_SDK "${ARGS_TARGET_SDK}")
    set(NDK_VERSION "${ARGS_NDK_VERSION}")
    set(GRADLE_CMAKE_VERSION "${ARGS_GRADLE_CMAKE_VERSION}")
    set(ENTRY_POINT_FORWARD_DECLARATION "${ARGS_ENTRY_POINT_FORWARD_DECLARATION}")
    set(ENTRY_POINT_CALL "${ARGS_ENTRY_POINT_CALL}")
    set(GVK_SOURCE_DIR "${ARGS_GVK_SOURCE_DIR}")

    set(ENABLED_MODULES_BLOCK "set(gvk-default_ENABLED OFF CACHE BOOL \"\" FORCE)")
    set(LINK_LIBRARIES_BLOCK "")
    foreach(linkLibrary ${ARGS_LINK_LIBRARIES})
        string(APPEND ENABLED_MODULES_BLOCK "\nset(${linkLibrary}_ENABLED ON CACHE BOOL \"\" FORCE)")
        if(LINK_LIBRARIES_BLOCK)
            string(APPEND LINK_LIBRARIES_BLOCK "\n")
        endif()
        string(APPEND LINK_LIBRARIES_BLOCK "    ${linkLibrary}")
    endforeach()

    # NOTE : Each entry in SOURCE_FILES is a literal, caller-constructed CMake path
    #   expression (eg. "\${gvkSamplesDir}/some-file.cpp") -- this function has no
    #   opinion on where a caller's source actually lives relative to the generated
    #   file, only GVK_SOURCE_DIR (gvk's own location) is universal across every
    #   caller.  gvkSourceDir/gvkSamplesDir are defined by
    #   gvk-android-cpp-cmakelists.txt.in itself (not substituted here) -- these
    #   expressions are resolved when the GENERATED file is configured by Gradle's own
    #   CMake invocation, not now.
    set(SOURCE_FILES_BLOCK "")
    foreach(sourceFile ${ARGS_SOURCE_FILES})
        if(SOURCE_FILES_BLOCK)
            string(APPEND SOURCE_FILES_BLOCK "\n")
        endif()
        string(APPEND SOURCE_FILES_BLOCK "    \"${sourceFile}\"")
    endforeach()

    # NOTE : Same caller-constructed-literal-expression pattern as SOURCE_FILES above.
    #   Always include gvkSamplesDir isn't assumed -- a caller whose source doesn't live
    #   under gvk's own samples dir (eg. a different project entirely) has no use for it.
    set(INCLUDE_DIRECTORIES_BLOCK "")
    foreach(includeDirectory ${ARGS_INCLUDE_DIRECTORIES})
        if(INCLUDE_DIRECTORIES_BLOCK)
            string(APPEND INCLUDE_DIRECTORIES_BLOCK "\n")
        endif()
        string(APPEND INCLUDE_DIRECTORIES_BLOCK "    \"${includeDirectory}\"")
    endforeach()

    # NOTE : android-main.cpp is otherwise gvk-only (<gvk-system.hpp> + the NDK/log
    #   headers) -- a caller whose entry point needs a type/header of its own (eg.
    #   kaiju::Game) supplies it here as literal #include lines.
    set(EXTRA_INCLUDES_BLOCK "")
    foreach(extraInclude ${ARGS_EXTRA_INCLUDES})
        if(EXTRA_INCLUDES_BLOCK)
            string(APPEND EXTRA_INCLUDES_BLOCK "\n")
        endif()
        string(APPEND EXTRA_INCLUDES_BLOCK "${extraInclude}")
    endforeach()

    set(packagePath "${ARGS_PACKAGE}")
    string(REPLACE "." "/" packagePath "${packagePath}")

    # NOTE : NEWLINE_STYLE LF matches every one of these files as hand-written (Gradle/
    #   Java/XML/the Android app's own CMakeLists.txt) -- without it, configure_file()
    #   defaults to the host platform's convention, which on Windows means silently
    #   rewriting every line ending to CRLF (found for real: a diff against the
    #   originals that otherwise looked identical).
    set(templateDir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    configure_file("${templateDir}/gvk-android-build.gradle.in" "${ARGS_OUTPUT_DIR}/build.gradle" @ONLY NEWLINE_STYLE LF)
    configure_file("${templateDir}/gvk-android-manifest.xml.in" "${ARGS_OUTPUT_DIR}/src/main/AndroidManifest.xml" @ONLY NEWLINE_STYLE LF)
    configure_file("${templateDir}/gvk-android-activity.java.in" "${ARGS_OUTPUT_DIR}/src/main/java/${packagePath}/MainActivity.java" @ONLY NEWLINE_STYLE LF)
    configure_file("${templateDir}/gvk-android-themes.xml.in" "${ARGS_OUTPUT_DIR}/src/main/res/values/themes.xml" COPYONLY)
    configure_file("${templateDir}/gvk-android-cpp-cmakelists.txt.in" "${ARGS_OUTPUT_DIR}/src/main/cpp/CMakeLists.txt" @ONLY NEWLINE_STYLE LF)
    configure_file("${templateDir}/gvk-android-main.cpp.in" "${ARGS_OUTPUT_DIR}/src/main/cpp/android-main.cpp" @ONLY NEWLINE_STYLE LF)

    set_property(GLOBAL APPEND PROPERTY gvk-android-gradle-modules "${ARGS_GRADLE_MODULE_PATH}")
endfunction()

# NOTE : Call once, after every gvk_add_android_app() call for a given Gradle project
#   root, to (re)generate settings.gradle from the accumulated module list and refresh
#   the root-level files every Gradle project needs (the wrapper, build.gradle,
#   gradle.properties) -- none of these have any gvk- or sample-specific content, so
#   they're tracked once here and copied, same as everything gvk_add_android_app()
#   generates.  OUTPUT_DIR ends up entirely generated/copied with no hand-maintained
#   exceptions, so it can be gitignored wholesale rather than file-by-file.
function(gvk_configure_android_root)
    cmake_parse_arguments(ARGS "" "OUTPUT_DIR" "" ${ARGN})
    get_property(gradleModules GLOBAL PROPERTY gvk-android-gradle-modules)
    list(REMOVE_DUPLICATES gradleModules)
    set(MODULE_INCLUDES_BLOCK "")
    foreach(gradleModule ${gradleModules})
        if(MODULE_INCLUDES_BLOCK)
            string(APPEND MODULE_INCLUDES_BLOCK "\n")
        endif()
        string(APPEND MODULE_INCLUDES_BLOCK "include '${gradleModule}'")
    endforeach()
    set(templateDir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    configure_file("${templateDir}/gvk-android-settings.gradle.in" "${ARGS_OUTPUT_DIR}/settings.gradle" @ONLY NEWLINE_STYLE LF)
    configure_file("${templateDir}/gvk-android-root-build.gradle" "${ARGS_OUTPUT_DIR}/build.gradle" COPYONLY)
    configure_file("${templateDir}/gvk-android-gradle.properties" "${ARGS_OUTPUT_DIR}/gradle.properties" COPYONLY)
    # NOTE : The wrapper's own relative layout (gradlew next to a gradle/wrapper/
    #   directory) is load-bearing -- Gradle's launcher script looks for the jar/
    #   properties at that exact relative path, so it's preserved verbatim here.
    #   COPYONLY doesn't guarantee gradlew's executable bit survives on every platform
    #   (not exercised on Linux/macOS yet -- Windows only uses gradlew.bat).
    configure_file("${templateDir}/gradle-wrapper/gradlew" "${ARGS_OUTPUT_DIR}/gradlew" COPYONLY)
    configure_file("${templateDir}/gradle-wrapper/gradlew.bat" "${ARGS_OUTPUT_DIR}/gradlew.bat" COPYONLY)
    configure_file("${templateDir}/gradle-wrapper/gradle/wrapper/gradle-wrapper.jar" "${ARGS_OUTPUT_DIR}/gradle/wrapper/gradle-wrapper.jar" COPYONLY)
    configure_file("${templateDir}/gradle-wrapper/gradle/wrapper/gradle-wrapper.properties" "${ARGS_OUTPUT_DIR}/gradle/wrapper/gradle-wrapper.properties" COPYONLY)
endfunction()
