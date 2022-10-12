
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

#include "gvk/xml/handle.hpp"
#include "tinyxml2-utilities.hpp"

#include <map>

namespace gvk {
namespace xml {

Handle::Handle(const tinyxml2::XMLElement& xmlElement)
{
    name = get_xml_text(xmlElement.FirstChildElement("name"));
    if (!name.empty()) {
        isDispatchable = get_xml_text(xmlElement.FirstChildElement("type")) == "VK_DEFINE_HANDLE";
        vkObjectType = get_xml_attribute(xmlElement, "objtypeenum");
        for (const auto& parent : string::split(get_xml_attribute(xmlElement, "parent"), ",")) {
            parents.insert(parent);
        }
    } else {
        name = get_xml_attribute(xmlElement, "name");
        alias = get_xml_attribute(xmlElement, "alias");
    }
}

} // namespace xml
} // namespace gvk
