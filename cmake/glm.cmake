
include_guard()

include(FetchContent)

# set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(glm_VERSION 0af55ccecd98d4e5a8d1fad7de25ba429d60e863) # 1.0.1
FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm.git"
    GIT_TAG ${glm_VERSION}
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glm)
set_target_properties(glm PROPERTIES FOLDER "${GVK_IDE_FOLDER}/external/")
# FetchContent_GetProperties(glm SOURCE_DIR glm_SOURCE_DIR)


gvk_install_library(TARGET glm-header-only VERSION ${glm_VERSION})



if (0)
install(DIRECTORY "${glm_SOURCE_DIR}/glm/" DESTINATION include/glm/)




if(0)
    set(ARGS_TARGET glm)
    install(TARGETS glm EXPORT glmTargets)
    export(EXPORT glmTargets FILE "${CMAKE_BINARY_DIR}/cmake/glmTargets.cmake")
    install(EXPORT glmTargets DESTINATION cmake/glm/)
    set(configVersion "${CMAKE_BINARY_DIR}/cmake/glmConfigVersion.cmake")
    write_basic_package_version_file("${configVersion}" VERSION ${glm_VERSION} COMPATIBILITY ExactVersion)
    set(configTemplate "${gvkBuildModuleDirectory}/gvk-target.config.cmake.in")
    set(config "${CMAKE_BINARY_DIR}/cmake/glmConfig.cmake")

    get_target_property(interfaceLinkLibraries glm INTERFACE_LINK_LIBRARIES)
    string(REPLACE "$" "" interfaceLinkLibraries "${interfaceLinkLibraries}")
    string(REPLACE "<LINK_ONLY:" "" interfaceLinkLibraries "${interfaceLinkLibraries}")
    string(REPLACE ">" "" interfaceLinkLibraries "${interfaceLinkLibraries}")
    list(REMOVE_ITEM interfaceLinkLibraries ${CMAKE_DL_LIBS} glm-header-only)

    configure_package_config_file("${configTemplate}" "${config}" INSTALL_DESTINATION {CMAKE_BINARY_DIR}/cmake/)
    install(FILES "${config}" "${configVersion}" DESTINATION cmake/glm/)
else()
    gvk_install_artifacts(TARGET glm VERSION ${glm_VERSION})
endif()

endif()

# export(EXPORT ${ARGS_TARGET}Targets FILE "${CMAKE_BINARY_DIR}/cmake/${ARGS_TARGET}Targets.cmake")
# install(EXPORT ${ARGS_TARGET}Targets DESTINATION cmake/${ARGS_TARGET}/)
# set(configVersion "${CMAKE_BINARY_DIR}/cmake/${ARGS_TARGET}ConfigVersion.cmake")
# write_basic_package_version_file("${configVersion}" VERSION ${ARGS_VERSION} COMPATIBILITY ExactVersion)
# set(configTemplate "${gvkBuildModuleDirectory}/gvk-target.config.cmake.in")
# set(config "${CMAKE_BINARY_DIR}/cmake/${ARGS_TARGET}Config.cmake")
# get_target_property(interfaceLinkLibraries ${ARGS_TARGET} INTERFACE_LINK_LIBRARIES)
# string(REPLACE "$" "" interfaceLinkLibraries "${interfaceLinkLibraries}")
# string(REPLACE "<LINK_ONLY:" "" interfaceLinkLibraries "${interfaceLinkLibraries}")
# string(REPLACE ">" "" interfaceLinkLibraries "${interfaceLinkLibraries}")
# list(REMOVE_ITEM interfaceLinkLibraries ${CMAKE_DL_LIBS} rt Threads::Threads Vulkan::Vulkan)
# configure_package_config_file("${configTemplate}" "${config}" INSTALL_DESTINATION {CMAKE_BINARY_DIR}/cmake/)
# install(FILES "${config}" "${configVersion}" DESTINATION cmake/${ARGS_TARGET}/)
