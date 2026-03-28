/**
 * @file WXIconCache.h
 * @brief Singleton cache for wxImage icon loading.
 * @author Implementation
 * @date Created: Mar 28, 2026
 */
#ifndef _WXICONCACHE_H_
#define _WXICONCACHE_H_

#include <wx/wx.h>
#include <map>
#include <string>

namespace Frontier {

/**
 * @brief Singleton cache for vehicle and fleet icon images.
 *
 * WXIconCache is the only place in the codebase where wxImage objects
 * are loaded from disk for ship and fleet icons. A second call for the
 * same filename returns the cached image without a second disk read.
 */
class WXIconCache {
public:
    /// Return the singleton instance.
    static WXIconCache & instance();

    /**
     * @brief Retrieve (and cache) the image for the given filename.
     *
     * If the image has not been loaded yet, loads it from disk.
     * If loading fails, logs a warning and returns a default-constructed
     * (empty) wxImage rather than crashing.
     *
     * @param filename Path to the image file.
     * @return const reference to the cached wxImage.
     */
    const wxImage & get(const std::string & filename);

private:
    WXIconCache() {}
    WXIconCache(const WXIconCache &) = delete;
    WXIconCache & operator=(const WXIconCache &) = delete;

    std::map<std::string, wxImage> m_cache;
};

} // namespace Frontier

#endif // _WXICONCACHE_H_
