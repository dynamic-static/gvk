
/*******************************************************************************

MIT License

Copyright (c) Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#pragma once

// NOTE : gvk-system does not supply main()/android_main() itself -- the application does,
//  same shape as kaiju's own existing main.cpp/kaiju::game() split.  Application code
//  implements gvk_main(argc, ppArgv) with its portable entry logic.
//    - Desktop : gvk_main *is* main(), via the macro below.
//    - Android : the application also implements its own real
//      extern "C" void android_main(struct android_app*) (the actual native_app_glue
//      contract -- no argc/ppArgv exist on Android), which at minimum calls
//      gvk::system::set_android_app(pAndroidApp) so Surface::create() can find it, then
//      calls gvk_main(0, nullptr).
#ifdef __ANDROID__

struct android_app;

namespace gvk {
namespace system {

void set_android_app(android_app* pAndroidApp);

} // namespace system
} // namespace gvk

#else

#define gvk_main main

#endif // __ANDROID__
