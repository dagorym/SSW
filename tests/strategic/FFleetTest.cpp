/**
 * @file FFleetTest.cpp
 * @brief Implementation file for the FFleetTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FFleetTest.h"
#include "strategic/FSystem.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FFleetTest );

void FFleetTest::setUp(){
	m_f1 = new FFleet;
}

void FFleetTest::tearDown(){
	delete m_f1;
}

void FFleetTest::testConstructor(){
	CPPUNIT_ASSERT( m_f1->getID() >=0 );
	CPPUNIT_ASSERT( m_f1->getLocation() == 0 );
	CPPUNIT_ASSERT( m_f1->getInTransit() == false );
	CPPUNIT_ASSERT( m_f1->isMilitia() == false );
	CPPUNIT_ASSERT( m_f1->getIcon() == NULL );
	CPPUNIT_ASSERT( m_f1->isHolding() == false );
	CPPUNIT_ASSERT( m_f1->getSpeed() == 0 );
	CPPUNIT_ASSERT( m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT( m_f1->getHomeSystem() == "" );
	CPPUNIT_ASSERT( m_f1->getCoord(0) == 0 );
	CPPUNIT_ASSERT( m_f1->getCoord(1) == 0 );
	CPPUNIT_ASSERT( m_f1->getDestination() == 0 );
	CPPUNIT_ASSERT( m_f1->getJumpRoute() == 0 );
}

void FFleetTest::testBasicSetters(){
	m_f1->setInTransit(true);
	CPPUNIT_ASSERT( m_f1->getInTransit() == true );
	m_f1->setOwner(3);
	CPPUNIT_ASSERT( m_f1->getOwner() == 3 );
	m_f1->setName("Destroyers");
	CPPUNIT_ASSERT( m_f1->getName() == "Destroyers" );
	m_f1->setSpeed(2);
	CPPUNIT_ASSERT( m_f1->getSpeed() == 2 );
	m_f1->setLocation(10);
	CPPUNIT_ASSERT( m_f1->getLocation() == 10 );
	m_f1->setDestination(12);
	CPPUNIT_ASSERT( m_f1->getDestination() == 12 );
	m_f1->setIcon("../../icons/UPFFleet.png");
	CPPUNIT_ASSERT( m_f1->getIcon() != NULL );
	m_f1->setIcon("../../icons/SatharFleet.png");
	CPPUNIT_ASSERT( m_f1->getIcon() != NULL );
	m_f1->setMilitia(true,"Prenglar");
	CPPUNIT_ASSERT( m_f1->isMilitia() == true );
	CPPUNIT_ASSERT( m_f1->getHomeSystem() == "Prenglar" );
	m_f1->setHolding(true);
	CPPUNIT_ASSERT( m_f1->isHolding() == true );
}

void FFleetTest::testSetLocation(){
	FSystem *s = new FSystem;
	m_f1->setLocation(s,false);
	CPPUNIT_ASSERT(m_f1->getLocation() >= 0 );
	CPPUNIT_ASSERT(m_f1->getDestination() == -1 );
	CPPUNIT_ASSERT(m_f1->getInTransit() == false );
	CPPUNIT_ASSERT(m_f1->getTransitTime() == 0 );
	CPPUNIT_ASSERT(m_f1->getSpeed() == 0 );
	CPPUNIT_ASSERT(m_f1->getJumpRoute() == -1 );
//	CPPUNIT_ASSERT(m_f1->getCoord(0) == 0 );
//	CPPUNIT_ASSERT(m_f1->getCoord(1) == 0 );
}

}
