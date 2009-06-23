/**
 * @file FSeekerMissileLauncherTest.h
 * @brief Header file for the FSeekerMissileLauncherTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FSeekerMissileLauncherTest_H_
#define FSeekerMissileLauncherTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "weapons/FSeekerMissileLauncher.h"

namespace FrontierTests {
using namespace Frontier;

class FSeekerMissileLauncherTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FSeekerMissileLauncherTest );
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

#endif /* FSeekerMissileLauncherTest_H_ */
