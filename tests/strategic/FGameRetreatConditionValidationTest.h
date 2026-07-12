/**
 * @file FGameRetreatConditionValidationTest.h
 * @brief Declares behavioral coverage for FGame::init()'s Sathar retreat-condition
 *        validation/re-prompt loop.
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */

#ifndef FGameRetreatConditionValidationTest_H_
#define FGameRetreatConditionValidationTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FGame.h"
#include "strategic/IStrategicUI.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Verifies FGame::init() validates IStrategicUI::selectRetreatCondition()'s
 *        return value against the valid 1..5 range and re-prompts on out-of-range
 *        results (for example wxID_CANCEL from an X-close/cancel of the dialog)
 *        instead of silently storing an invalid value that would disable UPF
 *        victory for the rest of the game.
 *
 * Each test installs its own scripted mock IStrategicUI (via FGame::create(IStrategicUI*))
 * so the sequence of values returned by selectRetreatCondition() can be controlled
 * per test case.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
class FGameRetreatConditionValidationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FGameRetreatConditionValidationTest);
	CPPUNIT_TEST(testInitRepromptsUntilValidRetreatConditionIsReturned);
	CPPUNIT_TEST(testInitAcceptsValidRetreatConditionWithoutExtraPrompts);
	CPPUNIT_TEST(testInitStoredConditionDrivesCorrectVictoryDispatch);
	CPPUNIT_TEST_SUITE_END();

public:
	/**
	 * @brief No shared per-test fixture state; each test installs its own mock UI.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void setUp();
	/**
	 * @brief No shared per-test fixture state to tear down.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void tearDown();

	/**
	 * @brief A mock that returns out-of-range values (0, 6, and an X-close/cancel
	 *        sentinel) before finally returning a valid 1..5 value must cause
	 *        init() to keep re-invoking selectRetreatCondition() until the stored
	 *        condition is the valid one. Asserts both the observed dispatched
	 *        condition (via showRetreatConditions()) and that the call counter
	 *        incremented more than once, proving re-prompting occurred. This test
	 *        fails against the pre-fix unvalidated assignment, which would store
	 *        the first (invalid) value and dispatch the "no retreat condition
	 *        selected" error text instead.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testInitRepromptsUntilValidRetreatConditionIsReturned();

	/**
	 * @brief A mock that returns a valid 1..5 value on the very first call must
	 *        have that value stored unchanged with exactly one
	 *        selectRetreatCondition() invocation (no unnecessary re-prompting).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testInitAcceptsValidRetreatConditionWithoutExtraPrompts();

	/**
	 * @brief After init() stores a valid retreat condition returned by the mock,
	 *        showRetreatConditions()'s per-condition switch dispatch -- driven by
	 *        the same m_satharRetreat member that checkForVictory()'s victory
	 *        switch reads -- must reflect that real condition rather than falling
	 *        through to the invalid/default branch, confirming UPF victory is
	 *        never silently disabled by a bogus stored value. checkForVictory()
	 *        itself is private and not directly test-accessible without deep
	 *        combat-state setup, so this test uses the feasible, publicly
	 *        observable proxy for the same underlying dispatch.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testInitStoredConditionDrivesCorrectVictoryDispatch();
};

}

#endif /* FGameRetreatConditionValidationTest_H_ */
