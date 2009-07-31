/**
 * @file FGameTest.h
 * @brief Header file for the FGameTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#ifndef FGameTest_H_
#define FGameTest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FGame.h"

namespace FrontierTests {
using namespace Frontier;

class FGameTest : public CppUnit::TestFixture{
	CPPUNIT_TEST_SUITE( FGameTest );
	CPPUNIT_TEST( testConstructor );
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST( testShowPlayers );
	CPPUNIT_TEST( testEndSatharTurn );
	CPPUNIT_TEST( testPlaceNova );
	CPPUNIT_TEST( testEndUPFTurn );
	CPPUNIT_TEST( testShowRetreatCondition );
	CPPUNIT_TEST( testGetPlayer );
	CPPUNIT_TEST( testSerialize );
	CPPUNIT_TEST_SUITE_END();

private:
	FGame *m_g1;

public:
	void setUp();
	void tearDown();

	void testConstructor();
	void testInit();
	void testShowPlayers();
	void testPlaceNova();
	void testEndSatharTurn();
	void testEndUPFTurn();
	void testShowRetreatCondition();
	void testGetPlayer();
	void testSerialize();
};

}

#endif /* FGameTest_H_ */
