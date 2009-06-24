/**
 * @file FFortress.h
 * @brief Header file for the FFortressTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FFortressTest_H_
#define FFortressTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FFortress.h"

namespace FrontierTests {
using namespace Frontier;

class FFortressTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FFortressTest );
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

#endif /* FFortressTest_H_ */
