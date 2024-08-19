
include_guard(GLOBAL)

################################################################################
# TODO : Documentation
set(gvk-asio-enabled           ON CACHE BOOL "" FORCE)
set(gvk-install-asio-artifacts ON CACHE BOOL "")
set(gvk-install-asio-headers   ON CACHE BOOL "")

################################################################################
# TODO : Documentation
find_package(Threads REQUIRED)

################################################################################
# TODO : Documentation
set(asio_VERSION 12e0ce9e0500bf0f247dbd1ae894272656456079) # 1.30.2
FetchContent_Declare(
    asio
    GIT_REPOSITORY "https://github.com/chriskohlhoff/asio.git"
    GIT_TAG ${asio_VERSION}
    GIT_PROGRESS TRUE
)

################################################################################
# TODO : Documentation
FetchContent_MakeAvailable(asio)
FetchContent_GetProperties(asio SOURCE_DIR asio_SOURCE_DIR)
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE "$<BUILD_INTERFACE:${asio_SOURCE_DIR}/asio/include/>" $<INSTALL_INTERFACE:include>)

################################################################################
# TODO : Documentation
if(gvk-install-asio-artifacts)
    gvk_install_artifacts(TARGET asio VERSION ${asio_VERSION})
endif()
