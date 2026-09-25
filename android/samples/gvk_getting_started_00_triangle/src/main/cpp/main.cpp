// NOTE : Proof-of-life only.  No gvk/Vulkan code yet — this exists to prove the
//   Gradle/CMake/GameActivity plumbing works end to end before building the real
//   gvk::system Android backend on top of it (see kaiju session-notes, plan step 5).

#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/log.h>

#define LOG_TAG "gvk_getting_started_00_triangle"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static void on_app_cmd(struct android_app* app, int32_t cmd)
{
    (void)app;
    LOGI("on_app_cmd(%d)", cmd);
}

extern "C" void android_main(struct android_app* app)
{
    LOGI("android_main() : gvk-getting-started-00-triangle launched");
    app->onAppCmd = on_app_cmd;
    while (!app->destroyRequested) {
        int events = 0;
        android_poll_source* pSource = nullptr;
        // NOTE : Blocking (-1) is correct while there's nothing to render every frame;
        //   once real rendering exists this becomes a non-blocking (0) poll instead.
        while (ALooper_pollOnce(-1, nullptr, &events, (void**)&pSource) >= 0) {
            if (pSource) {
                pSource->process(app, pSource);
            }
            if (app->destroyRequested) {
                break;
            }
        }
    }
    LOGI("android_main() : returning, destroyRequested");
}
