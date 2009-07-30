/**
 * @file FGameTest.cpp
 * @brief Implementation file for the FGameTest class
 * @author Tom Stephens
 * @date Created: Jun 19, 2009
 */

#include "FGameTest.h"
//#include "strategic/FGame.h"

#include <wx/wx.h>
#include <wx/dcmemory.h>

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
//	wxBitmap b(800,600);
//	wxMemoryDC dc;
//	dc.SelectObject(b);
	m_g1->init(NULL);
}

}
