/**
 * @file FPointTest.h
 * @brief Header file for the FPointTest class
 * @author Tom Stephens
 * @date Created: Jun 18, 2009
 */

#ifndef FPOINTTEST_H_
#define FPOINTTEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "core/FPoint.h"

namespace FrontierTests {
using namespace Frontier;

class FPointTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FPointTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testDefaultConstructor );
	CPPUNIT_TEST( testSetters );
	CPPUNIT_TEST( testGetDistance );
	CPPUNIT_TEST( testEquality );
	CPPUNIT_TEST( testInequality );
	CPPUNIT_TEST( testLessThan );
	CPPUNIT_TEST( testPrinting );
	CPPUNIT_TEST_SUITE_END();

private:
	FPoint * m_p1, *m_p3, *m_p4;
	FPoint m_p2;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testDefaultConstructor();
	void testSetters();
	void testGetDistance();
	void testEquality();
	void testInequality();
	void testLessThan();
	void testPrinting();
};

}

#endif /* FPOINTTEST_H_ */
