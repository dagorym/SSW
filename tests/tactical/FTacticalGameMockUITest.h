/**
 * @file FTacticalGameMockUITest.h
 * @brief Header file for FTacticalGame mock UI runtime tests
 */

#ifndef FTACTICALGAMEMOCKUITEST_H_
#define FTACTICALGAMEMOCKUITEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FTacticalGameMockUITest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalGameMockUITest );
CPPUNIT_TEST( testDeterministicTwoShipCombatDeclaresAttackerWinnerViaModelState );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

void testDeterministicTwoShipCombatDeclaresAttackerWinnerViaModelState();
};

}

#endif
