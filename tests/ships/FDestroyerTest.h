/**
 * @file FDestroyer.h
 * @brief Header file for the FDestroyerTest class
 * @author Tom Stephens
 * @date Created: Jun 23, 2009
 */

#ifndef FDestroyerTest_H_
#define FDestroyerTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "ships/FDestroyer.h"

namespace FrontierTests {
using namespace Frontier;

class FDestroyerTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FDestroyerTest );
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

#endif /* FDestroyerTest_H_ */
