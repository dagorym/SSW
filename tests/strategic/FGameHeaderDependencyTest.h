/**
 * @file FGameHeaderDependencyTest.h
 * @brief Declares dependency-boundary regression tests for strategic FGame headers and source.
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Mar 28, 2026
 */

#ifndef FGameHeaderDependencyTest_H_
#define FGameHeaderDependencyTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief Guards the strategic-module boundary by asserting FGame header/source dependency constraints.
 *
 * The fixture confirms wx render/input types and direct dialog includes do not leak into the
 * strategic surface and that strategic interactions remain routed through IStrategicUI seams.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Mar 28, 2026
 * @date Last Modified: Mar 28, 2026
 */
class FGameHeaderDependencyTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FGameHeaderDependencyTest);
	CPPUNIT_TEST(testHeaderDoesNotIncludeWxHeaders);
	CPPUNIT_TEST(testHeaderDoesNotExposeWxRenderOrMouseTypes);
	CPPUNIT_TEST(testHeaderUsesStrategicUIAndRemovesParentMember);
	CPPUNIT_TEST(testHeaderDoesNotDeclareStrategicDrawOrMouseHandlers);
	CPPUNIT_TEST(testSourceDoesNotUseDisplayPaintingHelpers);
	CPPUNIT_TEST(testSourceDoesNotImplementStrategicDrawOrMouseHandlers);
	CPPUNIT_TEST(testWXGameDisplayOwnsStrategicDrawEntryPoint);
	CPPUNIT_TEST(testSourceRemovesDirectStrategicDialogIncludes);
	CPPUNIT_TEST(testSourceRoutesStrategicInteractionsThroughInterface);
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * @brief Verifies FGame.h does not directly include wx headers.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testHeaderDoesNotIncludeWxHeaders();
	/**
	 * @brief Verifies FGame.h does not expose wx render or mouse event types.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testHeaderDoesNotExposeWxRenderOrMouseTypes();
	/**
	 * @brief Verifies FGame.h uses IStrategicUI and omits legacy wx parent member storage.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testHeaderUsesStrategicUIAndRemovesParentMember();
	/**
	 * @brief Verifies strategic draw and mouse handler declarations are absent from FGame.h.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testHeaderDoesNotDeclareStrategicDrawOrMouseHandlers();
	/**
	 * @brief Verifies FGame.cpp no longer depends on display painting helper classes.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testSourceDoesNotUseDisplayPaintingHelpers();
	/**
	 * @brief Verifies strategic draw/mouse handler implementations are absent from FGame.cpp.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testSourceDoesNotImplementStrategicDrawOrMouseHandlers();
	/**
	 * @brief Verifies WXGameDisplay owns the strategic draw entry point declarations/definitions.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testWXGameDisplayOwnsStrategicDrawEntryPoint();
	/**
	 * @brief Verifies direct strategic dialog includes were removed from FGame.cpp.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testSourceRemovesDirectStrategicDialogIncludes();
	/**
	 * @brief Verifies strategic UI interactions in FGame.cpp route through the interface seam.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Mar 28, 2026
	 * @date Last Modified: Mar 28, 2026
	 */
	void testSourceRoutesStrategicInteractionsThroughInterface();
};

}

#endif /* FGameHeaderDependencyTest_H_ */
