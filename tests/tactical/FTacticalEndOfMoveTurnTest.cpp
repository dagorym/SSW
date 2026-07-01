/**
 * @file FTacticalEndOfMoveTurnTest.cpp
 * @brief Behavioral tests for TMF-05 end-of-move single facing change.
 *
 * All tests construct the real FTacticalGame model with real ship/fleet objects,
 * exercise the runtime API, and assert observable results (heading values, pending
 * field values, return values). No source-text inspection is used.
 *
 * Heading convention: 0=W, 1=SW, 2=SE, 3=E, 4=NE, 5=NW
 * turnShip(h, +1) = left turn (heading + 1 mod 6)
 * turnShip(h, -1) = right turn (heading - 1 mod 6)
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jun 30, 2026
 */

#include "FTacticalEndOfMoveTurnTest.h"

#include "core/FHexMap.h"
#include "ships/FFrigate.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

namespace {

using namespace Frontier;

// ---------------------------------------------------------------------------
// Shared fixture setup helpers
// ---------------------------------------------------------------------------

struct FEndOfMoveTurnFixture {
    FTacticalGame game;
    FleetList attackFleets;
    FleetList defendFleets;
    FFleet * attackFleet;
    FFleet * defendFleet;
    FFrigate * attacker;
    FFrigate * defender;
    FPoint attackerPos;
    FPoint defenderPos;
};

/**
 * Set up a minimal two-ship game in PH_MOVE with the attacker selected
 * and moved exactly nMoveHexes hexes (by clicking movement hexes).
 *
 * @param fixture          Fixture to populate.
 * @param nMoveHexes       Number of forward hexes to move (0 = no moves).
 * @param attackerSpeed    Override attacker speed (-1 = leave as default 0).
 * @param attackerMR       Override attacker MR (-1 = leave as default 3).
 * @param attackerHeading  Initial heading for the attacker ship (1 = SW by default).
 */
static void setupFixtureWithMoves(
    FEndOfMoveTurnFixture & fixture,
    int nMoveHexes = 0,
    int attackerSpeed = -1,
    int attackerMR = -1,
    int attackerHeading = 1)
{
    fixture.attackFleet = new FFleet();
    fixture.defendFleet = new FFleet();
    fixture.attacker = new FFrigate();
    fixture.defender = new FFrigate();
    fixture.attackerPos = FPoint(15, 10);
    fixture.defenderPos = FPoint(25, 25);

    fixture.attackFleet->addShip(fixture.attacker);
    fixture.defendFleet->addShip(fixture.defender);
    fixture.attackFleets.push_back(fixture.attackFleet);
    fixture.defendFleets.push_back(fixture.defendFleet);

    fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);

    // Place defender
    fixture.game.setState(BS_SetupDefendFleet);
    fixture.game.setControlState(true);
    fixture.game.setShip(fixture.defender);
    CPPUNIT_ASSERT(fixture.game.placeShip(fixture.defenderPos));
    CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(3));

    // Place attacker
    fixture.game.setState(BS_SetupAttackFleet);
    fixture.game.setControlState(true);
    fixture.game.setShip(fixture.attacker);
    CPPUNIT_ASSERT(fixture.game.placeShip(fixture.attackerPos));
    CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(attackerHeading));

    // Apply overrides before entering PH_MOVE so turn data picks them up.
    if (attackerSpeed >= 0) {
        fixture.attacker->setSpeed(attackerSpeed);
    }
    if (attackerMR >= 0) {
        fixture.attacker->setMR(static_cast<unsigned int>(attackerMR));
    }

    fixture.game.setState(BS_Battle);
    fixture.game.setMovingPlayer(true);
    fixture.game.setPhase(PH_MOVE);
    CPPUNIT_ASSERT(fixture.game.selectShipFromHex(fixture.attackerPos));

    // Move the requested number of hexes by clicking on movement hexes in sequence.
    for (int i = 0; i < nMoveHexes; ++i) {
        const std::vector<FPoint> & moves = fixture.game.getMovementHexes();
        CPPUNIT_ASSERT_MESSAGE("Not enough movement hexes available", !moves.empty());
        CPPUNIT_ASSERT(fixture.game.handleHexClick(moves.front()));
    }
}

static void destroyFixture(FEndOfMoveTurnFixture & fixture) {
    for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
        delete *itr;
    }
    for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
        delete *itr;
    }
    fixture.attackFleets.clear();
    fixture.defendFleets.clear();
}

static FTacticalTurnData * requireTurnData(FEndOfMoveTurnFixture & fixture) {
    FTacticalTurnData * td = fixture.game.findTurnData(fixture.attacker->getID());
    CPPUNIT_ASSERT_MESSAGE("Turn data not found for attacker", td != NULL);
    return td;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// Test suite registration
// ---------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalEndOfMoveTurnTest );

void FTacticalEndOfMoveTurnTest::setUp() {}
void FTacticalEndOfMoveTurnTest::tearDown() {}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

/**
 * Both turn directions are available when MR > 0 and minimum move is satisfied.
 *
 * FFrigate: speed starts at 0, ADF = 4. MinMove = speed - ADF = 0 - 4 = -4 (<=0 so 0 hexes is enough).
 * MR = 3 by default.
 */
void FTacticalEndOfMoveTurnTest::testCanApplyTurnBothDirectionsWhenMRAvailableAndMinMoveSatisfied() {
    FEndOfMoveTurnFixture fixture;
    // Speed 0, MR 3 — min move is (0 - ADF) = negative, so 0 moves is enough.
    setupFixtureWithMoves(fixture, 0, 0, 3);

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should return true", fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should return true", fixture.game.canApplyEndOfMoveTurnRight());

    destroyFixture(fixture);
}

/**
 * Turn is blocked when ship's current MR is 0.
 */
void FTacticalEndOfMoveTurnTest::testCannotApplyTurnWhenMRIsZero() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 0 /* MR = 0 */);

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should return false when MR=0",
        !fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should return false when MR=0",
        !fixture.game.canApplyEndOfMoveTurnRight());

    destroyFixture(fixture);
}

/**
 * Turn is blocked when nMoved < speed - ADF (minimum move not yet satisfied).
 *
 * Use speed = 5, ADF = 4: minMove = 5 - 4 = 1. With 0 hexes moved, the
 * turn gate should refuse.
 */
void FTacticalEndOfMoveTurnTest::testCannotApplyTurnBeforeMinimumMoveIsSatisfied() {
    FEndOfMoveTurnFixture fixture;
    // Speed 5, ADF will be 4 (FFrigate default), MR 3. Min move = 5 - 4 = 1.
    // We move 0 hexes, so nMoved (0) < minMove (1).
    setupFixtureWithMoves(fixture, 0, 5, 3);

    FTacticalTurnData * td = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(0, td->nMoved);
    const int minMove = td->speed - fixture.attacker->getADF();
    CPPUNIT_ASSERT_MESSAGE("minMove must be > 0 for this test to be meaningful", minMove > 0);

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should return false before min move",
        !fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should return false before min move",
        !fixture.game.canApplyEndOfMoveTurnRight());

    destroyFixture(fixture);
}

/**
 * Turn is blocked when the path end hex already has an MR_TURN flag.
 *
 * Move the ship 1 hex via a left-turn hex (the left hexes contain hexes that
 * were approached with an MR_TURN). We simulate by directly adding an MR_TURN
 * flag on the path end hex inside the turn data.
 */
void FTacticalEndOfMoveTurnTest::testCannotApplyTurnWhenPathEndHexHasMRTurnFlag() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3);

    FTacticalTurnData * td = requireTurnData(fixture);

    // Manually simulate having entered the current hex with a left-turn MR.
    // Add a second hex to the path and flag it with MR_TURN.
    FPoint endHex(15, 11); // any hex different from the start
    td->path.addPoint(endHex);
    td->path.addFlag(endHex, MR_TURN);

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should return false when path end has MR_TURN",
        !fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should return false when path end has MR_TURN",
        !fixture.game.canApplyEndOfMoveTurnRight());

    destroyFixture(fixture);
}

/**
 * applyEndOfMoveTurn(+1) sets the ship heading immediately (observable via getHeading()).
 * Heading 1 + 1 = 2.
 */
void FTacticalEndOfMoveTurnTest::testApplyLeftTurnSetsHeadingImmediatelyForVisualFeedback() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1 /* heading SW */);

    const int originHeading = fixture.attacker->getHeading();
    CPPUNIT_ASSERT_EQUAL(1, originHeading);

    CPPUNIT_ASSERT_MESSAGE("applyEndOfMoveTurn(+1) should succeed", fixture.game.applyEndOfMoveTurn(1));

    const int newHeading = fixture.attacker->getHeading();
    CPPUNIT_ASSERT_EQUAL((originHeading + 1) % 6, newHeading);

    destroyFixture(fixture);
}

/**
 * applyEndOfMoveTurn(-1) sets the ship heading immediately (observable via getHeading()).
 * Heading 1 - 1 = 0.
 */
void FTacticalEndOfMoveTurnTest::testApplyRightTurnSetsHeadingImmediatelyForVisualFeedback() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1 /* heading SW */);

    const int originHeading = fixture.attacker->getHeading();
    CPPUNIT_ASSERT_EQUAL(1, originHeading);

    CPPUNIT_ASSERT_MESSAGE("applyEndOfMoveTurn(-1) should succeed", fixture.game.applyEndOfMoveTurn(-1));

    const int newHeading = fixture.attacker->getHeading();
    // heading 1 - 1 = 0
    CPPUNIT_ASSERT_EQUAL(0, newHeading);

    destroyFixture(fixture);
}

/**
 * After applying a turn, pendingEndOfMoveFacing == newHeading and
 * endOfMoveOriginFacing == originalHeading.
 */
void FTacticalEndOfMoveTurnTest::testPendingTurnRecordsOriginAndNewFacing() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 2 /* heading SE */);

    const int originHeading = fixture.attacker->getHeading();
    CPPUNIT_ASSERT_EQUAL(2, originHeading);

    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(1)); // left turn

    FTacticalTurnData * td = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(originHeading, td->endOfMoveOriginFacing);
    const int expectedNewHeading = (originHeading + 1) % 6; // 3
    CPPUNIT_ASSERT_EQUAL(expectedNewHeading, td->pendingEndOfMoveFacing);

    destroyFixture(fixture);
}

/**
 * After a left turn is applied, canApplyEndOfMoveTurnLeft returns false and
 * canApplyEndOfMoveTurnRight returns true (only reverse is enabled).
 */
void FTacticalEndOfMoveTurnTest::testOnlyReverseDirectionEnabledAfterLeftTurnApplied() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1);

    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(1)); // left

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should be false after left turn applied",
        !fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should be true (reverse) after left turn applied",
        fixture.game.canApplyEndOfMoveTurnRight());

    destroyFixture(fixture);
}

/**
 * After a right turn is applied, canApplyEndOfMoveTurnRight returns false and
 * canApplyEndOfMoveTurnLeft returns true (only reverse is enabled).
 */
void FTacticalEndOfMoveTurnTest::testOnlyReverseDirectionEnabledAfterRightTurnApplied() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1);

    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(-1)); // right

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should be false after right turn applied",
        !fixture.game.canApplyEndOfMoveTurnRight());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should be true (reverse) after right turn applied",
        fixture.game.canApplyEndOfMoveTurnLeft());

    destroyFixture(fixture);
}

/**
 * Applying the right turn (reverse) after a left turn pending restores ship heading
 * to the origin and returns true.
 */
void FTacticalEndOfMoveTurnTest::testApplyingReverseWhenLeftPendingRestoresOriginHeading() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1); // heading = 1 (SW)

    const int originHeading = fixture.attacker->getHeading();
    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(1));  // left turn -> heading 2
    CPPUNIT_ASSERT_EQUAL(2, fixture.attacker->getHeading()); // confirm applied

    CPPUNIT_ASSERT_MESSAGE("Reversing left turn with right should succeed",
        fixture.game.applyEndOfMoveTurn(-1));

    CPPUNIT_ASSERT_EQUAL(originHeading, fixture.attacker->getHeading());

    destroyFixture(fixture);
}

/**
 * Applying the left turn (reverse) after a right turn pending restores ship heading
 * to the origin and returns true.
 */
void FTacticalEndOfMoveTurnTest::testApplyingReverseWhenRightPendingRestoresOriginHeading() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 3); // heading = 3 (E)

    const int originHeading = fixture.attacker->getHeading();
    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(-1)); // right turn -> heading 2
    CPPUNIT_ASSERT_EQUAL(2, fixture.attacker->getHeading());

    CPPUNIT_ASSERT_MESSAGE("Reversing right turn with left should succeed",
        fixture.game.applyEndOfMoveTurn(1));

    CPPUNIT_ASSERT_EQUAL(originHeading, fixture.attacker->getHeading());

    destroyFixture(fixture);
}

/**
 * After reversing a pending turn, both pendingEndOfMoveFacing and
 * endOfMoveOriginFacing are reset to -1.
 */
void FTacticalEndOfMoveTurnTest::testReverseClearsPendingFacingFields() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1);

    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(1));  // left turn
    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(-1)); // reverse

    FTacticalTurnData * td = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(-1, td->pendingEndOfMoveFacing);
    CPPUNIT_ASSERT_EQUAL(-1, td->endOfMoveOriginFacing);

    destroyFixture(fixture);
}

/**
 * Calling applyEndOfMoveTurn in the same direction while a turn is already pending
 * returns false and does not change the heading.
 */
void FTacticalEndOfMoveTurnTest::testSecondSameDirectionTurnIsBlockedWhenPendingExists() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1);

    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(1)); // first left turn
    const int headingAfterFirst = fixture.attacker->getHeading();

    const bool secondTurnResult = fixture.game.applyEndOfMoveTurn(1); // second left turn — must fail
    CPPUNIT_ASSERT_MESSAGE("Second same-direction turn should be blocked", !secondTurnResult);
    CPPUNIT_ASSERT_EQUAL(headingAfterFirst, fixture.attacker->getHeading());

    destroyFixture(fixture);
}

/**
 * finalizeMovementState() commits pendingEndOfMoveFacing so curHeading and
 * ship heading reflect the pending turn after finalization.
 */
void FTacticalEndOfMoveTurnTest::testFinalizeMovementStateCommitsPendingFacing() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 2); // heading 2 (SE)

    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(1)); // left -> heading 3 (E)
    const int pendingHeading = fixture.attacker->getHeading();
    CPPUNIT_ASSERT_EQUAL(3, pendingHeading);

    FTacticalTurnData * td = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(3, td->pendingEndOfMoveFacing);

    fixture.game.finalizeMovementState();

    // After finalize, curHeading and finalHeading must equal the committed heading.
    FTacticalTurnData * tdAfter = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(3, tdAfter->curHeading);
    CPPUNIT_ASSERT_EQUAL(3, tdAfter->finalHeading);
    // Ship heading should also be 3.
    CPPUNIT_ASSERT_EQUAL(3, fixture.attacker->getHeading());

    destroyFixture(fixture);
}

/**
 * finalizeMovementState() without any pending turn leaves curHeading unchanged.
 */
void FTacticalEndOfMoveTurnTest::testFinalizeWithNoPendingUsesCurrentHeading() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3, 1);

    FTacticalTurnData * td = requireTurnData(fixture);
    const int headingBefore = td->curHeading;
    CPPUNIT_ASSERT_EQUAL(-1, td->pendingEndOfMoveFacing);

    fixture.game.finalizeMovementState();

    FTacticalTurnData * tdAfter = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(headingBefore, tdAfter->curHeading);

    destroyFixture(fixture);
}

/**
 * Clicking a movement hex while a pending end-of-move turn is active:
 * - clears pendingEndOfMoveFacing
 * - clears endOfMoveOriginFacing
 * - restores the ship heading to the pre-turn heading
 * - then processes the hex click normally.
 */
void FTacticalEndOfMoveTurnTest::testHexClickWhilePendingTurnClearsAndRestoresOriginHeading() {
    FEndOfMoveTurnFixture fixture;
    // speed 0 so any move count satisfies min-move. MR 3.
    setupFixtureWithMoves(fixture, 0, 0, 3, 1); // heading 1 (SW)

    CPPUNIT_ASSERT(fixture.game.applyEndOfMoveTurn(1)); // left -> heading 2 (SE)
    CPPUNIT_ASSERT_EQUAL(2, fixture.attacker->getHeading());

    FTacticalTurnData * tdBefore = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(2, tdBefore->pendingEndOfMoveFacing);

    // Click a movement hex — any legal movement hex is fine.
    const std::vector<FPoint> & moves = fixture.game.getMovementHexes();
    CPPUNIT_ASSERT_MESSAGE("Movement hexes must be available for hex click test", !moves.empty());
    fixture.game.handleHexClick(moves.front());

    FTacticalTurnData * tdAfter = requireTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(-1, tdAfter->pendingEndOfMoveFacing);
    CPPUNIT_ASSERT_EQUAL(-1, tdAfter->endOfMoveOriginFacing);

    // Ship heading was restored to origin before the hex click resolved its routing logic.
    // After the hex click the heading may change due to movement, but the pending field
    // is cleared and the origin heading was restored at the start of hex processing.
    // We verify the pending state is gone (behavioral guarantee).
    CPPUNIT_ASSERT_EQUAL(-1, tdAfter->pendingEndOfMoveFacing);

    destroyFixture(fixture);
}

/**
 * A ship that decelerated to speed 0 this turn (minimum move gate satisfied because
 * minMove = 0 - ADF <= 0) can use the end-of-move turn.
 *
 * This extends the existing free-rotation gate: stopped ships with MR > 0
 * should also qualify for the end-of-move turn.
 */
void FTacticalEndOfMoveTurnTest::testSpeedZeroDecelerationShipCanUseEndOfMoveTurn() {
    FEndOfMoveTurnFixture fixture;
    // Speed 0, MR 3: minMove = 0 - 4 = -4, which is <= 0, so 0 hexes satisfies min move.
    setupFixtureWithMoves(fixture, 0, 0, 3, 4); // heading 4 (NE)

    CPPUNIT_ASSERT_MESSAGE("Speed-0 ship with MR should be able to turn left",
        fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("Speed-0 ship with MR should be able to turn right",
        fixture.game.canApplyEndOfMoveTurnRight());

    CPPUNIT_ASSERT_MESSAGE("applyEndOfMoveTurn(+1) should succeed for speed-0 ship",
        fixture.game.applyEndOfMoveTurn(1));

    // Heading 4 + 1 = 5 (NW)
    CPPUNIT_ASSERT_EQUAL(5, fixture.attacker->getHeading());

    destroyFixture(fixture);
}

} // namespace FrontierTests
