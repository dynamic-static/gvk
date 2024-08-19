
include_guard(GLOBAL)

################################################################################
# TODO : Documentation
set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(googletest_VERSION f8d7d77c06936315286eb55f8de22cd23c188571) # 1.14.0
FetchContent_Declare(
    googletest
    GIT_REPOSITORY "https://github.com/google/googletest.git"
    GIT_TAG ${googletest_VERSION}
    GIT_PROGRESS TRUE
)

################################################################################
# TODO : Documentation
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

################################################################################
# TODO : Documentation
FetchContent_MakeAvailable(googletest)
set(folder "${GVK_IDE_FOLDER}/external/gtest/")
set_target_properties(gtest PROPERTIES FOLDER "${folder}")
set_target_properties(gtest_main PROPERTIES FOLDER "${folder}")

################################################################################
# TODO : Documentation
include(GoogleTest)
