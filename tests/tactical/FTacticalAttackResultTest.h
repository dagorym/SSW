/**
 * @file FTacticalAttackResultTest.h
 * @brief Header file for the FTacticalAttackResultTest class
 */

#ifndef FTacticalAttackResultTest_H_
#define FTacticalAttackResultTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "tactical/FTacticalAttackResult.h"

namespace FrontierTests {
using namespace Frontier;

class FTacticalAttackResultTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalAttackResultTest );
	CPPUNIT_TEST( testDefaultConstructorRepresentsNotFiredAttack );
	CPPUNIT_TEST( testHelpersAndEffectsReflectMutatedOutcomeState );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testDefaultConstructorRepresentsNotFiredAttack();
	void testHelpersAndEffectsReflectMutatedOutcomeState();
};

}

#endif
