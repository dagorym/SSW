/**
 * @file FWeaponTest.h
 * @brief Header file for the FWeaponTest class
 * @author Tom Stephens
 * @date Created: Jun 18, 2009
 */

#ifndef FWeaponTest_H_
#define FWeaponTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FWeapon.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Validates baseline behavior for the FWeapon model and fire lifecycle.
 *
 * This fixture covers construction, ammo handling, serialization, targeting, and
 * fire-path skip semantics for both base and concrete weapon instances.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Jun 19, 2009
 * @date Last Modified: Mar 24, 2026
 */
class FWeaponTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FWeaponTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( setMaxAmmo );
	CPPUNIT_TEST( setCurrentAmmo );
	CPPUNIT_TEST( checkAddAmmoToName );
	CPPUNIT_TEST( reloadTest );
	CPPUNIT_TEST( setDamageStatus );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testBaseWeaponZeroRange );
	CPPUNIT_TEST( testSetTarget );
	CPPUNIT_TEST( testFireWhenEmpty );
	CPPUNIT_TEST( testFireNoTarget );
	CPPUNIT_TEST( testFireAtTarget );
	CPPUNIT_TEST( testBadIdentifier );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon *m_w1;

public:
	/**
	 * @brief Allocates a base weapon fixture before each unit test.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void setUp();

	/**
	 * @brief Releases the base weapon fixture after each unit test.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void tearDown();

	/**
	 * @brief Verifies default construction values for a base FWeapon instance.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void testConstructor();

	/**
	 * @brief Verifies maximum ammo assignment behavior.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void setMaxAmmo();

	/**
	 * @brief Verifies current ammo clamping relative to maximum ammo.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void setCurrentAmmo();

	/**
	 * @brief Verifies displayed short-name ammo suffix formatting.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void checkAddAmmoToName();

	/**
	 * @brief Verifies reload resets current ammo to configured maximum.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void reloadTest();

	/**
	 * @brief Verifies weapon damage-state flagging.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void setDamageStatus();

	/**
	 * @brief Verifies serialization round-trip of ammo and damage state.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void testSerialize();

	/**
	 * @brief Verifies zero-range base behavior and invalid-range fire skip semantics.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 24, 2026
	 * @date Last Modified: Mar 24, 2026
	 */
	void testBaseWeaponZeroRange();

	/**
	 * @brief Verifies target assignment and reset behavior.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void testSetTarget();

	/**
	 * @brief Exercises firing behavior when ammo is depleted.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void testFireWhenEmpty();

	/**
	 * @brief Exercises firing behavior when no target is assigned.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void testFireNoTarget();

	/**
	 * @brief Exercises hit/miss lifecycle and target cleanup across repeated firing.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 19, 2009
	 * @date Last Modified: Jun 19, 2009
	 */
	void testFireAtTarget();

	/**
	 * @brief Verifies invalid weapon identifiers are rejected by the factory.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	void testBadIdentifier();
};

}

#endif /* FWeaponTest_H_ */
