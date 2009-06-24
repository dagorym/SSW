/**
 * @file FFighter.h
 * @brief Header file for the FFighterTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FFighterTest_H_
#define FFighterTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FFighter.h"

namespace FrontierTests {
using namespace Frontier;

class FFighterTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FFighterTest );
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

#endif /* FFighterTest_H_ */
