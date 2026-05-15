/**
 * @file FGameMockStrategicUITest.h
 * @brief Declares the strategic mock-UI initialization regression fixture.
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Apr 3, 2026
 * @date Last Modified: Apr 3, 2026
 */

#ifndef FGameMockStrategicUITest_H_
#define FGameMockStrategicUITest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FGame.h"
#include "strategic/IStrategicUI.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Verifies FGame initialization and routing behavior through a mock IStrategicUI implementation.
 *
 * This fixture confirms strategic setup callbacks, redraw notifications, and message pathways
 * are routed through the interface seam without requiring wx-backed dialogs.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Apr 3, 2026
 * @date Last Modified: Apr 3, 2026
 */
class FGameMockStrategicUITest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FGameMockStrategicUITest);
	CPPUNIT_TEST(testInitWithMockStrategicUI);
	CPPUNIT_TEST_SUITE_END();

private:
	FGame *m_game;
	IStrategicUI *m_ui;

public:
	/**
	 * @brief Installs the mock strategic UI and creates a game instance bound to it.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Apr 3, 2026
	 * @date Last Modified: Apr 3, 2026
	 */
	void setUp();
	/**
	 * @brief Destroys the game instance and mock UI after each test.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Apr 3, 2026
	 * @date Last Modified: Apr 3, 2026
	 */
	void tearDown();

	/**
	 * @brief Validates initialization state and strategic callback sequencing with a mock UI.
	 *
	 * @author Tom Stephens, gpt-5.3-codex (medium)
	 * @date Created: Apr 3, 2026
	 * @date Last Modified: Apr 3, 2026
	 */
	void testInitWithMockStrategicUI();
};

}

#endif /* FGameMockStrategicUITest_H_ */
