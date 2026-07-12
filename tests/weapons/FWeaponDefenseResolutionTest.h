/**
 * @file FWeaponDefenseResolutionTest.h
 * @brief Header file for the FWeaponDefenseResolutionTest class
 */

#ifndef FWeaponDefenseResolutionTest_H_
#define FWeaponDefenseResolutionTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FWeapon.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Exercises FWeapon::fire()'s to-hit resolution against a target's
 * most-effective OPERATING defense (with the attracting-screen override) and
 * the laser-only masking-screen "fired out of the screen" effect, using real
 * FVehicle ship subclasses and real concrete weapon instances.
 *
 * @author Claude Sonnet 5 (medium)
 * @date Created: Jul 11, 2026
 * @date Last Modified: Jul 11, 2026
 */
class FWeaponDefenseResolutionTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FWeaponDefenseResolutionTest );
	CPPUNIT_TEST( testMostEffectiveDefenseResolvesAgainstHullOverRaisedProtonScreenForLaserBattery );
	CPPUNIT_TEST( testAttractOverrideResolvesElectronScreenForProtonBeamOverHull );
	CPPUNIT_TEST( testAttractOverrideResolvesProtonScreenForElectronBeamOverHull );
	CPPUNIT_TEST( testAttractOverrideResolvesStasisScreenForTorpedoSeekerAndMineOverHull );
	CPPUNIT_TEST( testMaskingScreenLaserOutUsesAttackerModifierAndHalvesDamage );
	CPPUNIT_TEST( testMaskingScreenLaserInUsesTargetModifierAndHalvesDamage );
	CPPUNIT_TEST( testParentlessMineFireDoesNotCrashAndResolvesAgainstTargetDefense );
	CPPUNIT_TEST_SUITE_END();

public:
	/// no shared fixture state; each test constructs its own real ships and weapons
	void setUp();
	/// no shared fixture state to release
	void tearDown();

	/**
	 * @brief AC: a target owning a reflective hull with a raised Proton
	 * Screen, fired at by a Laser Battery, resolves to-hit against the hull
	 * (modifier 0), not the raised Proton Screen (+15), because the hull is
	 * the most-effective operating defense and Proton Screen does not
	 * attract Laser Battery.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testMostEffectiveDefenseResolvesAgainstHullOverRaisedProtonScreenForLaserBattery();

	/**
	 * @brief AC: a target with a raised Electron Screen fired at by a Proton
	 * Beam resolves against the Electron Screen (attract modifier +10), not
	 * the hull (0), because Electron Screen attracts Proton Beam.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testAttractOverrideResolvesElectronScreenForProtonBeamOverHull();

	/**
	 * @brief AC (analogous case): a target with a raised Proton Screen fired
	 * at by an Electron Battery resolves against the Proton Screen (attract
	 * modifier +10), not the hull (0), because Proton Screen attracts
	 * Electron Beam.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testAttractOverrideResolvesProtonScreenForElectronBeamOverHull();

	/**
	 * @brief AC (analogous case): a target with a raised Stasis Screen fired
	 * at by a Torpedo, a Seeker Missile, or a Mine resolves against the
	 * Stasis Screen's attract modifier for each weapon type, not the hull
	 * (0), because Stasis Screen attracts all three.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testAttractOverrideResolvesStasisScreenForTorpedoSeekerAndMineOverHull();

	/**
	 * @brief AC (T3): an attacker with its Masking Screen raised firing a
	 * laser resolves to-hit using the attacker's MS laser modifier and
	 * halves damage (rounded up).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testMaskingScreenLaserOutUsesAttackerModifierAndHalvesDamage();

	/**
	 * @brief AC: an attacker without a raised Masking Screen firing a laser
	 * at a target with a raised Masking Screen resolves against the target's
	 * MS and halves damage (rounded up).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testMaskingScreenLaserInUsesTargetModifierAndHalvesDamage();

	/**
	 * @brief AC: a parentless weapon (a Mine, fired as it is in the real
	 * game from a hex rather than a ship) does not crash when fired, and
	 * still resolves its to-hit modifier against the target's operating
	 * defenses (including the attracting-screen override).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testParentlessMineFireDoesNotCrashAndResolvesAgainstTargetDefense();
};

}

#endif /* FWeaponDefenseResolutionTest_H_ */
