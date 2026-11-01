
list(APPEND gvkCoreSampleLinkLibraries
    gvk-gui
    gvk-handles
    gvk-math
    gvk-runtime
    gvk-spirv
    gvk-structures
    gvk-system
    stb
)

set(gvkCoreSamplesEnabled ${gvk-build-samples})
foreach(gvkCoreSampleLinkLibrary ${gvkCoreSampleLinkLibraries})
    if(NOT ${${gvkCoreSampleLinkLibrary}_ENABLED})
        unset(gvkCoreSamplesEnabled)
    endif()
endforeach()

# NOTE : When enabled, gvk_add_core_sample() also generates an Android Gradle app for
#   each sample (gvk/android/samples/<name>/) via gvk_add_android_app() -- see
#   gvk/cmake/android.cmake.  Off by default: this writes files under gvk/android/
#   every configure, and most desktop configures don't care about Android at all.
option(gvk-build-android-samples "Generate an Android Gradle app for each core sample" OFF)

macro(gvk_add_core_sample target linkLibraries)
    gvk_add_executable(
        TARGET ${target}
        FOLDER "samples/"
        LINK_LIBRARIES ${linkLibraries}
        SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-entry-points.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-entry-points.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-png.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-utilities.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/${target}.cpp"
    )
    if(VK_NO_PROTOTYPES)
        target_compile_definitions(${target} PUBLIC VK_NO_PROTOTYPES=1)
    endif()
    if(gvk-build-android-samples)
        # NOTE : "gvk-getting-started-00-triangle" -> "getting_started_00_triangle" --
        #   Java package segments can't contain hyphens, see kaiju session notes,
        #   package name sanitization.
        string(REPLACE "gvk-" "" androidSampleName "${target}")
        string(REPLACE "-" "_" androidSampleName "${androidSampleName}")
        gvk_add_android_app(
            ANDROID_TARGET "gvk_${androidSampleName}"
            PACKAGE "com.gvk.${androidSampleName}"
            APP_LABEL "${target}"
            OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../android/samples/gvk_${androidSampleName}"
            GRADLE_MODULE_PATH ":samples:gvk_${androidSampleName}"
            # NOTE : 6 "../" from src/main/cpp/ (gvk/android/samples/<name>/src/main/cpp/)
            #   back up to gvk's own repo root.
            GVK_SOURCE_DIR "\${CMAKE_CURRENT_LIST_DIR}/../../../../../.."
            LINK_LIBRARIES ${linkLibraries}
            SOURCE_FILES
                "\${gvkSamplesDir}/gvk-sample-entry-points.cpp"
                "\${gvkSamplesDir}/gvk-sample-entry-points.hpp"
                "\${gvkSamplesDir}/gvk-sample-png.hpp"
                "\${gvkSamplesDir}/gvk-sample-utilities.hpp"
                "\${gvkSamplesDir}/${target}.cpp"
            INCLUDE_DIRECTORIES "\${gvkSamplesDir}"
            # NOTE : No trailing ';' -- the template supplies it.  CMake can't tell "a
            #   string containing a literal semicolon" apart from "a list" (worse inside
            #   a macro body, where \; escaping didn't survive expansion either), so an
            #   embedded ';' here gets silently eaten -- found for real: the generated
            #   android-main.cpp lost this declaration's trailing semicolon.
            ENTRY_POINT_FORWARD_DECLARATION "int gvk_main(int argc, const char* ppArgv[])"
            ENTRY_POINT_CALL "gvk_main(0, nullptr)"
        )
    endif()
endmacro()

if(gvkCoreSamplesEnabled)
    # NOTE : Samples set to 896 x 504 for screenshots
    gvk_add_core_sample(gvk-getting-started-00-triangle        "${gvkCoreSampleLinkLibraries}")
    gvk_add_core_sample(gvk-getting-started-01-mesh            "${gvkCoreSampleLinkLibraries}")
    gvk_add_core_sample(gvk-getting-started-02-uniform-buffer  "${gvkCoreSampleLinkLibraries}")
    gvk_add_core_sample(gvk-getting-started-03-texture-mapping "${gvkCoreSampleLinkLibraries}")
    gvk_add_core_sample(gvk-getting-started-04-render-target   "${gvkCoreSampleLinkLibraries}")
    gvk_add_core_sample(gvk-getting-started-05-gui             "${gvkCoreSampleLinkLibraries}")
    if(gvk-build-android-samples)
        gvk_configure_android_root(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../android")
    endif()
endif()

if(gvk-string_ENABLED)
    gvk_add_executable(
        TARGET gvk-log-parser
        FOLDER "samples/"
        LINK_LIBRARIES gvk-string
        SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/gvk-log-parser.cpp"
    )
endif()

if(gvk-xml_ENABLED)
    gvk_add_executable(
        TARGET gvk-xml-compare
        FOLDER "samples/"
        LINK_LIBRARIES gvk-xml
        SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/gvk-xml-compare.cpp"
    )
    gvk_add_executable(
        TARGET gvk-xml-search
        FOLDER "samples/"
        LINK_LIBRARIES gvk-xml
        SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/gvk-xml-search.cpp"
    )
endif()

if(gvk-layer_ENABLED)
    gvk_add_layer(
        TARGET VK_LAYER_INTEL_gvk_sample
        FOLDER "samples/"
        LINK_LIBRARIES gvk-layer
        SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-layer.cpp"
        DESCRIPTION "Intel(R) GPA Utilities for Vulkan* sample layer"
    )
endif()

if(gvk-state-tracker_ENABLED AND MSVC)
    gvk_add_executable(
        TARGET gvk-metadata-extractor-sample
        FOLDER "samples/gvk-metadata-extractor-sample/"
        LINK_LIBRARIES
            gvk-command-structures
            gvk-gui
            gvk-handles
            gvk-system
            VK_LAYER_INTEL_gvk_state_tracker_metadata_extractor
        INCLUDE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/gvk-metadata-extractor-sample-utilities.hpp"
        SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/gvk-metadata-extractor-sample.cpp"
    )
    gvk_add_layer(
        TARGET VK_LAYER_INTEL_gvk_metadata_extractor_sample
        FOLDER "samples/gvk-metadata-extractor-sample/"
        LINK_LIBRARIES
            gvk-containers
            gvk-handles
            VK_LAYER_INTEL_gvk_state_tracker_metadata_extractor
        INCLUDE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/gvk-metadata-extractor-sample-utilities.hpp"
        SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/gvk-metadata-extractor-sample-layer.cpp"
        DESCRIPTION
            "Intel(R) GPA Utilities for Vulkan* metadata extractor sample layer"
    )
    add_custom_command(
        TARGET gvk-metadata-extractor-sample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:VK_LAYER_INTEL_gvk_state_tracker>" "$<TARGET_FILE_DIR:gvk-metadata-extractor-sample>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE_DIR:VK_LAYER_INTEL_gvk_state_tracker>/VK_LAYER_INTEL_gvk_state_tracker.json" "$<TARGET_FILE_DIR:gvk-metadata-extractor-sample>"
    )
endif()
