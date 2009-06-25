/**
 * @file FFleetTest.h
 * @brief Header file for the FFleetTest class
 * @author Tom Stephens
 * @date Created: Jun 24, 2009
 */

#ifndef FFleetTest_H_
#define FFleetTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FFleet.h"

namespace FrontierTests {
using namespace Frontier;

class FFleetTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FFleetTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testBasicSetters );
	CPPUNIT_TEST( testSetLocation );
	CPPUNIT_TEST_SUITE_END();

private:
	FFleet *m_f1,*m_f2;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testBasicSetters();
	void testSetLocation();
};

}

#endif /* FFleetTest_H_ */
