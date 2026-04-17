/**
 * @file WXStrategicUITest.cpp
 * @brief Implementation file for the WXStrategicUITest class
 */

#include "WXStrategicUITest.h"

#include "gui/WXStrategicUI.h"
#include "strategic/FFleet.h"
#include "strategic/FMap.h"
#include "strategic/FPlayer.h"
#include "strategic/FSystem.h"

#include <vector>

#include <wx/app.h>
#include <wx/defs.h>

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

void WXStrategicUITest::testGuardedDialogMethodsReturnCancelForValidInputsWithoutWxRuntime() {
  // AC: with valid model inputs but no wx runtime, guard paths return wxID_CANCEL and avoid modal construction.
  CPPUNIT_ASSERT(wxTheApp == NULL);

  WXStrategicUI ui(NULL);
  FPlayer player;
  std::vector<unsigned int> playerIDs;
  playerIDs.push_back(player.getID());
  playerIDs.push_back(player.getID() + 1);
  FMap& map = FMap::create(false, playerIDs);
  FSystem system("Test System", 0.0f, 0.0f, 0.0f, player.getID());
  FSystem destination("Test Destination", 1.0f, 0.0f, 0.0f, player.getID());
  FFleet fleet;
  PlayerList players;

  CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL), ui.selectRetreatCondition());
  CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL),
                       ui.runUPFUnattachedSetup(&player, &map));
  CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL),
                       ui.runSatharFleetSetup(&player, &map, true));
  CPPUNIT_ASSERT_EQUAL(static_cast<int>(wxID_CANCEL),
                       ui.selectCombat(&system, FleetList(), FleetList(), &players));

  ui.showSystemDialog(&system, &map, &player);
  ui.showFleetDialog(&fleet, &system, &destination);
}

void WXStrategicUITest::testRequestRedrawIsNullSafe() {
  // AC: redraw requests remain safe when no parent UI window exists.
  WXStrategicUI ui(NULL);
  ui.requestRedraw();
  CPPUNIT_ASSERT(true);
}

}
