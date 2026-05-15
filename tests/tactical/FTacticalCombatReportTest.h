/**
 * @file FTacticalCombatReportTest.h
 * @brief Header file for the FTacticalCombatReportTest class
 */

#ifndef FTacticalCombatReportTest_H_
#define FTacticalCombatReportTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "tactical/FBattleScreen.h"
#include "tactical/FTacticalCombatReport.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief CppUnit fixture for FTacticalCombatReportTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Apr 08, 2026
 */
class FTacticalCombatReportTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalCombatReportTest );
	CPPUNIT_TEST( testReportModelPreservesRawAttackAndImmediateEventData );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryCapturesImmediateElectricalFireAndMineDamageEvents );
	CPPUNIT_TEST( testReportModelDefinesSeparateShipSummaryRollupTypes );
	CPPUNIT_TEST( testTacticalCombatReportSummaryHitDetailToggleDefaultsEnabled );
	CPPUNIT_TEST( testReportModelSupportsAllPlannedReportingModes );
	CPPUNIT_TEST( testBattleScreenExposesTacticalReportLifecycleApi );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryBuildsHitDetailsForHitAttacksOnly );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryHitDetailsCapturePlayerReadableOutcome );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryAggregatesMultipleAttacksPerShip );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryWeaponFireAttackShapeDoesNotDoubleCountTargetHullDamage );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryMineDamageAttackShapeDoesNotDoubleCountTargetHullDamage );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryAttackShapeRetainsNonHullInternalEffects );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryDoesNotDoubleCountNestedHullDamageForAttackTarget );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryCountsStandaloneReportLevelHullDamageEvents );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryCountsStandaloneHullDamageWhileSuppressingMatchingNestedAttackEvent );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryRetainsNonHullNestedEffectsWhenSuppressingDuplicateHullDamage );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummarySummarizesHullDamageAndEffects );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryOmitsUndamagedShips );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryUsesStoredShipReferences );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryCountsNestedHullDamageForOtherShips );
	CPPUNIT_TEST( testBuildTacticalCombatReportSummaryCountsNestedHullDamageWhenOwnerDiffersFromAttackTarget );
	CPPUNIT_TEST( testBattleScreenDelegatesSummaryGenerationToModelBuilder );
	CPPUNIT_TEST( testTacticalCombatReportSummaryDocumentsCanonicalAttackHullDamageRule );
	CPPUNIT_TEST( testBattleScreenNormalizesNestedAttackEventsOntoStoredAttackIndex );
	CPPUNIT_TEST( testBattleScreenStoresStandaloneImmediateEventsWithUnattachedAttackIndex );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);
	std::string extractFunctionBody(const std::string & source, const std::string & signature);

public:
	void setUp();
	void tearDown();

	/**
	 * @brief Verifies report Model Preserves Raw Attack And Immediate Event Data.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testReportModelPreservesRawAttackAndImmediateEventData();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Captures Immediate Electrical Fire And Mine Damage Events.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testBuildTacticalCombatReportSummaryCapturesImmediateElectricalFireAndMineDamageEvents();
	/**
	 * @brief Verifies report Model Defines Separate Ship Summary Rollup Types.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testReportModelDefinesSeparateShipSummaryRollupTypes();
	/**
	 * @brief Verifies tactical Combat Report Summary Hit Detail Toggle Defaults Enabled.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 08, 2026
	 * @date Last Modified: Apr 08, 2026
	 */
	void testTacticalCombatReportSummaryHitDetailToggleDefaultsEnabled();
	/**
	 * @brief Verifies report Model Supports All Planned Reporting Modes.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testReportModelSupportsAllPlannedReportingModes();
	/**
	 * @brief Verifies battle Screen Exposes Tactical Report Lifecycle Api.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testBattleScreenExposesTacticalReportLifecycleApi();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Builds Hit Details For Hit Attacks Only.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 08, 2026
	 * @date Last Modified: Apr 08, 2026
	 */
	void testBuildTacticalCombatReportSummaryBuildsHitDetailsForHitAttacksOnly();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Hit Details Capture Player Readable Outcome.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 08, 2026
	 * @date Last Modified: Apr 08, 2026
	 */
	void testBuildTacticalCombatReportSummaryHitDetailsCapturePlayerReadableOutcome();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Aggregates Multiple Attacks Per Ship.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 21, 2026
	 * @date Last Modified: Mar 21, 2026
	 */
	void testBuildTacticalCombatReportSummaryAggregatesMultipleAttacksPerShip();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Weapon Fire Attack Shape Does Not Double Count Target Hull Damage.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 25, 2026
	 * @date Last Modified: Mar 25, 2026
	 */
	void testBuildTacticalCombatReportSummaryWeaponFireAttackShapeDoesNotDoubleCountTargetHullDamage();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Mine Damage Attack Shape Does Not Double Count Target Hull Damage.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 25, 2026
	 * @date Last Modified: Mar 25, 2026
	 */
	void testBuildTacticalCombatReportSummaryMineDamageAttackShapeDoesNotDoubleCountTargetHullDamage();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Attack Shape Retains Non Hull Internal Effects.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 25, 2026
	 * @date Last Modified: Mar 25, 2026
	 */
	void testBuildTacticalCombatReportSummaryAttackShapeRetainsNonHullInternalEffects();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Does Not Double Count Nested Hull Damage For Attack Target.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testBuildTacticalCombatReportSummaryDoesNotDoubleCountNestedHullDamageForAttackTarget();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Counts Standalone Report Level Hull Damage Events.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testBuildTacticalCombatReportSummaryCountsStandaloneReportLevelHullDamageEvents();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Counts Standalone Hull Damage While Suppressing Matching Nested Attack Event.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 25, 2026
	 * @date Last Modified: Mar 25, 2026
	 */
	void testBuildTacticalCombatReportSummaryCountsStandaloneHullDamageWhileSuppressingMatchingNestedAttackEvent();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Retains Non Hull Nested Effects When Suppressing Duplicate Hull Damage.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testBuildTacticalCombatReportSummaryRetainsNonHullNestedEffectsWhenSuppressingDuplicateHullDamage();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Summarizes Hull Damage And Effects.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 21, 2026
	 * @date Last Modified: Mar 21, 2026
	 */
	void testBuildTacticalCombatReportSummarySummarizesHullDamageAndEffects();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Omits Undamaged Ships.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 21, 2026
	 * @date Last Modified: Mar 21, 2026
	 */
	void testBuildTacticalCombatReportSummaryOmitsUndamagedShips();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Uses Stored Ship References.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 21, 2026
	 * @date Last Modified: Mar 21, 2026
	 */
	void testBuildTacticalCombatReportSummaryUsesStoredShipReferences();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Counts Nested Hull Damage For Other Ships.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testBuildTacticalCombatReportSummaryCountsNestedHullDamageForOtherShips();
	/**
	 * @brief Verifies build Tactical Combat Report Summary Counts Nested Hull Damage When Owner Differs From Attack Target.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 25, 2026
	 * @date Last Modified: Mar 25, 2026
	 */
	void testBuildTacticalCombatReportSummaryCountsNestedHullDamageWhenOwnerDiffersFromAttackTarget();
	/**
	 * @brief Verifies battle Screen Delegates Summary Generation To Model Builder.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 21, 2026
	 * @date Last Modified: Mar 21, 2026
	 */
	void testBattleScreenDelegatesSummaryGenerationToModelBuilder();
	/**
	 * @brief Verifies tactical Combat Report Summary Documents Canonical Attack Hull Damage Rule.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testTacticalCombatReportSummaryDocumentsCanonicalAttackHullDamageRule();
	/**
	 * @brief Verifies battle Screen Normalizes Nested Attack Events Onto Stored Attack Index.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testBattleScreenNormalizesNestedAttackEventsOntoStoredAttackIndex();
	/**
	 * @brief Verifies battle Screen Stores Standalone Immediate Events With Unattached Attack Index.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testBattleScreenStoresStandaloneImmediateEventsWithUnattachedAttackIndex();
};

}

#endif
