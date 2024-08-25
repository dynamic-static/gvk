
macro(gvk_add_sample target)
    gvk_add_executable(
        TARGET ${target}
        FOLDER "samples/"
        LINK_LIBRARIES gvk stb
        SOURCE_FILES
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-entry-points.cpp"
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-entry-points.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-png.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-utilities.hpp"
            "${CMAKE_CURRENT_LIST_DIR}/${target}.cpp"
    )
endmacro()

if(
    gvk-runtime-enabled     AND
    gvk-handles-enabled     AND
    gvk-format-info-enabled AND
    gvk-gui-enabled         AND
    gvk-math-enabled        AND
    gvk-reference-enabled   AND
    gvk-spirv-enabled       AND
    gvk-string-enabled      AND
    gvk-structures-enabled  AND
    gvk-system-enabled
)
    gvk_add_sample(gvk-getting-started-00-triangle)
    gvk_add_sample(gvk-getting-started-01-mesh)
    gvk_add_sample(gvk-getting-started-02-uniform-buffer)
    gvk_add_sample(gvk-getting-started-03-texture-mapping)
    gvk_add_sample(gvk-getting-started-04-render-target)
    gvk_add_sample(gvk-getting-started-05-gui)
endif()

if(gvk-string-enabled)
    gvk_add_executable(
        TARGET gvk-log-parser
        FOLDER "samples/"
        LINK_LIBRARIES gvk-string
        SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/gvk-log-parser.cpp"
    )
endif()

if(gvk-layer-enabled AND gvk-structures-enabled)
    gvk_add_layer(
        TARGET VK_LAYER_INTEL_gvk_sample
        FOLDER "samples/"
        LINK_LIBRARIES gvk-layer gvk-structures
        SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/gvk-sample-layer.cpp"
        DESCRIPTION "Intel(R) GPA Utilities for Vulkan* sample layer"
    )
endif()
