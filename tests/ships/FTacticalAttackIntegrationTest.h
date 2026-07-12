/**
 * @file FTacticalAttackIntegrationTest.h
 * @brief Header file for the FTacticalAttackIntegrationTest class
 */

#ifndef FTacticalAttackIntegrationTest_H_
#define FTacticalAttackIntegrationTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FWeapon.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Exercises tactical fire-result integration behavior against real weapon
 * and vehicle damage application paths.
 *
 * This fixture validates that fire outcomes and reported tactical damage effects
 * remain consistent with legacy side effects and fallback semantics.
 *
 * @author gpt-5.3-codex (medium), Claude Sonnet 5 (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Jul 11, 2026
 */
class FTacticalAttackIntegrationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalAttackIntegrationTest );
	CPPUNIT_TEST( testFireHitPreservesLegacyCleanupSideEffects );
	CPPUNIT_TEST( testFireCopiesAppliedHullDamageIntoAttackResult );
	CPPUNIT_TEST( testFireCopiesAppliedNonHullEffectsIntoAttackResult );
	CPPUNIT_TEST( testFireReportsConvertedADFHitAsHullDamage );
	CPPUNIT_TEST( testFireReportsDisastrousFireFallbackAsHullDamage );
	CPPUNIT_TEST( testMaskingScreenTorpedoOutResolvesAgainstTargetDefenseNotAttackerMSAndDoesNotHalveDamage );
	CPPUNIT_TEST( testParentlessLaserFireDoesNotCrashAndDoesNotHalveDamageAgainstNonMaskedTarget );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon *m_weapon;
	FVehicle *m_attacker;
	FVehicle *m_target;

public:
	/**
	 * @brief Creates fresh harness instances for each tactical fire-result scenario.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void setUp();

	/**
	 * @brief Releases harness instances created for a tactical fire-result scenario.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void tearDown();

	/**
	 * @brief Verifies hit outcomes still consume ammo and clear target state.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testFireHitPreservesLegacyCleanupSideEffects();

	/**
	 * @brief Confirms reported hull damage matches the applied advanced-damage result.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testFireCopiesAppliedHullDamageIntoAttackResult();

	/**
	 * @brief Confirms reported non-hull effects match the applied advanced-damage result.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testFireCopiesAppliedNonHullEffectsIntoAttackResult();

	/**
	 * @brief Verifies converted ADF hits are surfaced as hull-damage effects only.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testFireReportsConvertedADFHitAsHullDamage();

	/**
	 * @brief Verifies full Disastrous Fire fallback is reported as hull damage only.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testFireReportsDisastrousFireFallbackAsHullDamage();

	/**
	 * @brief Verifies a non-laser weapon (Torpedo) fired from a ship whose own
	 * Masking Screen is raised resolves to-hit against the TARGET's
	 * most-effective operating defense (with attract override), not the
	 * attacker's Masking Screen, and does not halve damage. This is the T3
	 * regression case: the "fired out of the screen" to-hit override is
	 * laser-only (LC/LB), so a Torpedo must ignore the attacker's raised MS
	 * entirely and resolve via FVehicle::resolveToHitModifier() on the
	 * target instead. This assertion fails against the pre-fix code, which
	 * applied the attacker's MS modifier to every weapon type.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testMaskingScreenTorpedoOutResolvesAgainstTargetDefenseNotAttackerMSAndDoesNotHalveDamage();

	/**
	 * @brief Verifies a laser weapon fired with no parent (as mines and
	 * seekers fire) against a target whose current defense is NOT a Masking
	 * Screen does not crash and does not halve damage. This exercises the
	 * damage-halving block's `(m_parent != NULL && ...)` guard on the
	 * attacker side of the halving condition -- the specific null-deref risk
	 * the guard was added to prevent, since the target-side check alone is
	 * false here and the OR must evaluate the (now-guarded) attacker side.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testParentlessLaserFireDoesNotCrashAndDoesNotHalveDamageAgainstNonMaskedTarget();
};

}

#endif
