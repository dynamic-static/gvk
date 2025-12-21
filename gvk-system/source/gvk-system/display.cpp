
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

#include "gvk-system/display.hpp"
#include "gvk-system/surface.hpp"
#include "gvk-reference.hpp"

namespace gvk {
namespace system {

int Display::get(const Surface& surface, Display* pDisplay)
{
    (void)surface;
    (void)pDisplay;
    assert(pDisplay);
    return 0;
}

int Display::get(const Surface& surface, uint32_t pDisplalyCount, Display* pDisplays)
{
    (void)surface;
    (void)pDisplalyCount;
    (void)pDisplays;
    assert(pDisplays);
    return 0;
}

#if 0
private:
    class ControlBlock final
    {
    public:
        ControlBlock();
        ~ControlBlock();
    private:
        ControlBlock(const ControlBlock&) = delete;
        ControlBlock& operator=(const ControlBlock&) = delete;
};

#if 0
    const std::string& get_title() const;
    void set_title(const std::string& title);
    const CursorMode& get_cursor_mode() const;
    void set_cursor_mode(CursorMode cursorMode);
    void set_cursor_type(CursorType cursorType);
    const PlatformInfo& get_platform_info() const;
#endif
    gvk_reference_type(Display)
};
#endif

} // namespace system
} // namespace gvk
