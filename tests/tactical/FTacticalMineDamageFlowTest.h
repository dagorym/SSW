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
 * @date Last Modified: May 28, 2026
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
};

}

#endif
