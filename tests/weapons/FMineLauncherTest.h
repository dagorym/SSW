/**
 * @file FMineLauncherTest.h
 * @brief Header file for the FMineLauncherTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FMineLauncherTest_H_
#define FMineLauncherTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FMineLauncher.h"

namespace FrontierTests {
using namespace Frontier;

class FMineLauncherTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FMineLauncherTest );
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

#endif /* FMineLauncherTest_H_ */
