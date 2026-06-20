/**
 * @file FTacticalSeekerMovementTest.h
 * @brief Header file for seeker movement model-side regression tests
 */

#ifndef _FTACTICALSEEKERMOVEMENTTEST_H_
#define _FTACTICALSEEKERMOVEMENTTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for model-side seeker targeting and movement helpers.
 *
 * Verifies source-level tactical seeker helper contracts remain deterministic,
 * non-wx, and aligned with planner acceptance criteria.
 *
 * @author gpt-5.4 (high), claude-sonnet-4-6 (medium), claude-sonnet-4-6 (standard), claude-sonnet-4-6 (medium)
 * @date Created: May 25, 2026
 * @date Last Modified: Jun 19, 2026
 */
class FTacticalSeekerMovementTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalSeekerMovementTest );
CPPUNIT_TEST( testSeekerTargetSelectionFiltersAndTieBreaksAcrossSides );
CPPUNIT_TEST( testSeekerHeadingAndMovementGreedyTurnLimits );
CPPUNIT_TEST( testSeekerMovementAllowanceProgression );
CPPUNIT_TEST( testDeterministicTargetSelectionHelperFiltersAndReturnsTies );
CPPUNIT_TEST( testSameHexContactTargetSelectionPrioritizesMaxHPAtRuntime );
CPPUNIT_TEST( testDeterministicGreedyNextStepHelperReturnsOneStepMove );
CPPUNIT_TEST( testActiveSeekerResolutionHandlesPremoveContactAndEnemySkipRuntime );
CPPUNIT_TEST( testCompleteSeekerActivationResolvesMovementContactAndSurvivorProgressionRuntime );
CPPUNIT_TEST( testSeekerHelpersRemainModelOnlyAndFHexMapGeneric );
CPPUNIT_TEST( testInactiveSeekerIgnoredByPathContactCheck );
CPPUNIT_TEST( testSeekerRemovedFromModelAfterMovementContact );
CPPUNIT_TEST( testMovementPathPopulatedByResolveActiveSeekers );
CPPUNIT_TEST( testSeekerMoveCountLabelFieldsReflectPathAndAllowance );
CPPUNIT_TEST( testClearNonImpactingSeekerMovementPathsPreservesBookkeeping );
CPPUNIT_TEST( testNonImpactingSeekerPathClearedAfterDamageApplied );
CPPUNIT_TEST( testCompleteMovePhaseCallsNonImpactingClearAfterDamageSourceContract );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

/**
 * @brief Verifies closest-target seeker selection filters and random tie-breaking.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 27, 2026
 */
void testSeekerTargetSelectionFiltersAndTieBreaksAcrossSides();

/**
 * @brief Verifies heading clamp and per-hex greedy turn behavior.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 27, 2026
 */
void testSeekerHeadingAndMovementGreedyTurnLimits();

/**
 * @brief Verifies seeker movement allowance progression and cap behavior.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 27, 2026
 */
void testSeekerMovementAllowanceProgression();

/**
 * @brief Verifies deterministic target helper ignores invalid entries and preserves ties.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 27, 2026
 * @date Last Modified: May 27, 2026
 */
void testDeterministicTargetSelectionHelperFiltersAndReturnsTies();

/**
 * @brief Verifies same-hex seeker contact selection excludes lower-max-HP ships at runtime.
 *
 * @author gpt-5.4 (high), GPT-5
 * @date Created: May 27, 2026
 * @date Last Modified: May 27, 2026
 */
void testSameHexContactTargetSelectionPrioritizesMaxHPAtRuntime();

/**
 * @brief Verifies deterministic greedy helper returns a legal one-step move.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 27, 2026
 * @date Last Modified: May 27, 2026
 */
void testDeterministicGreedyNextStepHelperReturnsOneStepMove();

/**
 * @brief Verifies runtime pre-movement contact outcomes and enemy-seeker skip behavior.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 27, 2026
 * @date Last Modified: May 27, 2026
 */
void testActiveSeekerResolutionHandlesPremoveContactAndEnemySkipRuntime();

/**
 * @brief Verifies seeker-activation completion resolves movement contact and survivor progression.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 27, 2026
 * @date Last Modified: May 27, 2026
 */
void testCompleteSeekerActivationResolvesMovementContactAndSurvivorProgressionRuntime();

/**
 * @brief Verifies seeker helpers and FHexMap surface remain non-wx model code.
 *
 * @author gpt-5.4 (high)
 * @date Created: May 25, 2026
 * @date Last Modified: May 25, 2026
 */
void testSeekerHelpersRemainModelOnlyAndFHexMapGeneric();

/**
 * @brief Verifies inactive seekers in a ship's path do not trigger movement contact.
 *
 * Confirms that checkForActiveSeekersOnPath skips inactive seekers entirely so
 * they cannot generate contact outcomes regardless of hex position.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: May 28, 2026
 * @date Last Modified: May 28, 2026
 */
void testInactiveSeekerIgnoredByPathContactCheck();

/**
 * @brief Verifies that a seeker is removed from the model after movement-path contact.
 *
 * Confirms that applyMovementSeekerDamage removes the contacting seeker from
 * m_seekerMissiles exactly once so no stale records remain after detonation.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: May 28, 2026
 * @date Last Modified: May 28, 2026
 */
void testSeekerRemovedFromModelAfterMovementContact();
/**
 * @brief Verifies movementPath is populated during resolveActiveSeekersForMovingPlayer.
 *
 * SMC-06 prerequisite: each active seeker's movementPath starts at the seeker's
 * pre-move hex, ends at the final hex after movement, is reset at the start of
 * each resolution pass (no stale accumulation), and non-moving (enemy/inactive)
 * seekers have their paths cleared.
 *
 * @author claude-sonnet-4-6 (standard)
 * @date Created: May 30, 2026
 * @date Last Modified: May 30, 2026
 */
void testMovementPathPopulatedByResolveActiveSeekers();
/**
 * @brief Behavioral: SMFR-05 — clearNonImpactingSeekerMovementPaths clears path but preserves bookkeeping.
 *
 * Constructs real FTacticalSeekerMissileState records with movementPath populated
 * and explicit movementAllowance/movementTurn values, seeds them into a real
 * FTacticalGame, calls clearNonImpactingSeekerMovementPaths(), and asserts
 * that movementPath is empty while movementAllowance and movementTurn are unchanged.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 19, 2026
 * @date Last Modified: Jun 19, 2026
 */
void testClearNonImpactingSeekerMovementPathsPreservesBookkeeping();

/**
 * @brief Behavioral: SMFR-05 — non-impacting seeker path empty after applyMovementSeekerDamage
 *        plus clearNonImpactingSeekerMovementPaths while movementAllowance/movementTurn survive.
 *
 * Sets up an impacting seeker (on a ship's path) and a non-impacting seeker (elsewhere).
 * Calls applyMovementSeekerDamage() to remove the impacting seeker, then calls
 * clearNonImpactingSeekerMovementPaths() on the surviving seeker.  Asserts:
 * - The impacting seeker is gone from m_seekerMissiles.
 * - The non-impacting seeker still has its movementAllowance and movementTurn.
 * - The non-impacting seeker's movementPath is now empty.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 19, 2026
 * @date Last Modified: Jun 19, 2026
 */
void testNonImpactingSeekerPathClearedAfterDamageApplied();

/**
 * @brief Source-contract supplement: completeMovePhase calls clearNonImpactingSeekerMovementPaths
 *        after applyMovementSeekerDamage in SMFR-05.
 *
 * Source-inspection test verifying the call order in completeMovePhase so the
 * clearNonImpactingSeekerMovementPaths call is guaranteed to run after the impacting
 * seekers have already been erased.  Supplements (does not replace) the behavioral
 * assertion in testNonImpactingSeekerPathClearedAfterDamageApplied.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 19, 2026
 * @date Last Modified: Jun 19, 2026
 */
void testCompleteMovePhaseCallsNonImpactingClearAfterDamageSourceContract();

/**
 * @brief Behavioral: SMFR-04 — seeker state fields for move-count label are correct.
 *
 * The drawSeekerMoveCountOverlay implementation picks count = movementPath.size()-1 when
 * a path is recorded, or movementAllowance otherwise.  This model-level behavioral test
 * verifies that FTacticalSeekerMissileState carries the correct field values in each case:
 * - A freshly activated seeker (no movement path) has movementAllowance accessible as a
 *   non-negative integer representing the fallback label value.
 * - After resolveActiveSeekersForMovingPlayer() the seeker's movementPath is non-empty so
 *   movementPath.size()-1 is the correct count (>= 0), confirming that value would be
 *   shown instead of movementAllowance.
 * - Two seekers at the same hex both retain independent movementAllowance values,
 *   confirming the stacked-display count computation is per-seeker.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 19, 2026
 * @date Last Modified: Jun 19, 2026
 */
void testSeekerMoveCountLabelFieldsReflectPathAndAllowance();
};

}

#endif
