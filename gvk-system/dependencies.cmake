
include_guard(GLOBAL)
gvk_enable_module(gvk-reference)
gvk_enable_module(gvk-string)
if(NOT ANDROID)
    # NOTE : GLFW has no Android backend.  Its own CMakeLists.txt configures eagerly
    #   (looking for X11/Wayland) regardless of whether anything links it, so it has to
    #   stay disabled outright for Android, not just unlinked -- confirmed by a real
    #   "Failed to find wayland-scanner" configure failure under the NDK toolchain.
    gvk_enable_external_module(glfw)
endif()
