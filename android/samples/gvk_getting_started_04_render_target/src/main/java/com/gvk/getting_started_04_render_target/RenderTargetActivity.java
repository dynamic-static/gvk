package com.gvk.getting_started_04_render_target;

import com.google.androidgamesdk.GameActivity;

// NOTE : This is intentionally minimal.  Android only launches Java/Kotlin Activity
//   classes, so this is the unavoidable glue GameActivity requires; everything else
//   happens on the native side, starting at android_main() in android-main.cpp.
public class RenderTargetActivity extends GameActivity {
    static {
        System.loadLibrary("gvk_getting_started_04_render_target");
    }
}
