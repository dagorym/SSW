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
	CPPUNIT_TEST_SUITE_END();

private:
	FVehicle *m_v1;
	const virtual int save(std::ostream &os) const;
	virtual int load(std::istream &is);

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testSetters();
	void testSerialize();
	void testBadType();
	void testTakeDamageBasicPopulatesOptionalResolution();
	void testTakeDamageAdvancedPreservesMutationWithOptionalResolution();
	void testDamageHelpersReportExplicitComponentMetadata();
	void testAdvancedDamageKeepsADFAndMRCumulativeUntilZero();
	void testAdvancedDamageFallsBackForAlreadyDamagedSubsystems();
	void testAdvancedDamageStillDamagesEligibleWeaponAndDefenseComponents();
};

}

#endif /* FVehicleTest_H_ */
