/**
 * @file FTacticalBattleDisplayFireFlowTest.h
 * @brief Header file for the FTacticalBattleDisplayFireFlowTest class
 */

#ifndef FTacticalBattleDisplayFireFlowTest_H_
#define FTacticalBattleDisplayFireFlowTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalBattleDisplayFireFlowTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalBattleDisplayFireFlowTest );
CPPUNIT_TEST( testDrawAndOnPaintUseBattleScreenStateAccessors );
CPPUNIT_TEST( testLegacyFireAllWeaponsHelperRemoved );
CPPUNIT_TEST( testDefensiveFireDoneDelegatesToModelFirePhaseResolution );
CPPUNIT_TEST( testOffensiveFireDoneDelegatesToModelFirePhaseResolution );
CPPUNIT_TEST( testWeaponSelectionDelegatesToBattleScreenModelApi );
CPPUNIT_TEST( testDefenseSelectionDelegatesToBattleScreenModelApi );
CPPUNIT_TEST( testMinePlacementFlowUsesModelMinePlacementApis );
CPPUNIT_TEST( testMinePlacementDisplayUsesModelShipList );
CPPUNIT_TEST( testDisplayClickFlowUsesModelForwardingApis );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertContains(const std::string & haystack, const std::string & needle);
void assertNotContains(const std::string & haystack, const std::string & needle);
void assertBefore(const std::string & haystack, const std::string & first, const std::string & second);
unsigned int countOccurrences(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

void testDrawAndOnPaintUseBattleScreenStateAccessors();
void testLegacyFireAllWeaponsHelperRemoved();
void testDefensiveFireDoneDelegatesToModelFirePhaseResolution();
void testOffensiveFireDoneDelegatesToModelFirePhaseResolution();
void testWeaponSelectionDelegatesToBattleScreenModelApi();
void testDefenseSelectionDelegatesToBattleScreenModelApi();
void testMinePlacementFlowUsesModelMinePlacementApis();
void testMinePlacementDisplayUsesModelShipList();
void testDisplayClickFlowUsesModelForwardingApis();
};

}

#endif
