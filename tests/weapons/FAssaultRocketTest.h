/**
 * @file FAssaultRocketTest.h
 * @brief Header file for the FAssaultRocketTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FAssaultRocketTest_H_
#define FAssaultRocketTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FAssaultRocket.h"

namespace FrontierTests {
using namespace Frontier;

class FAssaultRocketTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FAssaultRocketTest );
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

#endif /* FAssaultRocketTest_H_ */
