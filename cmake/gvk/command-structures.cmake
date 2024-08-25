
include_guard(GLOBAL)

gvk_enable_target(command-structures)

include(gvk/cppgen)
include(gvk/runtime)
include(gvk/structures)

add_subdirectory(gvk-command-structures)

################################################################################
# gvk-command-structures install
if(gvk-command-structures-install-artifacts)
    gvk_install_artifacts(TARGET gvk-command-structures)
endif()
if(gvk-command-structures-install-headers)
    gvk_install_headers(TARGET gvk-command-structures)
endif()
