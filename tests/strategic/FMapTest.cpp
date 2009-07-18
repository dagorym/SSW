/**
 * @file FMapTest.cpp
 * @brief Implementation file for the FMapTest class
 * @author Tom Stephens
 * @date Created: Jul 18, 2009
 */

#include "FMapTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FMapTest );

void FMapTest::setUp(){
	m_pList.push_back(0);
	m_pList.push_back(1);
}

void FMapTest::tearDown(){
}

void FMapTest::testConstructor(){
	FMap *m = &(FMap::create());
	CPPUNIT_ASSERT( m != NULL);
	delete m;

	m = &(FMap::create(true,m_pList));
	CPPUNIT_ASSERT( m != NULL);
	delete m;
}

void FMapTest::testCreateFrontier(){
	FMap *m = &(FMap::create(false,m_pList));
	CPPUNIT_ASSERT( m!=NULL );
	delete m;
	m = &(FMap::create(false,m_pList));
}

void FMapTest::testGetSystemByName(){
	FMap &m = FMap::getMap();
	CPPUNIT_ASSERT(m.getSystem("Prenglar") != NULL);
	CPPUNIT_ASSERT(m.getSystem("Joe's Pool Hall") == NULL);
}

void FMapTest::testGetSystemByID(){
	FMap &m = FMap::getMap();
	unsigned int id = m.getSystem("Prenglar")->getID();
	FSystem *s = m.getSystem(id);
	CPPUNIT_ASSERT(s->getName() == "Prenglar");
	CPPUNIT_ASSERT(m.getSystem(9999) == NULL);
}

void FMapTest::getJumpRouteBySystems(){
	FMap &m = FMap::getMap();
	CPPUNIT_ASSERT(m.getJumpRoute("Prenglar","Cassidine") != NULL );
	CPPUNIT_ASSERT(m.getJumpRoute("Prenglar","White Light") == NULL );
}

void FMapTest::getJumpRouteByID(){
	FMap &m = FMap::getMap();
	FJumpRoute *j = m.getJumpRoute("Prenglar","Cassidine");
	unsigned int id = j->getID();
	CPPUNIT_ASSERT(m.getJumpRoute(id)->getStart()->getName() == "Prenglar");
	CPPUNIT_ASSERT(m.getJumpRoute(id)->getEnd()->getName() == "Cassidine");
	CPPUNIT_ASSERT(m.getJumpRoute(22222) == NULL);
}

void FMapTest::testGetConnectedSystems(){
	FFleet f;
	f.setOwner(0);
	FMap &m = FMap::getMap();
	std::vector<std::string> sList = m.getConnectedSystems("Cassidine",0,&f);
	CPPUNIT_ASSERT( sList.size() == 3 );
	f.setMilitia(true,"Timeon");
	sList = m.getConnectedSystems("Prenglar",0,&f);
	CPPUNIT_ASSERT( sList.size() == 1 );
}

void FMapTest::testSerialize(){
	FMap *m = &(FMap::getMap());
	std::ofstream os("test",std::ios::binary);
	m->save(os);
	os.close();
	delete m;
	m = &(FMap::create());
	std::ifstream is("test",std::ios::binary);
	m->load(is);
	is.close();
	CPPUNIT_ASSERT(m->getSystem("White Light") != NULL);
}

void FMapTest::testDraw(){
	wxWindow w(NULL,wxID_ANY);
	wxClientDC dc(&w);
	FMap &m = FMap::getMap();
	m.draw(dc);
}

}
