/**
 * @file WXIconCacheTest.h
 * @brief Declares the WXIconCache test fixture that verifies shared icon-path resolution behavior.
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Apr 17, 2026
 */

#ifndef WXIconCacheTest_H_
#define WXIconCacheTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief Verifies that WXIconCache loads images through the shared asset resolver and preserves cache-key behavior.
 *
 * This fixture covers the regression contract that icon loading resolves paths through
 * FGameConfig::resolveAssetPath(...) while still indexing cached images by the original
 * short filename used by callers.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Apr 17, 2026
 */
class WXIconCacheTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( WXIconCacheTest );
	CPPUNIT_TEST( testGetResolvesFilenameThroughSharedAssetResolver );
	CPPUNIT_TEST( testCacheKeyRemainsOriginalFilename );
	CPPUNIT_TEST_SUITE_END();

private:
	/**
	 * @brief Reads a source file into memory for source-contract assertions.
	 *
	 * @param path Repository-relative or absolute file path to read.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	std::string readFile(const std::string & path);

public:
	/**
	 * @brief Initializes wx image handlers and resets singleton state before each test.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void setUp();
	/**
	 * @brief Resets singleton state after each test case completes.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void tearDown();

	/**
	 * @brief Ensures WXIconCache resolves load paths through FGameConfig::resolveAssetPath(...).
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Apr 17, 2026
	 * @date Last Modified: Apr 17, 2026
	 */
	void testGetResolvesFilenameThroughSharedAssetResolver();
	/**
	 * @brief Verifies cache lookups remain keyed by the original short filename.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testCacheKeyRemainsOriginalFilename();
};

}

#endif
