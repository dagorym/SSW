/**
 * @file WXStrategicUITest.cpp
 * @brief Implementation file for the WXStrategicUITest class
 */

#include "WXStrategicUITest.h"

#include "gui/WXStrategicUI.h"

namespace FrontierTests {
using namespace Frontier;

CPPUNIT_TEST_SUITE_REGISTRATION( WXStrategicUITest );

void WXStrategicUITest::setUp() {
}

void WXStrategicUITest::tearDown() {
}

void WXStrategicUITest::testGuardedDialogMethodsReturnNonZeroWhenNoUIContext() {
// AC: dialog/cancel-style API methods must return non-zero when UI context is unavailable.
WXStrategicUI ui(NULL);

CPPUNIT_ASSERT(ui.selectRetreatCondition() != 0);
CPPUNIT_ASSERT(ui.runUPFUnattachedSetup(NULL, NULL) != 0);
CPPUNIT_ASSERT(ui.runSatharFleetSetup(NULL, NULL, true) != 0);

FleetList defenders;
FleetList attackers;
CPPUNIT_ASSERT(ui.selectCombat(NULL, defenders, attackers, NULL) != 0);
}

void WXStrategicUITest::testRequestRedrawIsNullSafe() {
// AC: redraw requests remain safe when no parent UI window exists.
WXStrategicUI ui(NULL);
ui.requestRedraw();
CPPUNIT_ASSERT(true);
}

}
