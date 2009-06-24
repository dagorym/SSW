/**
 * @file FMinelayer.h
 * @brief Header file for the FMinelayerTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FMinelayerTest_H_
#define FMinelayerTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FMinelayer.h"

namespace FrontierTests {
using namespace Frontier;

class FMinelayerTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FMinelayerTest );
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

#endif /* FMinelayerTest_H_ */
