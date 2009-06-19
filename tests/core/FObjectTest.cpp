/**
 * @file FObjectTest.cpp
 * @brief Implementation file for the FObjectTest class
 * @author Tom Stephens
 * @date Created: DATE
 */

#include "FObjectTest.h"

namespace FrontierTests {
using namespace Frontier;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FObjectTest );

void FObjectTest::setUp(){
}

void FObjectTest::tearDown(){
}

void FObjectTest::testConstructor(){
	m_o = new FObject;
	delete m_o;
}


}
