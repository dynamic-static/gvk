// NOTE : The only Android-specific file in this app -- gvk-getting-started-02-uniform-buffer.cpp
//   itself is the same shared source gvk's desktop build uses (gvk/samples/), unmodified
//   for Android beyond gvk_sample_utilities.hpp's already-platform-guarded surface
//   creation.  gvk-system doesn't supply this glue itself (see gvk-system/entry-point.hpp);
//   every Android app/sample provides its own, same as it already provides its own
//   desktop main() (gvk_main(), which *is* main() on desktop via that header's macro).

#include <gvk-system.hpp>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/log.h>

#include <cstdio>
#include <thread>
#include <unistd.h>

#define LOG_TAG "gvk_getting_started_02_uniform_buffer"

// NOTE : Defined in gvk-getting-started-02-uniform-buffer.cpp, a separate translation
//   unit -- unlike main() it's not an entry point the toolchain knows about, so it needs a
//   real forward declaration here.
int gvk_main(int argc, const char* ppArgv[]);

// NOTE : gvk_main() and the code it calls (gvk-sample-utilities.hpp's process_gvk_error(),
//  glslang, etc.) report errors via std::cerr/std::cout -- correct and portable, but not
//  routed to logcat for a native Android app, so a failure otherwise looks exactly like a
//  silent, immediate exit (see gvk_main()'s own return code logged below, discovered this
//  way).  Standard Android native-app pattern: redirect the process' stdout/stderr through
//  a pipe into a background thread that forwards each line to logcat.
static void redirect_stdio_to_logcat()
{
    static int sPipe[2];
    pipe(sPipe);
    dup2(sPipe[1], STDOUT_FILENO);
    dup2(sPipe[1], STDERR_FILENO);
    std::thread(
        [] {
            char line[1024];
            FILE* pFile = fdopen(sPipe[0], "r");
            while (pFile && fgets(line, sizeof(line), pFile)) {
                __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, line);
            }
        }
    ).detach();
}

extern "C" void android_main(struct android_app* pAndroidApp)
{
    redirect_stdio_to_logcat();
    gvk::system::set_android_app(pAndroidApp);
    auto result = gvk_main(0, nullptr);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "gvk_main() returned %d", result);
}
