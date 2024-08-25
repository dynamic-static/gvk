
include_guard(GLOBAL)

################################################################################
# TODO : Documentation
gvk_enable_target(glm)

################################################################################
# TODO : Documentation
set(glm_VERSION 0af55ccecd98d4e5a8d1fad7de25ba429d60e863) # 1.0.1
FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm.git"
    GIT_TAG ${glm_VERSION}
    GIT_PROGRESS TRUE
)

################################################################################
# TODO : Documentation
FetchContent_MakeAvailable(glm)
set_target_properties(glm PROPERTIES FOLDER "${GVK_IDE_FOLDER}/external/")

################################################################################
# TODO : Documentation
if(gvk-glm-install-artifacts)
    gvk_install_artifacts(TARGET glm-header-only VERSION ${glm_VERSION})
endif()
if(gvk-glm-install-headers)
    install(DIRECTORY "${glm_SOURCE_DIR}/glm/" DESTINATION include/glm/)
endif()
