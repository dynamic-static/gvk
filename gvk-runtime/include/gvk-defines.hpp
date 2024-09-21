
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

/**
NOTE : gvk_dl<open/sym/close> may be overriden to use a different system call (LoadLibraryEx(), for example)
NOTE : Best practices for loading dll/so libraries (ie. full paths instead of relative paths) should be exercised
*/

#ifdef __linux__
#ifndef GVK_PLATFORM_LINUX
#define GVK_PLATFORM_LINUX
#endif
#ifndef VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <dlfcn.h>
#ifndef gvk_dlopen
#define gvk_dlopen(LIBRARY_NAME) (void*)dlopen(LIBRARY_NAME, RTLD_LAZY | RTLD_LOCAL)
#endif
#ifndef gvk_dlsym
#define gvk_dlsym(LIBRARY_HANDLE, SYMBOL_NAME) (void*)dlsym(LIBRARY_HANDLE, SYMBOL_NAME)
#endif
#ifndef gvk_dlclose
#define gvk_dlclose(LIBRARY_HANDLE) dlclose(LIBRARY_HANDLE)
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifndef GVK_PLATFORM_WINDOWS
#define GVK_PLATFORM_WINDOWS
#endif
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#ifndef gvk_dlopen
#define gvk_dlopen(LIBRARY_NAME) (void*)LoadLibraryA(LIBRARY_NAME)
#endif
#ifndef gvk_dlsym
#define gvk_dlsym(LIBRARY_HANDLE, SYMBOL_NAME) (void*)GetProcAddress((HMODULE)LIBRARY_HANDLE, SYMBOL_NAME)
#endif
#ifndef gvk_dlclose
#define gvk_dlclose(LIBRARY_HANDLE) FreeLibrary((HMODULE)LIBRARY_HANDLE)
#endif
#endif

#ifndef VK_ENABLE_BETA_EXTENSIONS
#define VK_ENABLE_BETA_EXTENSIONS
#endif
#include "vulkan/vulkan.h"

#if defined(__clang__)
#define GVK_COMPILER_CLANG
#elif defined(__GNUC__)
#define GVK_COMPILER_GCC
#elif defined(_MSVC_LANG)
#define GVK_COMPILER_MSVC
#endif

#include <cassert>

#define gvk_stringify(STR) #STR
#define gvk_expand(STR) gvk_stringify(STR)
#define gvk_file_line (__FILE__ "(" gvk_expand(__LINE__) ")")

/**
@example
    VkResult example_function()
    {
        gvk_result_scope_begin(VK_ERROR_<...>) {
            gvk_result(vkFunctionCall0(...));
            gvk_result(vkFunctionCall1(...));
            gvk_result(vkFunctionCall2(...));
        } gvk_result_scope_end;
        if (gvkResult == VK_ERROR_<...>) {
            // ...recover...
        }
        return gvkResult;
    }
*/
#define gvk_result_scope_begin(GVK_RESULT) VkResult gvkResult = GVK_RESULT; (void)gvkResult; do {
#define gvk_result(GVK_CALL)                                                               \
gvkResult = (GVK_CALL);                                                                    \
if (gvkResult != VK_SUCCESS) {                                                             \
    if (!gvk::detail::process_result_scope_failure(gvkResult, gvk_file_line, #GVK_CALL)) { \
        assert(gvkResult == VK_SUCCESS && #GVK_CALL);                                      \
    }                                                                                      \
    break;                                                                                 \
}
#define gvk_result_scope_end } while(0);

namespace gvk {

/**
Callback for processing gvk_result_scope failures
@param [in] gvkResult The VkResult of the failed gvk_result_scope
@param [in] pFileLine A string with the file and line number where the error occured
@param [in[ pGvkCall A string with the expression that triggered the error
@return Whether or not to continue execution in Debug configurations
    @note In Debug configurations returning VK_FALSE will trigger an assert()
*/
typedef VkBool32 (*PFN_result_scope_callback)(VkResult gvkResult, const char* pFileLine, const char* pGvkCall);

/**
Global gvk_result_scope callback
*/
extern PFN_result_scope_callback gPfnGvkResultScopeCallback;

/**
thread_local gvk_result_scope callback
    @note thread_local gvk_result_scope callbacks take precedence over the global gvk_result_scope
    @note If a thread_local gvk_result_scope calback isn't set for a particular thread, the global callback will be used if set
*/
extern thread_local PFN_result_scope_callback tlPfnGvkResultScopeCallback;

/**
Gets a pointer to the given VkAllocationCallbacks if valid, otherwise nullptr
@param [in] allocator The VkAllocationCallbacks to validate
@return A pointer to the given VkAllocationCallbacks if valid, otherwise nullptr
*/
const VkAllocationCallbacks* validate_allocator(const VkAllocationCallbacks& allocator);

namespace detail {

VkBool32 process_result_scope_failure(VkResult gvkResult, const char* pFileLine, const char* pGvkCall);

} // namespace detail
} // namespace gvk
