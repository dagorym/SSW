/**
 * @file FTacticalGameMockUITest.h
 * @brief Header file for FTacticalGame mock UI runtime tests
 */

#ifndef FTACTICALGAMEMOCKUITEST_H_
#define FTACTICALGAMEMOCKUITEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalGameMockUITest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
class FTacticalGameMockUITest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalGameMockUITest );
CPPUNIT_TEST( testDeterministicTwoShipCombatDeclaresAttackerWinnerViaModelState );
CPPUNIT_TEST( testInstalledUISeamRoutesDeclareWinnerStyleNotifyWinnerAfterCombatOverStateTransition );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

/**
 * @brief Verifies deterministic Two Ship Combat Declares Attacker Winner Via Model State.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
void testDeterministicTwoShipCombatDeclaresAttackerWinnerViaModelState();

/**
 * @brief Verifies the FTacticalGame::installUI()/getUI() seam correctly routes a
 *        declareWinner()-style notifyWinner() call through to the installed UI.
 *
 * TMFR-01 context: FBattleScreen::declareWinner() (the close-path entry point that
 * calls closeBattleScreen() after announcing the winner) fetches its ITacticalUI via
 * `m_tacticalGame->getUI()` rather than a separately tracked pointer, then calls
 * `tacticalUI->notifyWinner(attackerWins)`. This test does not construct FBattleScreen
 * (it is a wx-owning wxFrame outside this non-wx test runner's scope); instead it uses
 * a mock ITacticalUI installed via installUI() to verify the exact seam
 * FBattleScreen::declareWinner() depends on: `game.getUI()` resolves to the exact
 * installed mock, and invoking notifyWinner() through that seam (mirroring
 * declareWinner()'s own call) reaches the mock exactly once with the expected flag.
 * This intentionally does not drive fireAllWeapons() to force a live combat-over
 * outcome first, since the seam-routing behavior under test does not depend on how
 * combat was resolved, and the shared sibling test's fireAllWeapons()-then-assert
 * pattern was found to be sensitive to the process-wide RNG state left behind by
 * whichever tests ran earlier in the same process.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 03, 2026
 * @date Last Modified: Jul 03, 2026
 */
void testInstalledUISeamRoutesDeclareWinnerStyleNotifyWinnerAfterCombatOverStateTransition();
};

}

#endif
