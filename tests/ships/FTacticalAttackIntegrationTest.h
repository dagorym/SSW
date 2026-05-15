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
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: May 02, 2026
 */
class FTacticalAttackIntegrationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalAttackIntegrationTest );
	CPPUNIT_TEST( testFireHitPreservesLegacyCleanupSideEffects );
	CPPUNIT_TEST( testFireCopiesAppliedHullDamageIntoAttackResult );
	CPPUNIT_TEST( testFireCopiesAppliedNonHullEffectsIntoAttackResult );
	CPPUNIT_TEST( testFireReportsConvertedADFHitAsHullDamage );
	CPPUNIT_TEST( testFireReportsDisastrousFireFallbackAsHullDamage );
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
};

}

#endif
