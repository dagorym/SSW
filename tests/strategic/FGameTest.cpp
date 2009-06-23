/**
 * @file FGameTest.cpp
 * @brief Implementation file for the FGameTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FGameTest.h"
#include "strategic/FGame.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FGameTest );

void FGameTest::setUp(){
}

void FGameTest::tearDown(){
}

void FGameTest::testConstructor(){
FGame &g1 = FGame::create();
}

}
