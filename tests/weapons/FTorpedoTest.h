/**
 * @file FTorpedoTest.h
 * @brief Header file for the FTorpedoTest class
 * @author Tom Stephens
 * @date Created: Jun 22, 2009
 */

#ifndef FTorpedoTest_H_
#define FTorpedoTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FTorpedo.h"

namespace FrontierTests {
using namespace Frontier;

class FTorpedoTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FTorpedoTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST_SUITE_END();

private:
	FWeapon * m_w1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
};

}

#endif /* FTorpedoTest_H_ */
