/**
 * @file FTacticalBattleScreenElectricalFireTest.h
 * @brief Header file for the FTacticalBattleScreenElectricalFireTest class
 */

#ifndef FTacticalBattleScreenElectricalFireTest_H_
#define FTacticalBattleScreenElectricalFireTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <vector>

namespace FrontierTests {

class FTacticalBattleScreenElectricalFireTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalBattleScreenElectricalFireTest );
CPPUNIT_TEST( testApplyFireDamageDelegatesToTacticalModel );
CPPUNIT_TEST( testDeclareWinnerUsesTacticalUIAndGuardedClosePath );
CPPUNIT_TEST( testCloseBattleScreenUsesModelCloseGuardWithModalFirstPath );
CPPUNIT_TEST( testModalCallerSitesOwnBattleScreenLifetimeAfterShowModal );
CPPUNIT_TEST_SUITE_END();

private:
std::string readFile(const std::string & path);
std::string extractFunctionBody(const std::string & source, const std::string & signature);
void assertAppearsInOrder(const std::string & haystack, const std::vector<std::string> & needles);
unsigned int countOccurrences(const std::string & haystack, const std::string & needle);

public:
void setUp();
void tearDown();

void testApplyFireDamageDelegatesToTacticalModel();
void testDeclareWinnerUsesTacticalUIAndGuardedClosePath();
void testCloseBattleScreenUsesModelCloseGuardWithModalFirstPath();
void testModalCallerSitesOwnBattleScreenLifetimeAfterShowModal();
};

}

#endif
