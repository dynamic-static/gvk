
include_guard(GLOBAL)

set(gvk-command-structures-enabled           ON CACHE BOOL "" FORCE)
set(gvk-install-command-structures-artifacts ON CACHE BOOL "")
set(gvk-install-command-structures-headers   ON CACHE BOOL "")

include(gvk/cppgen)
include(gvk/runtime)
include(gvk/structures)

add_subdirectory(gvk-command-structures)

################################################################################
# gvk-command-structures install
if(gvk-install-command-structures-artifacts)
    gvk_install_artifacts(TARGET gvk-command-structures)
endif()
if(gvk-install-command-structures-headers)
    gvk_install_headers(TARGET gvk-command-structures)
endif()
