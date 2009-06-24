/**
 * @file FHvCruiser.h
 * @brief Header file for the FHvCruiserTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FHvCruiserTest_H_
#define FHvCruiserTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FHvCruiser.h"

namespace FrontierTests {
using namespace Frontier;

class FHvCruiserTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FHvCruiserTest );
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

#endif /* FHvCruiserTest_H_ */
