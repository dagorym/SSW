/**
 * @file FBattleship.h
 * @brief Header file for the FBattleshipTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FBattleshipTest_H_
#define FBattleshipTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FBattleship.h"

namespace FrontierTests {
using namespace Frontier;

class FBattleshipTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FBattleshipTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST_SUITE_END();

private:
	FVehicle * m_v1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
};

}

#endif /* FBattleshipTest_H_ */
