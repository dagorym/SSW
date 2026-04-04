/**
 * @file FGameMockStrategicUITest.h
 * @brief Header file for FGameMockStrategicUITest
 */

#ifndef FGameMockStrategicUITest_H_
#define FGameMockStrategicUITest_H_

#include <cppunit/extensions/HelperMacros.h>

#include "strategic/FGame.h"
#include "strategic/IStrategicUI.h"

namespace FrontierTests {
using namespace Frontier;

class FGameMockStrategicUITest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FGameMockStrategicUITest);
	CPPUNIT_TEST(testInitWithMockStrategicUI);
	CPPUNIT_TEST_SUITE_END();

private:
	FGame *m_game;
	IStrategicUI *m_ui;

public:
	void setUp();
	void tearDown();

	void testInitWithMockStrategicUI();
};

}

#endif /* FGameMockStrategicUITest_H_ */
