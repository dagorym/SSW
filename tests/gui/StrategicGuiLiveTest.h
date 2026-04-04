/**
 * @file StrategicGuiLiveTest.h
 * @brief Header file for live strategic GUI smoke coverage
 */

#ifndef _STRATEGICGUILIVETEST_H_
#define _STRATEGICGUILIVETEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "WXGuiTestHarness.h"

namespace FrontierTests {

class StrategicGuiLiveTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( StrategicGuiLiveTest );
CPPUNIT_TEST( testMainFrameBuildsExpectedInitialUI );
CPPUNIT_TEST( testGamePanelPaintTracksParentSize );
CPPUNIT_TEST( testStrategicDialogsCloseModallyWithoutInput );
CPPUNIT_TEST( testWXStrategicUIParentBackedModalAndRedrawPaths );
CPPUNIT_TEST( testUPFAndSatharFleetDialogsMutateModelState );
CPPUNIT_TEST( testTransferShipsDialogMovesShipsAndUpdatesFleets );
CPPUNIT_TEST( testCombatSelectionDialogsReturnCodesAndState );
CPPUNIT_TEST( testBattleResultsDialogUpdatesShipStatistics );
CPPUNIT_TEST_SUITE_END();

private:
	WXGuiTestHarness m_harness;

public:
	void setUp();
	void tearDown();

	void testMainFrameBuildsExpectedInitialUI();
	void testGamePanelPaintTracksParentSize();
	void testStrategicDialogsCloseModallyWithoutInput();
	void testWXStrategicUIParentBackedModalAndRedrawPaths();
	void testUPFAndSatharFleetDialogsMutateModelState();
	void testTransferShipsDialogMovesShipsAndUpdatesFleets();
	void testCombatSelectionDialogsReturnCodesAndState();
	void testBattleResultsDialogUpdatesShipStatistics();
};

}

#endif
