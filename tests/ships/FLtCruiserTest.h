/**
 * @file FLtCruiser.h
 * @brief Header file for the FLtCruiserTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FLtCruiserTest_H_
#define FLtCruiserTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FLtCruiser.h"

namespace FrontierTests {
using namespace Frontier;

class FLtCruiserTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FLtCruiserTest );
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

#endif /* FLtCruiserTest_H_ */
