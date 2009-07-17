/**
 * @file FJumpRouteTest.cpp
 * @brief Implementation file for the FJumpRouteTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FJumpRouteTest.h"
#include "strategic/FSystem.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FJumpRouteTest );

void FJumpRouteTest::setUp(){
	m_j1 = new FJumpRoute();
}

void FJumpRouteTest::tearDown(){
	delete m_j1;
}

void FJumpRouteTest::testConstructor(){
	CPPUNIT_ASSERT(m_j1->getStart() == NULL);
	CPPUNIT_ASSERT(m_j1->getEnd() == NULL);
	CPPUNIT_ASSERT(m_j1->getID() == 0);
	CPPUNIT_ASSERT(m_j1->getLength() == 0);
	CPPUNIT_ASSERT(m_j1->isKnown(0) == false);
}

void FJumpRouteTest::testFullConstructor(){
	FSystem *s = new FSystem("a",2,0,0,0);
	FSystem *e = new FSystem("b",0,0,0,0);
	std::vector<unsigned int> p;
	p.push_back(0);
	FJumpRoute j(s,e,p);
	CPPUNIT_ASSERT(j.getStart()->getID() == s->getID());
	CPPUNIT_ASSERT(j.getEnd()->getID() == e->getID());
	CPPUNIT_ASSERT(j.getID() == 0);
	CPPUNIT_ASSERT(j.getLength() == 2);
	CPPUNIT_ASSERT(j.isKnown(0) == true);
	delete s;
	delete e;
}

void FJumpRouteTest::testGetJumpTimeSpeed0(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(0,10) == 10);
	CPPUNIT_ASSERT(m_j1->getJumpTime(4,10) == 10);
}

void FJumpRouteTest::testGetJumpTimeSpeed1(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(1,10) == 10);
	FSystem *s = new FSystem("a",2,0,0,0);
	FSystem *e = new FSystem("b",0,0,0,0);
	std::vector<unsigned int> p;
	p.push_back(0);
	FJumpRoute j(s,e,p);
	CPPUNIT_ASSERT(j.getJumpTime(1) == 2);
}

void FJumpRouteTest::testGetJumpTimeSpeed2(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(2,10) == 6);
	CPPUNIT_ASSERT(m_j1->getJumpTime(2,5) == 3);
	CPPUNIT_ASSERT(m_j1->getJumpTime(2,8) == 4);
}

void FJumpRouteTest::testGetJumpTimeSpeed3(){
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,5) == 2);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,6) == 2);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,7) == 3);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,8) == 4);
	CPPUNIT_ASSERT(m_j1->getJumpTime(3,10) == 4);
}

void FJumpRouteTest::testSerialize(){
	FSystem *s = new FSystem("a",2,0,0,0);
	FSystem *e = new FSystem("b",0,0,0,0);
	std::vector<unsigned int> p;
	p.push_back(1);
	FJumpRoute j(s,e,p);
	std::ofstream os("test",std::ios::binary);
	j.save(os);
	os.close();
	FJumpRoute j2;
	std::ifstream is("test",std::ios::binary);
	j2.load(is);
	is.close();
	CPPUNIT_ASSERT((unsigned int)(j2.getStart()) == s->getID());
	CPPUNIT_ASSERT((unsigned int)(j2.getEnd()) == e->getID());
	CPPUNIT_ASSERT(j2.getID() == 0);
	CPPUNIT_ASSERT(j2.getLength() == 2);
	CPPUNIT_ASSERT(j2.isKnown(0) == false);
	CPPUNIT_ASSERT(j2.isKnown(1) == true);


}

}
