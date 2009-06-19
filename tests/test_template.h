/**
 * @file XXX.h
 * @brief Header file for the XXX class
 * @author Tom Stephens
 * @date Created: DATE
 */

#ifndef XXX_H_
#define XXX_H_

#include <cppunit/extensions/HelperMacros.h>

#include "core/xxx.h"

namespace FrontierTests {
using namespace Frontier;

class XXX : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( XXX );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST_SUITE_END();

private:

public:
	void setUp();
	void tearDown();

	void testConstructor();
};

}

#endif /* XXX_H_ */
