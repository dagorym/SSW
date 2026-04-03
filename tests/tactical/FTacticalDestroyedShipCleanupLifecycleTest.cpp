/**
 * @file FTacticalDestroyedShipCleanupLifecycleTest.cpp
 * @brief Implementation file for the FTacticalDestroyedShipCleanupLifecycleTest class
 */

#include "FTacticalDestroyedShipCleanupLifecycleTest.h"

#include <string>
#include <vector>

#include "ships/FVehicle.h"

namespace Frontier {

struct FDestroyedShipCleanupLifecycle {
const std::vector<unsigned int> * destroyedShipIDs;
FVehicle * selectedShip;
void (*clearSelectedShip)(void *);
void (*reDraw)(void *);
void (*clearDestroyedShipBookkeeping)(void *);
bool (*hasWinner)(void *);
void (*declareWinner)(void *);
void * context;
};

void runDestroyedShipCleanupLifecycle(const FDestroyedShipCleanupLifecycle & lifecycle);

}

namespace FrontierTests {

namespace {

using namespace Frontier;

class FCleanupVehicleHarness : public FVehicle {
public:
void forceID(unsigned int id) {
m_ID = id;
}
};

struct FCleanupLifecycleState {
bool selectedShipCleared;
unsigned int redrawCount;
unsigned int clearBookkeepingCount;
unsigned int hasWinnerCount;
unsigned int declareWinnerCount;
bool hasWinnerResult;
std::vector<std::string> callSequence;
};

void clearSelectedShip(void * context) {
FCleanupLifecycleState * state = static_cast<FCleanupLifecycleState *>(context);
state->selectedShipCleared = true;
state->callSequence.push_back("clearSelectedShip");
}

void redraw(void * context) {
FCleanupLifecycleState * state = static_cast<FCleanupLifecycleState *>(context);
state->redrawCount++;
state->callSequence.push_back("reDraw");
}

void clearBookkeeping(void * context) {
FCleanupLifecycleState * state = static_cast<FCleanupLifecycleState *>(context);
state->clearBookkeepingCount++;
state->callSequence.push_back("clearBookkeeping");
}

bool hasWinner(void * context) {
FCleanupLifecycleState * state = static_cast<FCleanupLifecycleState *>(context);
state->hasWinnerCount++;
state->callSequence.push_back("hasWinner");
return state->hasWinnerResult;
}

void declareWinner(void * context) {
FCleanupLifecycleState * state = static_cast<FCleanupLifecycleState *>(context);
state->declareWinnerCount++;
state->callSequence.push_back("declareWinner");
}

unsigned int findCallIndex(const std::vector<std::string> & calls, const std::string & callName) {
for (unsigned int i = 0; i < static_cast<unsigned int>(calls.size()); ++i) {
if (calls[i] == callName) {
return i;
}
}
return static_cast<unsigned int>(calls.size());
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalDestroyedShipCleanupLifecycleTest );

void FTacticalDestroyedShipCleanupLifecycleTest::setUp() {
}

void FTacticalDestroyedShipCleanupLifecycleTest::tearDown() {
}

void FTacticalDestroyedShipCleanupLifecycleTest::testClearsSelectedShipAndRedrawsWhenSelectedShipDestroyed() {
FCleanupVehicleHarness selectedShip;
selectedShip.forceID(99);

std::vector<unsigned int> destroyedShipIDs;
destroyedShipIDs.push_back(12);
destroyedShipIDs.push_back(99);

FCleanupLifecycleState state;
state.selectedShipCleared = false;
state.redrawCount = 0;
state.clearBookkeepingCount = 0;
state.hasWinnerCount = 0;
state.declareWinnerCount = 0;
state.hasWinnerResult = false;

Frontier::FDestroyedShipCleanupLifecycle lifecycle;
lifecycle.destroyedShipIDs = &destroyedShipIDs;
lifecycle.selectedShip = &selectedShip;
lifecycle.clearSelectedShip = clearSelectedShip;
lifecycle.reDraw = redraw;
lifecycle.clearDestroyedShipBookkeeping = clearBookkeeping;
lifecycle.hasWinner = hasWinner;
lifecycle.declareWinner = declareWinner;
lifecycle.context = &state;

Frontier::runDestroyedShipCleanupLifecycle(lifecycle);

CPPUNIT_ASSERT(state.selectedShipCleared);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.redrawCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.clearBookkeepingCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.hasWinnerCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), state.declareWinnerCount);
}

void FTacticalDestroyedShipCleanupLifecycleTest::testBookkeepingClearedExactlyOncePerCleanup() {
std::vector<unsigned int> destroyedShipIDs;

FCleanupLifecycleState state;
state.selectedShipCleared = false;
state.redrawCount = 0;
state.clearBookkeepingCount = 0;
state.hasWinnerCount = 0;
state.declareWinnerCount = 0;
state.hasWinnerResult = false;

Frontier::FDestroyedShipCleanupLifecycle lifecycle;
lifecycle.destroyedShipIDs = &destroyedShipIDs;
lifecycle.selectedShip = NULL;
lifecycle.clearSelectedShip = clearSelectedShip;
lifecycle.reDraw = redraw;
lifecycle.clearDestroyedShipBookkeeping = clearBookkeeping;
lifecycle.hasWinner = hasWinner;
lifecycle.declareWinner = declareWinner;
lifecycle.context = &state;

Frontier::runDestroyedShipCleanupLifecycle(lifecycle);

CPPUNIT_ASSERT(!state.selectedShipCleared);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), state.redrawCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.clearBookkeepingCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.hasWinnerCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), state.declareWinnerCount);
}

void FTacticalDestroyedShipCleanupLifecycleTest::testWinnerHandlingRunsAfterBookkeepingClear() {
FCleanupVehicleHarness selectedShip;
selectedShip.forceID(7);

std::vector<unsigned int> destroyedShipIDs;
destroyedShipIDs.push_back(7);

FCleanupLifecycleState state;
state.selectedShipCleared = false;
state.redrawCount = 0;
state.clearBookkeepingCount = 0;
state.hasWinnerCount = 0;
state.declareWinnerCount = 0;
state.hasWinnerResult = true;

Frontier::FDestroyedShipCleanupLifecycle lifecycle;
lifecycle.destroyedShipIDs = &destroyedShipIDs;
lifecycle.selectedShip = &selectedShip;
lifecycle.clearSelectedShip = clearSelectedShip;
lifecycle.reDraw = redraw;
lifecycle.clearDestroyedShipBookkeeping = clearBookkeeping;
lifecycle.hasWinner = hasWinner;
lifecycle.declareWinner = declareWinner;
lifecycle.context = &state;

Frontier::runDestroyedShipCleanupLifecycle(lifecycle);

CPPUNIT_ASSERT(state.selectedShipCleared);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.redrawCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.clearBookkeepingCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.hasWinnerCount);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), state.declareWinnerCount);

const unsigned int bookkeepingIndex = findCallIndex(state.callSequence, "clearBookkeeping");
const unsigned int hasWinnerIndex = findCallIndex(state.callSequence, "hasWinner");
const unsigned int declareWinnerIndex = findCallIndex(state.callSequence, "declareWinner");
CPPUNIT_ASSERT(bookkeepingIndex < hasWinnerIndex);
CPPUNIT_ASSERT(hasWinnerIndex < declareWinnerIndex);
}

}
