/**
 * @file FHexPathTest.cpp
 * @brief Implementation file for the FHexPathTest class
 * @author Tom Stephens
 * @date Created: Oct 04, 2010
 */

#include "FHexPathTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FHexPathTest );

void FHexPathTest::setUp(){
	m_hl1 = new FHexPath();
}

void FHexPathTest::tearDown(){
	delete m_hl1;
}

void FHexPathTest::testConstructor(){
	CPPUNIT_ASSERT( m_hl1->getPathLength() == 0 );
}

void FHexPathTest::testAddPoint(){
	int result = m_hl1->addPoint(FPoint(1,1));
	CPPUNIT_ASSERT( result==1 );
	CPPUNIT_ASSERT( m_hl1->getPathLength() == 1 );
	result = m_hl1->addPoint(FPoint(1,2));
	CPPUNIT_ASSERT( result = 2 );
	CPPUNIT_ASSERT( m_hl1->getPathLength() == 2 );
}

void FHexPathTest::testAddPointWithGap(){
	int result = m_hl1->addPoint(FPoint(1,1));
	CPPUNIT_ASSERT( result==1 );
	CPPUNIT_ASSERT( m_hl1->getPathLength() == 1 );
	result = m_hl1->addPoint(FPoint(4,3));
	CPPUNIT_ASSERT( result==0 );
	CPPUNIT_ASSERT( m_hl1->getPathLength() == 1 );
}

void FHexPathTest::testAddFlag(){
	FPoint p1(1,1);
	m_hl1->addPoint(p1);
	int result = m_hl1->addFlag(p1,2);
	CPPUNIT_ASSERT( result==0 );
}

void FHexPathTest::testGetFlag(){
	FPoint p1(1,1);
	m_hl1->addPoint(p1);
	int result = m_hl1->addFlag(p1,2);
	CPPUNIT_ASSERT( result==0 );
	int flag = m_hl1->getFlag(p1);
	CPPUNIT_ASSERT( flag == 2 );
}

void FHexPathTest::testGetFlagNoPoint(){
	FPoint p1(1,1);
	m_hl1->addPoint(p1);
	int flag = m_hl1->getFlag(FPoint(10,3));
	CPPUNIT_ASSERT( flag == 0 );
}

void FHexPathTest::testFindPoint(){
	m_hl1->addPoint(FPoint(1,1));
	m_hl1->addPoint(FPoint(1,2));
	m_hl1->addPoint(FPoint(1,3));
	m_hl1->addPoint(FPoint(2,3));
	m_hl1->addPoint(FPoint(2,4));
	CPPUNIT_ASSERT( m_hl1->getPathLength() == 5 );
	bool test = m_hl1->isPointOnPath(FPoint(1,3));
	CPPUNIT_ASSERT( test == true );
}

void FHexPathTest::testFindPointNoPoint(){
	m_hl1->addPoint(FPoint(1,1));
	m_hl1->addPoint(FPoint(1,2));
	m_hl1->addPoint(FPoint(1,3));
	m_hl1->addPoint(FPoint(2,3));
	m_hl1->addPoint(FPoint(2,4));
	CPPUNIT_ASSERT( m_hl1->getPathLength() == 5 );
	bool test = m_hl1->isPointOnPath(FPoint(4,3));
	CPPUNIT_ASSERT( test == false );
}

}
