/**
 * @file FGameConfigTest.h
 * @brief Header file for the FGameConfigTest class
 * @author Tom Stephens
 * @date Created: Aug 2, 2009
 */

#ifndef FGameConfigTest_H_
#define FGameConfigTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "core/FGameConfig.h"

namespace FrontierTests {
using namespace Frontier;

class FGameConfigTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FGameConfigTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST_SUITE_END();

private:
	FGameConfig *m_c1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testSerialize();
};

}

#endif /* FGameConfigTest_H_ */
