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
 * @author gpt-5.3-codex (medium), gpt-5.4 (high)
 * @date Created: Mar 22, 2026
 * @date Last Modified: May 27, 2026
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
};

}

#endif
