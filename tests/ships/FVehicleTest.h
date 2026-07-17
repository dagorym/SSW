/**
 * @file FVehicleTest.h
 * @brief Header file for the FVehicleTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#ifndef FVehicleTest_H_
#define FVehicleTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FVehicle.h"
#include "tactical/FTacticalCombatReport.h"

namespace FrontierTests {
using namespace Frontier;

/**
 * @brief Validates baseline and tactical-damage behavior for FVehicle.
 *
 * This fixture combines legacy constructor/serialization coverage with tactical
 * damage-resolution regression scenarios for hull, subsystem, and fallback paths.
 *
 * @author Tom Stephens, gpt-5.3-codex (medium)
 * @date Created: Jun 23, 2009
 * @date Last Modified: May 02, 2026
 */
class FVehicleTest : public CppUnit::TestFixture, public Frontier::FPObject{
	CPPUNIT_TEST_SUITE( FVehicleTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testSetters );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST( testBadType );
	CPPUNIT_TEST( testTakeDamageBasicPopulatesOptionalResolution );
	CPPUNIT_TEST( testTakeDamageAdvancedPreservesMutationWithOptionalResolution );
	CPPUNIT_TEST( testDamageHelpersReportExplicitComponentMetadata );
	CPPUNIT_TEST( testAdvancedDamageKeepsADFAndMRCumulativeUntilZero );
	CPPUNIT_TEST( testAdvancedDamageFallsBackForAlreadyDamagedSubsystems );
	CPPUNIT_TEST( testAdvancedDamageStillDamagesEligibleWeaponAndDefenseComponents );
	CPPUNIT_TEST( testDisastrousFireAppliesFullEligibleBundleWithoutHullDamage );
	CPPUNIT_TEST( testDisastrousFireReportsOnlyNewPartialEffectsWithoutHullDamage );
	CPPUNIT_TEST( testDisastrousFireFallsBackToHullWhenNoBundledEffectCanApply );
	CPPUNIT_TEST( testLoadReassignsCurrentDefenseToLiveDefenseAfterMultiDefenseReload );
	CPPUNIT_TEST( testLoadedVehicleSurvivesWeaponFireViaCurrentDefense );
	CPPUNIT_TEST( testSerializeRoundTripsWeaponsDefensesAndState );
	CPPUNIT_TEST( testLoadRestoresActiveDefenseSelectionAcrossSaveLoad );
	CPPUNIT_TEST( testLoadAdvancesNextIDPastLoadedID );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnUnknownWeaponType );
	CPPUNIT_TEST( testLoadReturnsNonzeroOnUnknownDefenseType );
	CPPUNIT_TEST_SUITE_END();

private:
	FVehicle *m_v1;

	/**
	 * @brief Satisfies FPObject serialization requirements for the fixture type.
	 *
	 * @param os Output stream placeholder for fixture compatibility.
	 * @return Always returns 0 for this test fixture.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	const virtual int save(std::ostream &os) const;

	/**
	 * @brief Satisfies FPObject deserialization requirements for the fixture type.
	 *
	 * @param is Input stream placeholder for fixture compatibility.
	 * @return Always returns 0 for this test fixture.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	virtual int load(std::istream &is);

public:
	/**
	 * @brief Allocates a baseline vehicle instance used by legacy unit tests.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	void setUp();

	/**
	 * @brief Releases the baseline vehicle instance after each legacy unit test.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	void tearDown();

	/**
	 * @brief Verifies default vehicle construction values and identity metadata.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	void testConstructor();

	/**
	 * @brief Verifies setter behavior and bounds handling for core vehicle fields.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	void testSetters();

	/**
	 * @brief Verifies vehicle persistence and reload behavior for serialized fields.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	void testSerialize();

	/**
	 * @brief Verifies unknown ship identifiers fail creation cleanly.
	 *
	 * @author Tom Stephens
	 * @date Created: Jun 23, 2009
	 * @date Last Modified: Jun 23, 2009
	 */
	void testBadType();

	/**
	 * @brief Verifies basic damage flow reports explicit hull damage details.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testTakeDamageBasicPopulatesOptionalResolution();

	/**
	 * @brief Verifies optional reporting preserves legacy advanced-damage mutations.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testTakeDamageAdvancedPreservesMutationWithOptionalResolution();

	/**
	 * @brief Verifies component-hit effects include explicit weapon and defense metadata.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 20, 2026
	 * @date Last Modified: Mar 20, 2026
	 */
	void testDamageHelpersReportExplicitComponentMetadata();

	/**
	 * @brief Verifies repeated ADF/MR hits reduce cumulatively before hull fallback.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testAdvancedDamageKeepsADFAndMRCumulativeUntilZero();

	/**
	 * @brief Verifies already-damaged subsystem rolls fall back to hull damage.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testAdvancedDamageFallsBackForAlreadyDamagedSubsystems();

	/**
	 * @brief Verifies component rolls still disable eligible undamaged weapons/defenses.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testAdvancedDamageStillDamagesEligibleWeaponAndDefenseComponents();

	/**
	 * @brief Verifies Disastrous Fire applies all eligible bundled non-hull effects.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testDisastrousFireAppliesFullEligibleBundleWithoutHullDamage();

	/**
	 * @brief Verifies partial Disastrous Fire bundles report only newly applied effects.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testDisastrousFireReportsOnlyNewPartialEffectsWithoutHullDamage();

	/**
	 * @brief Verifies Disastrous Fire falls back to hull when no bundled effect can apply.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: May 02, 2026
	 * @date Last Modified: May 02, 2026
	 */
	void testDisastrousFireFallsBackToHullWhenNoBundledEffectCanApply();

	/**
	 * @brief Verifies CRIT-3: load() repoints m_currentDefense at the live
	 * reloaded defense instead of leaving it dangling at a freed pre-load entry.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testLoadReassignsCurrentDefenseToLiveDefenseAfterMultiDefenseReload();

	/**
	 * @brief Verifies CRIT-3: a freshly loaded vehicle can be fired upon (the
	 * combat damage path reads target->getCurrentDefense() directly) without
	 * a use-after-free.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 11, 2026
	 * @date Last Modified: Jul 11, 2026
	 */
	void testLoadedVehicleSurvivesWeaponFireViaCurrentDefense();

	/**
	 * @brief Verifies P5-2: a vehicle with multiple weapons and defenses
	 * round-trips through save()->load() (via std::stringstream) with all
	 * weapons (type/ammo/damage state), defenses (type/ammo/damage state),
	 * and HP/MR/ADF/DCR/heading/speed/owner/flag state preserved exactly.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testSerializeRoundTripsWeaponsDefensesAndState();

	/**
	 * @brief Verifies P5-2 H2: a vehicle saved with a non-default active
	 * defense (Masking Screen raised via setCurrentDefense(...)) reloads with
	 * the same defense reported by getCurrentDefense(), with the
	 * masking-screen turn count and ammo state consistent (not re-applied or
	 * reset) across the round trip.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadRestoresActiveDefenseSelectionAcrossSaveLoad();

	/**
	 * @brief Verifies P5-2 H3: after load() reads a vehicle with a large
	 * m_ID, a subsequently default-constructed FVehicle receives an ID
	 * strictly greater than the loaded ID (the static m_nextID counter is
	 * advanced past any loaded ID).
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadAdvancesNextIDPastLoadedID();

	/**
	 * @brief Verifies P5-2: FVehicle::load() returns nonzero (without a NULL
	 * dereference/crash) when the wire stream encodes an unrecognized weapon
	 * type tag that createWeapon(...) cannot resolve.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadReturnsNonzeroOnUnknownWeaponType();

	/**
	 * @brief Verifies P5-2: FVehicle::load() returns nonzero (without a NULL
	 * dereference/crash) when the wire stream encodes an unrecognized defense
	 * type tag that createDefense(...) cannot resolve.
	 *
	 * @author Claude Sonnet 5 (medium)
	 * @date Created: Jul 17, 2026
	 * @date Last Modified: Jul 17, 2026
	 */
	void testLoadReturnsNonzeroOnUnknownDefenseType();
};

}

#endif /* FVehicleTest_H_ */
