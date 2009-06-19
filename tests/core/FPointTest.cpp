/**
 * @file FPointTest.cpp
 * @brief Implementation file for the FPointTest class
 * @author Tom Stephens
 * @date Created: Jun 18, 2009
 */

#include "FPointTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FPointTest );

void FPointTest::setUp(){
	m_p1 = new FPoint(1,2);
	m_p3 = new FPoint(2,5);
	m_p4 = new FPoint(6,2);
}

void FPointTest::tearDown(){
	delete m_p1;
	delete m_p3;
	delete m_p4;
}

void FPointTest::testConstructor(){
	CPPUNIT_ASSERT( m_p1->getX() == 1 );
	CPPUNIT_ASSERT( m_p1->getY() == 2 );
}

void FPointTest::testDefaultConstructor(){
	FPoint p;
	CPPUNIT_ASSERT( p.getX() == -9999 );
	CPPUNIT_ASSERT( p.getY() == -9999 );
}

void FPointTest::testSetters(){
	m_p2.setX(1);
	m_p2.setY(2);
	CPPUNIT_ASSERT( m_p2.getX() == 1 );
	CPPUNIT_ASSERT( m_p2.getY() == 2 );
	m_p2.setPoint(2,4);
	CPPUNIT_ASSERT( m_p2.getX() == 2 );
	CPPUNIT_ASSERT( m_p2.getY() == 4 );
}

void FPointTest::testGetDistance(){
	CPPUNIT_ASSERT( m_p3->getDistance(*m_p4) == 5. );

}

void FPointTest::testEquality(){
	m_p2.setPoint(1,2);
	CPPUNIT_ASSERT( *m_p1 == m_p2 );
	CPPUNIT_ASSERT( !(*m_p1 == *m_p3) );
}

void FPointTest::testInequality(){
	m_p2.setPoint(2,1);
	CPPUNIT_ASSERT( *m_p1 != m_p2 );
	m_p2.setPoint(1,2);
	CPPUNIT_ASSERT( !(*m_p1 != m_p2) );
}

void FPointTest::testLessThan(){
	m_p2.setPoint(1,2);
	CPPUNIT_ASSERT( !(*m_p1 < m_p2));
	CPPUNIT_ASSERT( *m_p4 < *m_p3);
	m_p2.setX(0);
	CPPUNIT_ASSERT( m_p2 < *m_p1);
	CPPUNIT_ASSERT( !(m_p2 < m_p2));
}

void FPointTest::testPrinting(){
	std::ostringstream os;
	os << *m_p1;
	CPPUNIT_ASSERT( os.str() == "(1, 2)" );
}

}
