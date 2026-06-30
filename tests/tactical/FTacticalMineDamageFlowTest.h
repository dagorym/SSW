/**
 * @file FTacticalMineDamageFlowTest.h
 * @brief Header file for the FTacticalMineDamageFlowTest class
 */

#ifndef FTacticalMineDamageFlowTest_H_
#define FTacticalMineDamageFlowTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalMineDamageFlowTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium), gpt-5.4 (high), claude-sonnet-4-6 (medium)
 * @date Created: Mar 22, 2026
 * @date Last Modified: Jun 29, 2026
 */
class FTacticalMineDamageFlowTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalMineDamageFlowTest );
	CPPUNIT_TEST( testApplyMineDamageDefersDestroyedShipCleanupUntilAfterImmediateSummaryDialog );
	CPPUNIT_TEST( testFinalizeMoveAppliesMineDamageExactlyOncePerMovementResolution );
	CPPUNIT_TEST( testFinalizeMovePhaseStateDelegatesToCanonicalCompletionPath );
	CPPUNIT_TEST( testMineDamageAttackReportBuilderLeavesNestedEventsUnattachedUntilBattleScreenStoresThem );
	CPPUNIT_TEST( testRuntimeMoveDonePathUsesCanonicalCompletionAndResolvesOffBoardOutcome );
	CPPUNIT_TEST( testSeekerDetonationDamageResolutionUsesSMWeaponsICMAndImmediateReporting );
	CPPUNIT_TEST( testSeekerActivationPhaseResolvesPendingDamageWhenModelHasUI );
	CPPUNIT_TEST( testShipPathSeekerContactCheckedInCompleteMovePhase );
	CPPUNIT_TEST( testInactiveSeekerNotTriggeredByPathContact );
	CPPUNIT_TEST( testApplyMovementSeekerDamageDetonatesSeekersExactlyOnce );
	CPPUNIT_TEST( testSeekerDamageAppliedBeforeMineDamageInCompleteMovePhase );
	CPPUNIT_TEST( testTriggeredMineHexesInitiallyEmptyOnFreshGame );
	CPPUNIT_TEST( testTriggeredMineHexesEmptyAfterCompleteMovePhaseWithNoMines );
	CPPUNIT_TEST( testApplyMineDamageSummaryCalledUnconditionallyWhenMinesFire );
	CPPUNIT_TEST( testTriggeredMineHexesClearedAfterSummaryDialog );
	CPPUNIT_TEST( testLastTriggeredMineHexesDelegationInFBattleScreenHeader );
	CPPUNIT_TEST( testMineEncounterHighlightHexesNonEmptyDuringShowDamageSummaryCallback );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);
	std::string extractFunctionBody(const std::string & source, const std::string & signature);
	unsigned int countOccurrences(const std::string & haystack, const std::string & needle);

public:
	void setUp();
	void tearDown();

	/**
	 * @brief Verifies apply Mine Damage Defers Destroyed Ship Cleanup Until After Immediate Summary Dialog.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testApplyMineDamageDefersDestroyedShipCleanupUntilAfterImmediateSummaryDialog();
	/**
	 * @brief Verifies finalize Move Applies Mine Damage Exactly Once Per Movement Resolution.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testFinalizeMoveAppliesMineDamageExactlyOncePerMovementResolution();
	/**
	 * @brief Verifies finalize Move Phase State Delegates To Canonical Completion Path.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 02, 2026
	 * @date Last Modified: Apr 02, 2026
	 */
	void testFinalizeMovePhaseStateDelegatesToCanonicalCompletionPath();
	/**
	 * @brief Verifies mine Damage Attack Report Builder Leaves Nested Events Unattached Until Battle Screen Stores Them.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testMineDamageAttackReportBuilderLeavesNestedEventsUnattachedUntilBattleScreenStoresThem();
	/**
	 * @brief Verifies runtime Move Done Path Uses Canonical Completion And Resolves Off Board Outcome.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 02, 2026
	 * @date Last Modified: Apr 02, 2026
	 */
	void testRuntimeMoveDonePathUsesCanonicalCompletionAndResolvesOffBoardOutcome();
	/**
	 * @brief Verifies seeker Detonation Damage Resolution Uses Sm Weapons Icm And Immediate Reporting.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void testSeekerDetonationDamageResolutionUsesSMWeaponsICMAndImmediateReporting();
	/**
	 * @brief Verifies seeker Activation Phase Resolves Pending Damage When Model Has Ui.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.4 (high)
	 * @date Created: May 27, 2026
	 * @date Last Modified: May 27, 2026
	 */
	void testSeekerActivationPhaseResolvesPendingDamageWhenModelHasUI();
	/**
	 * @brief Verifies completeMovePhase calls checkForActiveSeekersOnPath per ship alongside mine check.
	 *
	 * Source-inspects completeMovePhase to confirm that checkForActiveSeekersOnPath is called
	 * for each moving ship and that the outcomes are cleared at the start of completion.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: May 28, 2026
	 * @date Last Modified: May 28, 2026
	 */
	void testShipPathSeekerContactCheckedInCompleteMovePhase();
	/**
	 * @brief Verifies checkForActiveSeekersOnPath skips inactive seekers entirely.
	 *
	 * Source-inspects checkForActiveSeekersOnPath to confirm inactive seekers are
	 * never considered for movement path contact.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: May 28, 2026
	 * @date Last Modified: May 28, 2026
	 */
	void testInactiveSeekerNotTriggeredByPathContact();
	/**
	 * @brief Verifies applyMovementSeekerDamage removes detonated seekers exactly once from the model.
	 *
	 * Source-inspects applyMovementSeekerDamage to confirm it collects IDs before resolution,
	 * resolves through the shared seam when UI is installed, and removes each seeker exactly once.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: May 28, 2026
	 * @date Last Modified: May 28, 2026
	 */
	void testApplyMovementSeekerDamageDetonatesSeekersExactlyOnce();
	/**
	 * @brief Verifies seeker damage is applied before mine damage in completeMovePhase.
	 *
	 * Source-inspects completeMovePhase to confirm applyMovementSeekerDamage appears
	 * before applyMineDamage in the resolution ordering.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: May 28, 2026
	 * @date Last Modified: May 28, 2026
	 */
	void testSeekerDamageAppliedBeforeMineDamageInCompleteMovePhase();

	/**
	 * @brief Behavioral: getLastTriggeredMineHexes() returns empty set on a fresh game instance.
	 *
	 * Constructs a real FTacticalGame, calls getLastTriggeredMineHexes(), and asserts empty.
	 * Verifies the accessor is present and correctly initialized.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testTriggeredMineHexesInitiallyEmptyOnFreshGame();

	/**
	 * @brief Behavioral: getLastTriggeredMineHexes() is empty after completeMovePhase() with no mines.
	 *
	 * Runs the full move-complete path through a real FTacticalGame with no mined hexes
	 * and asserts the triggered-hex set remains empty. Verifies the SMFR-03 state is
	 * orthogonal to mine-free movement.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testTriggeredMineHexesEmptyAfterCompleteMovePhaseWithNoMines();

	/**
	 * @brief Source-contract: applyMineDamage() calls showDamageSummary unconditionally when mines fire.
	 *
	 * Source-inspects applyMineDamage to confirm showDamageSummary is called without a guard
	 * on summary.ships.size(), so zero-damage encounters still show the dialog.
	 * Supplements the behavioral state tests in this fixture.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testApplyMineDamageSummaryCalledUnconditionallyWhenMinesFire();

	/**
	 * @brief Source-contract: triggered mine hexes are set before requestRedraw and cleared after showDamageSummary.
	 *
	 * Source-inspects applyMineDamage to confirm m_lastTriggeredMineHexes is populated before the
	 * requestRedraw call and cleared only after showDamageSummary returns, enforcing the
	 * SMFR-03 highlight lifecycle ordering.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testTriggeredMineHexesClearedAfterSummaryDialog();

	/**
	 * @brief Source-contract: FBattleScreen.h declares getLastTriggeredMineHexes delegation.
	 *
	 * Source-inspects FBattleScreen.h and FBattleScreen.cpp to confirm getLastTriggeredMineHexes
	 * is declared and delegates to FTacticalGame, per the SMFR-03 board-highlight flow.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testLastTriggeredMineHexesDelegationInFBattleScreenHeader();

	/**
	 * @brief Behavioral: triggered mine hexes are non-empty inside showDamageSummary callback and empty after.
	 *
	 * Constructs a real FTacticalGame with an FMinelayer (defender) and an attacker ship.
	 * Places a mine at the attacker's starting hex via placeMineFromSelection, installs a
	 * capturing mock ITacticalUI, then calls completeMovePhase() to trigger the mine encounter.
	 * Asserts (AC2) getLastTriggeredMineHexes() is NON-EMPTY inside the showDamageSummary callback,
	 * (AC3) getLastTriggeredMineHexes() is EMPTY after completeMovePhase() returns, and
	 * (AC1) showDamageSummary was invoked at least once for the mine encounter.
	 *
	 * @author claude-sonnet-4-6 (medium)
	 * @date Created: Jun 19, 2026
	 * @date Last Modified: Jun 19, 2026
	 */
	void testMineEncounterHighlightHexesNonEmptyDuringShowDamageSummaryCallback();
};

}

#endif
