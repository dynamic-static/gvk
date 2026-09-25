
cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
project(gvk_getting_started_00_triangle)

# NOTE : Proof-of-life app for the Gradle/CMake/GameActivity plumbing (see kaiju
#   session-notes, plan step 5).  Does not build or link gvk yet — that comes once this
#   is confirmed working end to end on device.

find_package(game-activity REQUIRED CONFIG)

# NOTE : game-activity_static bundles its own compiled android_native_app_glue
#   (android_main()/android_app event-pump convention) internally -- confirmed by a
#   "duplicate symbol" linker error from also compiling the NDK's own copy of
#   android_native_app_glue.c here.  Its header (<game-activity/native_app_glue/
#   android_native_app_glue.h>, included from main.cpp) comes from game-activity's own
#   Prefab-exported include directories via the target_link_libraries below -- nothing
#   extra needed here for it.
add_library(
    gvk_getting_started_00_triangle
    SHARED
    main.cpp
)

target_link_libraries(
    gvk_getting_started_00_triangle
    PUBLIC
    android
    log
    game-activity::game-activity_static
)

# NOTE : game-activity_static's JNI implementation of GameActivity.initializeNativeCode()
#   is otherwise stripped by the linker, since nothing in our own code calls it directly
#   -- only the JVM looks it up dynamically by name at runtime.  Without this, the app
#   crashes at launch with UnsatisfiedLinkError.
target_link_options(
    gvk_getting_started_00_triangle
    PRIVATE
    "-u" "Java_com_google_androidgamesdk_GameActivity_initializeNativeCode"
)
