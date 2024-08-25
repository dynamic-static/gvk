
include_guard(GLOBAL)

message("Entered SPIRV-Cross.cmake")

################################################################################
# TODO : Documentation
gvk_enable_target(SPIRV-Cross)

################################################################################
# TODO : Documentation
set(SPIRV_CROSS_CLI          OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_FORCE_PIC    ON  CACHE BOOL "" FORCE)
set(SPIRV_CROSS_SKIP_INSTALL ON  CACHE BOOL "" FORCE)
set(SPIRV-Cross_VERSION 2ccc81fd826e4dd4a2db2f94b8e6eb738a89f5f1) # vulkan-sdk-1.3.283.0
FetchContent_Declare(
    SPIRV-Cross
    GIT_REPOSITORY "https://github.com/KhronosGroup/SPIRV-Cross.git"
    GIT_TAG ${SPIRV-Cross_VERSION}
    GIT_PROGRESS TRUE
)

################################################################################
# TODO : Documentation
FetchContent_MakeAvailable(SPIRV-Cross)
FetchContent_GetProperties(SPIRV-Cross SOURCE_DIR SPIRV-Cross_SOURCE_DIR)

################################################################################
# TODO : Documentation
macro(gvk_setup_spirv_cross_target spirvCrossTarget)
    message("spirvCrossTarget = ${spirvCrossTarget}")
    list(APPEND spirvCrossLibraries ${spirvCrossTarget})
    set_target_properties(${spirvCrossTarget} PROPERTIES FOLDER "${GVK_IDE_FOLDER}/external/SPIRV-Cross/")
    gvk_install_artifacts(TARGET ${spirvCrossTarget} VERSION ${SPIRV-Cross_VERSION})
endmacro()

################################################################################
# TODO : Documentation
gvk_setup_spirv_cross_target(spirv-cross-c)
gvk_setup_spirv_cross_target(spirv-cross-core)
gvk_setup_spirv_cross_target(spirv-cross-cpp)
gvk_setup_spirv_cross_target(spirv-cross-glsl)
gvk_setup_spirv_cross_target(spirv-cross-hlsl)
gvk_setup_spirv_cross_target(spirv-cross-msl)
gvk_setup_spirv_cross_target(spirv-cross-reflect)
gvk_setup_spirv_cross_target(spirv-cross-util)
set(spirvCrossLibraries ${spirvCrossLibraries} PARENT_SCOPE)

################################################################################
# TODO : Documentation
if(gvk-SPIRV-Cross-install-headers)
    file(GLOB spirvCrossHeaderFiles "${SPIRV-Cross_SOURCE_DIR}/*.h" "${SPIRV-Cross_SOURCE_DIR}/*.hpp")
    install(FILES ${spirvCrossHeaderFiles} DESTINATION include/spirv_cross/)
endif()

message("--------------------------------------------")
message("spirvCrossLibraries = ${spirvCrossLibraries}")
message("--------------------------------------------")
message("Leaving SPIRV-Cross.cmake")
