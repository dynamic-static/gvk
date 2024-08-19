
include_guard(GLOBAL)

set(GVK_REFERENCE_ENABLED ON CACHE BOOL "" FORCE)

# TODO : If tests
include(external/asio)

add_subdirectory(gvk-reference)
