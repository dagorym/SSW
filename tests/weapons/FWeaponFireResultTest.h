/**
 * @file FWeaponFireResultTest.h
 * @brief Header file for the FWeaponFireResultTest class
 */

#ifndef FWeaponFireResultTest_H_
#define FWeaponFireResultTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FWeapon.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Validates structured tactical fire-result outcomes from FWeapon::fire().
 *
 * This fixture covers skipped-fire reasons and miss-path side effects so tactical
 * consumers receive deterministic outcome metadata.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 20, 2026
 * @date Last Modified: Mar 20, 2026
 */
class FWeaponFireResultTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FWeaponFireResultTest );
	CPPUNIT_TEST( testFireReturnsStructuredSkippedStateWhenOutOfAmmo );
	CPPUNIT_TEST( testFireReturnsStructuredSkippedStateWhenWeaponDamaged );
	CPPUNIT_TEST( testFireReturnsStructuredSkippedStateWhenNoTargetOrRangeInvalid );
	CPPUNIT_TEST( testFireReturnsStructuredMissAndPreservesLegacyFireSideEffects );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon *m_weapon;
	FVehicle *m_attacker;
	FVehicle *m_target;

public:
	/**
	 * @brief Creates a weapon and combatants for each fire-result scenario.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void setUp();

	/**
	 * @brief Releases weapon and combatant test fixtures after each scenario.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void tearDown();

	/**
	 * @brief Ensures out-of-ammo attacks report a structured not-fired result.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testFireReturnsStructuredSkippedStateWhenOutOfAmmo();

	/**
	 * @brief Ensures damaged weapons report the weapon-damaged skip reason.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testFireReturnsStructuredSkippedStateWhenWeaponDamaged();

	/**
	 * @brief Ensures no-target and invalid-range paths are distinguishable skip cases.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testFireReturnsStructuredSkippedStateWhenNoTargetOrRangeInvalid();

	/**
	 * @brief Ensures misses report TAO_Missed while preserving legacy fire side effects.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testFireReturnsStructuredMissAndPreservesLegacyFireSideEffects();
};

}

#endif
