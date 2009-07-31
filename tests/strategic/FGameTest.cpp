/**
 * @file FGameTest.cpp
 * @brief Implementation file for the FGameTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FGameTest.h"
//#include "strategic/FGame.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FGameTest );

void FGameTest::setUp(){
	m_g1 = &(FGame::create());
}

void FGameTest::tearDown(){
	delete m_g1;
}

void FGameTest::testConstructor(){
}

void FGameTest::testInit(){
	CPPUNIT_ASSERT( 0 == m_g1->init(NULL));
}

void FGameTest::testShowPlayers(){
	m_g1->init(NULL);
	m_g1->showPlayers();
}

void FGameTest::testEndSatharTurn(){
	m_g1->init(NULL);
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == false);
	m_g1->endSatharTurn();
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == true);
}

void FGameTest::testPlaceNova(){
	bool result = false;
	m_g1->init(NULL);
	// it's initally the Sathar turn so trying to place SF Nova should fail
	result = m_g1->placeNova();
	CPPUNIT_ASSERT(result == false);
	m_g1->endSatharTurn();
	result = m_g1->placeNova();
	CPPUNIT_ASSERT(result == true);
}

void FGameTest::testEndUPFTurn(){
	m_g1->init(NULL);
	m_g1->endSatharTurn();
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == true);
	m_g1->endUPFTurn();
	CPPUNIT_ASSERT(m_g1->isUPFTurn() == false);
}

void FGameTest::testShowRetreatCondition(){
	m_g1->init(NULL);
	m_g1->showRetreatConditions();
}

void FGameTest::testGetPlayer(){
	m_g1->init(NULL);
	m_g1->showPlayers();
	FPlayer *p = m_g1->getPlayer(1);
	CPPUNIT_ASSERT(p != NULL);
	CPPUNIT_ASSERT(p->getName() == "UPF");
	p = m_g1->getPlayer(2);
	CPPUNIT_ASSERT(p != NULL);
	CPPUNIT_ASSERT(p->getName() == "Sathar");
}

void FGameTest::testSerialize(){
	m_g1->init(NULL);
	std::ofstream os("test",std::ios::binary);
	m_g1->save(os);
	os.close();
	delete m_g1;
	m_g1 = &(FGame::create());
	std::ifstream is("test",std::ios::binary);
	m_g1->load(is);
	is.close();
	CPPUNIT_ASSERT(m_g1->getPlayer(2)->getName() == "Sathar");

}

}
