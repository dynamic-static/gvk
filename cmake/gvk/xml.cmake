
include_guard(GLOBAL)

set(GVK_XML_ENABLED ON CACHE BOOL "" FORCE)

include(gvk/string)
include(external/tinyxml2)

add_subdirectory(gvk-xml)
