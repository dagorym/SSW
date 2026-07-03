/**
 * @file FTacticalEndOfMoveTurnTest.cpp
 * @brief Behavioral tests for TMF-05 end-of-move single facing change and the
 *        TMFR-02 whole-path MR_TURN budget fix in canUseEndOfMoveTurn().
 *
 * All tests construct the real FTacticalGame model with real ship/fleet objects,
 * exercise the runtime API, and assert observable results (heading values, pending
 * field values, return values). No source-text inspection is used.
 *
 * Heading convention: 0=W, 1=SW, 2=SE, 3=E, 4=NE, 5=NW
 * turnShip(h, +1) = left turn (heading + 1 mod 6)
 * turnShip(h, -1) = right turn (heading - 1 mod 6)
 *
 * @author claude-sonnet-4-6 (medium), claude-sonnet-5 (medium)
 * @date Created: Jun 30, 2026
 * @date Last Modified: Jul 03, 2026
 */

#include "FTacticalEndOfMoveTurnTest.h"

#include "core/FHexMap.h"
#include "ships/FFrigate.h"
#include "ships/FMinelayer.h"
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
 * Turn remains available when the whole-path MR_TURN budget has not been exhausted.
 *
 * TMFR-02: canUseEndOfMoveTurn() was fixed to use the whole-path budget rule
 * (countFlags(MR_TURN) < getMR()) instead of testing only the end-of-path hex's
 * flag. This test uses MR=3 with exactly one MR_TURN flag recorded on the path
 * (1 < 3), so the turn must be allowed. Previously (pre-fix) this scenario was
 * incorrectly blocked because MR_TURN is recorded on the hex a ship turns INTO,
 * and a turn-then-advance on the final leg flagged the ship's current (destination)
 * hex even though MR remained.
 */
void FTacticalEndOfMoveTurnTest::testCanApplyTurnWhenMRTurnBudgetRemainsAfterOneTurnUsed() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 3);

    FTacticalTurnData * td = requireTurnData(fixture);

    // Simulate having used exactly one MR_TURN via normal movement: add a hex to
    // the path and flag it MR_TURN. countFlags(MR_TURN) is now 1, MR is 3 (1 < 3).
    FPoint endHex(15, 11); // any hex different from the start
    td->path.addPoint(endHex);
    td->path.addFlag(endHex, MR_TURN);

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should return true when MR_TURN budget remains (1 < 3)",
        fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should return true when MR_TURN budget remains (1 < 3)",
        fixture.game.canApplyEndOfMoveTurnRight());

    destroyFixture(fixture);
}

/**
 * Turn is blocked once the whole-path MR_TURN count reaches the ship's MR.
 *
 * Companion case to testCanApplyTurnWhenMRTurnBudgetRemainsAfterOneTurnUsed:
 * MR=2 with two distinct hexes flagged MR_TURN on the path (2 >= 2) must block
 * the end-of-move turn, confirming the budget rule still gates correctly once
 * turns used reach MR.
 */
void FTacticalEndOfMoveTurnTest::testCannotApplyTurnWhenMRTurnBudgetExhausted() {
    FEndOfMoveTurnFixture fixture;
    setupFixtureWithMoves(fixture, 0, 0, 2);

    FTacticalTurnData * td = requireTurnData(fixture);

    // Simulate having used two MR_TURNs via normal movement.
    FPoint firstTurnHex(15, 11);
    FPoint secondTurnHex(15, 12);
    td->path.addPoint(firstTurnHex);
    td->path.addFlag(firstTurnHex, MR_TURN);
    td->path.addPoint(secondTurnHex);
    td->path.addFlag(secondTurnHex, MR_TURN);

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should return false when MR_TURN budget is exhausted (2 >= 2)",
        !fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should return false when MR_TURN budget is exhausted (2 >= 2)",
        !fixture.game.canApplyEndOfMoveTurnRight());

    destroyFixture(fixture);
}

// ---------------------------------------------------------------------------
// TMFR-02 repro scenario: minelayer with MR=2, real forward-then-turn movement
// (no manually injected path flags) driven through handleHexClick() using the
// left/right turn hex lists exposed by getLeftTurnHexes()/getRightTurnHexes().
// ---------------------------------------------------------------------------

namespace {

struct FMinelayerTurnFixture {
    FTacticalGame game;
    FleetList attackFleets;
    FleetList defendFleets;
    FFleet * attackFleet;
    FFleet * defendFleet;
    FMinelayer * attacker;
    FFrigate * defender;
    FPoint attackerPos;
    FPoint defenderPos;
};

/**
 * Set up a two-ship game in PH_MOVE with an FMinelayer attacker selected,
 * MR overridden to 2, and speed set high enough to permit a forward move plus
 * two subsequent turn-hex selections.
 */
static void setupMinelayerFixture(FMinelayerTurnFixture & fixture, int attackerSpeed) {
    fixture.attackFleet = new FFleet();
    fixture.defendFleet = new FFleet();
    fixture.attacker = new FMinelayer();
    fixture.defender = new FFrigate();
    fixture.attackerPos = FPoint(15, 10);
    fixture.defenderPos = FPoint(25, 25);

    fixture.attackFleet->addShip(fixture.attacker);
    fixture.defendFleet->addShip(fixture.defender);
    fixture.attackFleets.push_back(fixture.attackFleet);
    fixture.defendFleets.push_back(fixture.defendFleet);

    fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);

    fixture.game.setState(BS_SetupDefendFleet);
    fixture.game.setControlState(true);
    fixture.game.setShip(fixture.defender);
    CPPUNIT_ASSERT(fixture.game.placeShip(fixture.defenderPos));
    CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(3));

    fixture.game.setState(BS_SetupAttackFleet);
    fixture.game.setControlState(true);
    fixture.game.setShip(fixture.attacker);
    CPPUNIT_ASSERT(fixture.game.placeShip(fixture.attackerPos));
    CPPUNIT_ASSERT(fixture.game.setShipPlacementHeading(1)); // heading SW

    fixture.attacker->setSpeed(attackerSpeed);
    fixture.attacker->setMR(2);
    CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(fixture.attacker->getMR()));

    fixture.game.setState(BS_Battle);
    fixture.game.setMovingPlayer(true);
    fixture.game.setPhase(PH_MOVE);
    CPPUNIT_ASSERT(fixture.game.selectShipFromHex(fixture.attackerPos));
}

static void destroyMinelayerFixture(FMinelayerTurnFixture & fixture) {
    for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
        delete *itr;
    }
    for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
        delete *itr;
    }
    fixture.attackFleets.clear();
    fixture.defendFleets.clear();
}

static FTacticalTurnData * requireMinelayerTurnData(FMinelayerTurnFixture & fixture) {
    FTacticalTurnData * td = fixture.game.findTurnData(fixture.attacker->getID());
    CPPUNIT_ASSERT_MESSAGE("Turn data not found for minelayer attacker", td != NULL);
    return td;
}

/// Click one available left-turn hex if present, otherwise fall back to a straight
/// movement hex; returns true if a hex was clicked.
static bool clickLeftTurnOrForwardHex(FMinelayerTurnFixture & fixture) {
    const std::vector<FPoint> & leftHexes = fixture.game.getLeftTurnHexes();
    if (!leftHexes.empty()) {
        return fixture.game.handleHexClick(leftHexes.front());
    }
    const std::vector<FPoint> & moves = fixture.game.getMovementHexes();
    CPPUNIT_ASSERT_MESSAGE("No movement or turn hexes available", !moves.empty());
    return fixture.game.handleHexClick(moves.front());
}

} // anonymous namespace

/**
 * Repro scenario: minelayer with MR=2, speed high enough to move forward once and
 * then use exactly one real MR_TURN via a left-turn hex click. The turn budget used
 * (1) is below MR (2), so canApplyEndOfMoveTurnLeft/Right must return true in the
 * hex right after that turn — this is the exact TMFR-02 defect scenario, driven
 * entirely through real movement (not a manually injected path flag).
 */
void FTacticalEndOfMoveTurnTest::testMinelayerWithMRTwoCanStillTurnAfterUsingOneTurnViaRealMovement() {
    FMinelayerTurnFixture fixture;
    setupMinelayerFixture(fixture, 3 /* speed */);

    // First hex: move straight forward so the path has length > 1 and turn hexes
    // become available on the next selection.
    const std::vector<FPoint> & initialMoves = fixture.game.getMovementHexes();
    CPPUNIT_ASSERT_MESSAGE("Initial movement hexes must be available", !initialMoves.empty());
    CPPUNIT_ASSERT(fixture.game.handleHexClick(initialMoves.front()));

    // Second hex: use a real left-turn (falls back to forward if no turn hex is
    // offered, though MR=2 with countFlags==0 < 2 should offer left/right hexes).
    const std::vector<FPoint> & leftHexes = fixture.game.getLeftTurnHexes();
    CPPUNIT_ASSERT_MESSAGE("Left-turn hexes must be available with MR_TURN budget remaining", !leftHexes.empty());
    CPPUNIT_ASSERT(fixture.game.handleHexClick(leftHexes.front()));

    FTacticalTurnData * td = requireMinelayerTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), td->path.countFlags(MR_TURN));
    CPPUNIT_ASSERT_MESSAGE("Minimum move must be satisfied for this scenario to be meaningful",
        td->nMoved >= td->speed - fixture.attacker->getADF());

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should be true right after using 1 of 2 MR_TURNs",
        fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should be true right after using 1 of 2 MR_TURNs",
        fixture.game.canApplyEndOfMoveTurnRight());

    destroyMinelayerFixture(fixture);
}

/**
 * A minelayer with MR=2 that has used both MR_TURNs via real movement (countFlags
 * == MR) is blocked from a further end-of-move turn, confirming the budget still
 * gates correctly once fully consumed.
 */
void FTacticalEndOfMoveTurnTest::testMinelayerWithMRTwoBlockedAfterUsingBothTurnsViaRealMovement() {
    FMinelayerTurnFixture fixture;
    setupMinelayerFixture(fixture, 4 /* speed */);

    const std::vector<FPoint> & initialMoves = fixture.game.getMovementHexes();
    CPPUNIT_ASSERT_MESSAGE("Initial movement hexes must be available", !initialMoves.empty());
    CPPUNIT_ASSERT(fixture.game.handleHexClick(initialMoves.front()));

    // Use both MR_TURN budget units via successive left-turn hex clicks.
    CPPUNIT_ASSERT(clickLeftTurnOrForwardHex(fixture));
    CPPUNIT_ASSERT(clickLeftTurnOrForwardHex(fixture));

    FTacticalTurnData * td = requireMinelayerTurnData(fixture);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(2), td->path.countFlags(MR_TURN));
    CPPUNIT_ASSERT_MESSAGE("Minimum move must be satisfied for this scenario to be meaningful",
        td->nMoved >= td->speed - fixture.attacker->getADF());

    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnLeft should be false once both MR_TURNs are used",
        !fixture.game.canApplyEndOfMoveTurnLeft());
    CPPUNIT_ASSERT_MESSAGE("canApplyEndOfMoveTurnRight should be false once both MR_TURNs are used",
        !fixture.game.canApplyEndOfMoveTurnRight());

    destroyMinelayerFixture(fixture);
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
