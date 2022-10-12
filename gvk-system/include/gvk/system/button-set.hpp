
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

#include "gvk/system/updateable.hpp"

#include <bitset>

namespace gvk {
namespace sys {

template <size_t Count>
class ButtonSet
    : public Updateable<std::bitset<Count>>
{
public:
    template <typename ButtonIdType>
    inline bool up(ButtonIdType buttonId) const
    {
        return !this->current[(size_t)buttonId];
    }

    template <typename ButtonIdType>
    inline bool down(ButtonIdType buttonId) const
    {
        return this->current[(size_t)buttonId];
    }

    template <typename ButtonIdType>
    inline bool held(ButtonIdType buttonId) const
    {
        return this->previous[(size_t)buttonId] && this->current[(size_t)buttonId];
    }

    template <typename ButtonIdType>
    inline bool pressed(ButtonIdType buttonId) const
    {
        return !this->previous[(size_t)buttonId] && this->current[(size_t)buttonId];
    }

    template <typename ButtonIdType>
    inline bool released(ButtonIdType buttonId) const
    {
        return this->previous[(size_t)buttonId] && !this->current[(size_t)buttonId];
    }
};

} // namespace sys
} // namespace gvk
