/**
 * @file WXIconCache.cpp
 * @brief Implementation of WXIconCache singleton.
 * @author Implementation
 * @date Created: Mar 28, 2026
 */
#include "gui/WXIconCache.h"
#include <wx/log.h>

namespace Frontier {

WXIconCache & WXIconCache::instance() {
    static WXIconCache s_instance;
    return s_instance;
}

const wxImage & WXIconCache::get(const std::string & filename) {
    auto it = m_cache.find(filename);
    if (it != m_cache.end()) {
        return it->second;
    }
    wxImage img(filename);
    if (!img.IsOk()) {
        wxLogWarning("WXIconCache: failed to load image '%s'", filename.c_str());
        m_cache[filename] = wxImage();
    } else {
        m_cache[filename] = img;
    }
    return m_cache[filename];
}

} // namespace Frontier
