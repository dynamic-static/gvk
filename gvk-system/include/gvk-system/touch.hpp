
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

#include "gvk-system/updateable.hpp"

#include <array>
#include <cstdint>

namespace gvk {
namespace system {

// NOTE : Deliberately minimal for now (current position/delta/down-pressed-released
//  per pointer, no historical sub-frame samples, no pressure/size/tool-type) -- the
//  richer per-pointer data GameActivity provides (GameActivityPointerAxes) is there
//  when buffered gesture/combo recognition actually gets designed, not needed for
//  camera control or ImGui interaction.  See kaiju session notes.
class Touch final
{
public:
    // NOTE : Matches GameActivity's own GAMEACTIVITY_MAX_NUM_POINTERS_IN_MOTION_EVENT
    //  default (GameActivityEvents.h) -- the real cap on simultaneous pointers a
    //  GameActivityMotionEvent can ever report.
    static constexpr size_t MaxPointerCount = 8;

    class Pointer final
    {
    public:
        inline bool up() const { return !active.current; }
        inline bool down() const { return active.current; }
        inline bool pressed() const { return !active.previous && active.current; }
        inline bool released() const { return active.previous && !active.current; }

        inline void update()
        {
            active.update();
            position.update();
        }

        // NOTE : Android's own per-touch id, stable from pressed() to released() --
        //  -1 when this slot isn't tracking an active pointer.  Lets a caller tell
        //  "the same finger moved" apart from "a different finger took this slot,"
        //  which matters once more than one pointer is down at a time.  Not itself
        //  Updateable -- backends set it directly when a slot starts tracking a new
        //  pointer, same as gvk::system::Surface::PlatformInfo's plain fields.
        int32_t id{ -1 };

        class : public Updateable<bool> { } active{ };

        class : public Updateable<std::array<float, 2>>
        {
        public:
            inline std::array<float, 2> delta() const
            {
                return { current[0] - previous[0], current[1] - previous[1] };
            }
        } position{ };
    };

    inline void update()
    {
        for (auto& pointer : pointers) {
            pointer.update();
        }
    }

    // NOTE : Index has no meaning beyond "a slot" -- find a pointer by its id (or an
    //  inactive slot to claim) by scanning, same as a backend's own translation code
    //  has to.  MaxPointerCount is small enough (8) that this is never worth indexing.
    std::array<Pointer, MaxPointerCount> pointers{ };
};

} // namespace system
} // namespace gvk
