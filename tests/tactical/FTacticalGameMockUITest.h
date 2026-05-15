/**
 * @file FTacticalGameMockUITest.h
 * @brief Header file for FTacticalGame mock UI runtime tests
 */

#ifndef FTACTICALGAMEMOCKUITEST_H_
#define FTACTICALGAMEMOCKUITEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalGameMockUITest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
class FTacticalGameMockUITest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( FTacticalGameMockUITest );
CPPUNIT_TEST( testDeterministicTwoShipCombatDeclaresAttackerWinnerViaModelState );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();

/**
 * @brief Verifies deterministic Two Ship Combat Declares Attacker Winner Via Model State.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Apr 03, 2026
 * @date Last Modified: Apr 03, 2026
 */
void testDeterministicTwoShipCombatDeclaresAttackerWinnerViaModelState();
};

}

#endif
