/**
 * @file FFrigate.h
 * @brief Header file for the FFrigateTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FFrigateTest_H_
#define FFrigateTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FFrigate.h"

namespace FrontierTests {
using namespace Frontier;

class FFrigateTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FFrigateTest );
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

#endif /* FFrigateTest_H_ */
