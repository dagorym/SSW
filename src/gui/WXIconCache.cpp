/**
 * @file WXIconCache.cpp
 * @brief Implementation of WXIconCache singleton.
 * @author Implementation
 * @date Created: Mar 28, 2026
 */
#include "gui/WXIconCache.h"
#include "core/FGameConfig.h"
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
    FGameConfig & gc = FGameConfig::getGameConfig();
    std::string fullPath = gc.getBasePath() + filename;
    wxImage img(fullPath);
    if (!img.IsOk()) {
        std::string fallbackPath = gc.getBasePath() + "../" + filename;
        wxImage fallback(fallbackPath);
        if (fallback.IsOk()) {
            img = fallback;
        }
    }
    if (!img.IsOk()) {
        wxLogWarning("WXIconCache: failed to load image '%s'", fullPath.c_str());
        m_cache[filename] = wxImage();
    } else {
        m_cache[filename] = img;
    }
    return m_cache[filename];
}

} // namespace Frontier
