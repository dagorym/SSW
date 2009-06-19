/**
 * @file FObjectTest.h
 * @brief Header file for the FObjectTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#ifndef FObjectTest_H_
#define FObjectTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "core/FObject.h"

namespace FrontierTests {
using namespace Frontier;

class FObjectTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FObjectTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST_SUITE_END();

private:
	FObject *m_o;

public:
	void setUp();
	void tearDown();

	void testConstructor();
};

}

#endif /* FObjectTest_H_ */
