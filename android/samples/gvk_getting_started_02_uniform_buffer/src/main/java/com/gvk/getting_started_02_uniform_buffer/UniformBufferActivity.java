package com.gvk.getting_started_02_uniform_buffer;

import com.google.androidgamesdk.GameActivity;

// NOTE : This is intentionally minimal.  Android only launches Java/Kotlin Activity
//   classes, so this is the unavoidable glue GameActivity requires; everything else
//   happens on the native side, starting at android_main() in android-main.cpp.
public class UniformBufferActivity extends GameActivity {
    static {
        System.loadLibrary("gvk_getting_started_02_uniform_buffer");
    }
}
