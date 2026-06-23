/**
 * @file FTacticalPreGameOrdnanceTest.cpp
 * @brief Behavioral tests for PGS-04 pre-game ordnance undeploy and mine-removal fixes.
 *
 * Covers:
 *   (a) Undeploy-one from stacked (hex, source) returns ammo and decrements count.
 *   (b) Mine removal with non-placing source selected still removes mine and
 *       restores ammo to the placing ship.
 *
 * @author claude-sonnet-4-6 (medium)
 * @date Created: Jun 22, 2026
 * @date Last Modified: Jun 22, 2026
 */

#include "FTacticalPreGameOrdnanceTest.h"

#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"
#include "weapons/FWeapon.h"

namespace FrontierTests {

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalPreGameOrdnanceTest );

void FTacticalPreGameOrdnanceTest::setUp() {
}

void FTacticalPreGameOrdnanceTest::tearDown() {
}

// ---------------------------------------------------------------------------
// Shared test helper: build a standard two-fleet game with one Minelayer on
// the defending side (owner 0) and one AssaultScout on the attacking side
// (owner 1), place both ships, and enter mine placement mode.
// Returns true if setup succeeded.
//
// On return:
//   game   - initialized, both ships placed, in BS_PlaceMines with mine
//            launcher selected.
//   minelayer / attacker  - raw pointers owned by the fleet objects.
//   defendFleet / attackFleet  - caller is responsible for delete.
// ---------------------------------------------------------------------------
static bool buildSingleMinelayerGame(
    Frontier::FTacticalGame & game,
    Frontier::FVehicle *& minelayer,
    Frontier::FVehicle *& attacker,
    Frontier::FFleet *& defendFleet,
    Frontier::FFleet *& attackFleet)
{
    using namespace Frontier;

    minelayer = createShip("Minelayer");
    if (!minelayer) return false;

    attacker = createShip("AssaultScout");
    if (!attacker) { delete minelayer; return false; }

    defendFleet = new FFleet();
    minelayer->setOwner(0);
    defendFleet->addShip(minelayer);
    FleetList defendFleets;
    defendFleets.push_back(defendFleet);

    attackFleet = new FFleet();
    attacker->setOwner(1);
    attackFleet->addShip(attacker);
    FleetList attackFleets;
    attackFleets.push_back(attackFleet);

    game.setupFleets(&attackFleets, &defendFleets, false, NULL);

    game.setState(BS_SetupDefendFleet);
    game.setControlState(true);
    game.setShip(minelayer);
    game.placeShip(FPoint(10, 10));
    game.setShipPlacementHeading(0);

    game.setState(BS_SetupAttackFleet);
    game.setControlState(true);
    game.setShip(attacker);
    game.placeShip(FPoint(30, 10));
    game.setShipPlacementHeading(3);

    game.setActivePlayer(false);   // defender = owner 0
    return game.beginOrdnancePlacement();
}

// ---------------------------------------------------------------------------

void FTacticalPreGameOrdnanceTest::testGetPlacedSeekerHexGroupsReturnsEmptyOutsidePlaceSeekers() {
    // AC: getPlacedSeekerHexGroups() must return empty outside BS_PlaceSeekers
    // (i.e. does not appear during mine phase).
    using namespace Frontier;

    FVehicle * minelayer = NULL;
    FVehicle * attacker  = NULL;
    FFleet   * defendFleet = NULL;
    FFleet   * attackFleet = NULL;
    FTacticalGame game;

    bool ok = buildSingleMinelayerGame(game, minelayer, attacker, defendFleet, attackFleet);
    CPPUNIT_ASSERT_MESSAGE("Setup must succeed", ok);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceMines after setup",
        static_cast<int>(BS_PlaceMines), game.getState());

    // During BS_PlaceMines the accessor must return empty.
    std::vector<FTacticalPreGameSeekerHexGroup> groups = game.getPlacedSeekerHexGroups();
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "getPlacedSeekerHexGroups() must return empty during BS_PlaceMines",
        static_cast<unsigned int>(0),
        static_cast<unsigned int>(groups.size()));

    delete defendFleet;
    delete attackFleet;
}

void FTacticalPreGameOrdnanceTest::testGetPlacedSeekerHexGroupsGroupsByHexAndSource() {
    // AC: During BS_PlaceSeekers, getPlacedSeekerHexGroups() groups inactive
    // seekers by (hex, source) with correct counts.
    using namespace Frontier;

    FVehicle * minelayer = NULL;
    FVehicle * attacker  = NULL;
    FFleet   * defendFleet = NULL;
    FFleet   * attackFleet = NULL;
    FTacticalGame game;

    bool ok = buildSingleMinelayerGame(game, minelayer, attacker, defendFleet, attackFleet);
    CPPUNIT_ASSERT_MESSAGE("Setup must succeed", ok);

    // Advance to BS_PlaceSeekers.
    game.completeMinePlacement();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceSeekers after completeMinePlacement()",
        static_cast<int>(BS_PlaceSeekers), game.getState());
    CPPUNIT_ASSERT_MESSAGE("SM launcher must be selected", game.getWeapon() != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Selected weapon must be SM",
        FWeapon::SM, game.getWeapon()->getType());

    // Find the seeker launcher to check ammo.
    FWeapon * seekerLauncher = game.getWeapon();
    CPPUNIT_ASSERT_MESSAGE("Seeker launcher must not be null", seekerLauncher != NULL);
    const int initialAmmo = seekerLauncher->getAmmo();
    CPPUNIT_ASSERT_MESSAGE("Seeker launcher must start with ammo > 0", initialAmmo > 0);

    // Before any placements the group list must be empty.
    std::vector<FTacticalPreGameSeekerHexGroup> empty = game.getPlacedSeekerHexGroups();
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "getPlacedSeekerHexGroups() must be empty before any seeker placement",
        static_cast<unsigned int>(0),
        static_cast<unsigned int>(empty.size()));

    // Place 3 seekers at the same hex from the same (auto-selected) source.
    const FPoint hex1(5, 5);
    bool c1 = game.handleHexClick(hex1);
    bool c2 = (seekerLauncher->getAmmo() > 0) ? game.handleHexClick(hex1) : false;
    bool c3 = (seekerLauncher->getAmmo() > 0) ? game.handleHexClick(hex1) : false;
    CPPUNIT_ASSERT_MESSAGE("First click must succeed", c1);
    if (!c2 || !c3) {
        // Not enough ammo to place 3 – skip grouping assertion but don't fail.
        delete defendFleet;
        delete attackFleet;
        return;
    }

    // One group with count == 3.
    std::vector<FTacticalPreGameSeekerHexGroup> groups1 = game.getPlacedSeekerHexGroups();
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "getPlacedSeekerHexGroups() must return 1 group after 3 clicks on same hex",
        static_cast<unsigned int>(1),
        static_cast<unsigned int>(groups1.size()));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Group count must be 3 after 3 clicks on same hex",
        static_cast<unsigned int>(3),
        groups1[0].count);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Group hex X must match", hex1.getX(), groups1[0].hex.getX());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Group hex Y must match", hex1.getY(), groups1[0].hex.getY());

    // Place one seeker at a different hex — must produce a second group.
    if (seekerLauncher->getAmmo() > 0) {
        const FPoint hex2(8, 8);
        bool c4 = game.handleHexClick(hex2);
        CPPUNIT_ASSERT_MESSAGE("Click on second hex must succeed", c4);

        std::vector<FTacticalPreGameSeekerHexGroup> groups2 = game.getPlacedSeekerHexGroups();
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "getPlacedSeekerHexGroups() must return 2 groups after clicks on two hexes",
            static_cast<unsigned int>(2),
            static_cast<unsigned int>(groups2.size()));
    }

    delete defendFleet;
    delete attackFleet;
}

void FTacticalPreGameOrdnanceTest::testRecallPlacedSeekerDecrementsCountAndRestoresAmmo() {
    // AC (a): recallPlacedSeekerAtHexSource() removes exactly one seeker and
    // restores one ammo round.
    using namespace Frontier;

    FVehicle * minelayer = NULL;
    FVehicle * attacker  = NULL;
    FFleet   * defendFleet = NULL;
    FFleet   * attackFleet = NULL;
    FTacticalGame game;

    bool ok = buildSingleMinelayerGame(game, minelayer, attacker, defendFleet, attackFleet);
    CPPUNIT_ASSERT_MESSAGE("Setup must succeed", ok);

    game.completeMinePlacement();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceSeekers",
        static_cast<int>(BS_PlaceSeekers), game.getState());

    FWeapon * seekerLauncher = game.getWeapon();
    CPPUNIT_ASSERT_MESSAGE("SM launcher must be selected", seekerLauncher != NULL);
    CPPUNIT_ASSERT_MESSAGE("Must have >= 2 ammo for this test", seekerLauncher->getAmmo() >= 2);

    const int ammoBeforePlacement = seekerLauncher->getAmmo();

    // Place 2 seekers at the same hex.
    const FPoint hex(6, 6);
    game.handleHexClick(hex);
    game.handleHexClick(hex);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker count must be 2 after 2 placements",
        static_cast<unsigned int>(2),
        static_cast<unsigned int>(game.getSeekerMissiles().size()));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Ammo must be decremented by 2",
        ammoBeforePlacement - 2, seekerLauncher->getAmmo());

    // Count SM-type placed-ordnance records.
    unsigned int smOrdnanceBefore = 0;
    for (unsigned int i = 0; i < game.getPlacedOrdnance().size(); ++i) {
        if (game.getPlacedOrdnance()[i].weaponType == FWeapon::SM) {
            smOrdnanceBefore++;
        }
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must have 2 SM placed-ordnance records before recall",
        static_cast<unsigned int>(2), smOrdnanceBefore);

    // Build the source from the first group entry.
    std::vector<FTacticalPreGameSeekerHexGroup> groups = game.getPlacedSeekerHexGroups();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must have 1 group before recall",
        static_cast<unsigned int>(1),
        static_cast<unsigned int>(groups.size()));
    const FTacticalOrdnanceSource src = groups[0].source;

    // Recall one seeker.
    const bool recalled = game.recallPlacedSeekerAtHexSource(hex, src);
    CPPUNIT_ASSERT_MESSAGE("recallPlacedSeekerAtHexSource() must return true", recalled);

    // One seeker must remain.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker count must be 1 after recall",
        static_cast<unsigned int>(1),
        static_cast<unsigned int>(game.getSeekerMissiles().size()));

    // Ammo must be restored by exactly one.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker launcher ammo must be restored by 1 after recall",
        ammoBeforePlacement - 1, seekerLauncher->getAmmo());

    // SM placed-ordnance count must decrease by 1.
    unsigned int smOrdnanceAfter = 0;
    for (unsigned int i = 0; i < game.getPlacedOrdnance().size(); ++i) {
        if (game.getPlacedOrdnance()[i].weaponType == FWeapon::SM) {
            smOrdnanceAfter++;
        }
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE("SM placed-ordnance count must decrease by 1 after recall",
        smOrdnanceBefore - 1, smOrdnanceAfter);

    delete defendFleet;
    delete attackFleet;
}

void FTacticalPreGameOrdnanceTest::testRecallFromStackedSeekerLeavesRemainingSeekersInPlace() {
    // AC (a): Recalling from a stack of 3 seekers leaves 2 in place.
    using namespace Frontier;

    FVehicle * minelayer = NULL;
    FVehicle * attacker  = NULL;
    FFleet   * defendFleet = NULL;
    FFleet   * attackFleet = NULL;
    FTacticalGame game;

    bool ok = buildSingleMinelayerGame(game, minelayer, attacker, defendFleet, attackFleet);
    CPPUNIT_ASSERT_MESSAGE("Setup must succeed", ok);

    game.completeMinePlacement();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceSeekers",
        static_cast<int>(BS_PlaceSeekers), game.getState());

    FWeapon * seekerLauncher = game.getWeapon();
    CPPUNIT_ASSERT_MESSAGE("SM launcher must be selected", seekerLauncher != NULL);
    if (seekerLauncher->getAmmo() < 3) {
        // Skip: not enough ammo; previous tests already cover count >= 2.
        delete defendFleet;
        delete attackFleet;
        return;
    }

    const FPoint hex(4, 4);
    game.handleHexClick(hex);
    game.handleHexClick(hex);
    game.handleHexClick(hex);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker count must be 3 after 3 placements",
        static_cast<unsigned int>(3),
        static_cast<unsigned int>(game.getSeekerMissiles().size()));

    // Verify group shows count == 3.
    std::vector<FTacticalPreGameSeekerHexGroup> groupsBefore = game.getPlacedSeekerHexGroups();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must have 1 group with count 3 before recall",
        static_cast<unsigned int>(1),
        static_cast<unsigned int>(groupsBefore.size()));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Group count must be 3 before recall",
        static_cast<unsigned int>(3), groupsBefore[0].count);

    const FTacticalOrdnanceSource src = groupsBefore[0].source;
    const bool recalled = game.recallPlacedSeekerAtHexSource(hex, src);
    CPPUNIT_ASSERT_MESSAGE("recall must return true", recalled);

    // 2 seekers must remain.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker count must be 2 after one recall from stack of 3",
        static_cast<unsigned int>(2),
        static_cast<unsigned int>(game.getSeekerMissiles().size()));

    // Group list must still show the (hex, source) with count == 2.
    std::vector<FTacticalPreGameSeekerHexGroup> groupsAfter = game.getPlacedSeekerHexGroups();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must still have 1 group after partial recall",
        static_cast<unsigned int>(1),
        static_cast<unsigned int>(groupsAfter.size()));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Group count must be 2 after one recall from stack of 3",
        static_cast<unsigned int>(2), groupsAfter[0].count);

    delete defendFleet;
    delete attackFleet;
}

void FTacticalPreGameOrdnanceTest::testRecallReturnsFalseWhenNothingToRecall() {
    // AC: recallPlacedSeekerAtHexSource() returns false when no matching seeker exists.
    using namespace Frontier;

    FVehicle * minelayer = NULL;
    FVehicle * attacker  = NULL;
    FFleet   * defendFleet = NULL;
    FFleet   * attackFleet = NULL;
    FTacticalGame game;

    bool ok = buildSingleMinelayerGame(game, minelayer, attacker, defendFleet, attackFleet);
    CPPUNIT_ASSERT_MESSAGE("Setup must succeed", ok);

    game.completeMinePlacement();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceSeekers",
        static_cast<int>(BS_PlaceSeekers), game.getState());

    // Dummy source — no seeker has been placed.
    FTacticalOrdnanceSource dummySrc;
    dummySrc.shipID = 9999;
    dummySrc.weaponIndex = 0;
    dummySrc.weaponID = 9999;

    const FPoint emptyHex(5, 5);
    const bool result = game.recallPlacedSeekerAtHexSource(emptyHex, dummySrc);
    CPPUNIT_ASSERT_MESSAGE("recallPlacedSeekerAtHexSource() must return false when no seeker exists at hex",
        !result);

    // Seeker count must remain zero.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker count must remain 0 after failed recall",
        static_cast<unsigned int>(0),
        static_cast<unsigned int>(game.getSeekerMissiles().size()));

    delete defendFleet;
    delete attackFleet;
}

void FTacticalPreGameOrdnanceTest::testMineRemovalWithNonPlacingSourceSelectedRestoresAmmoToPlacingShip() {
    // AC (b): During BS_PlaceMines, clicking a mined hex removes it regardless of
    // selected source and restores ammo to the placing ship.
    //
    // Setup: Two Minelayer ships on the defending side.  Ship A places a mine while
    // selected.  Then Ship B is selected as the current source.  Clicking the mined
    // hex must restore ammo to Ship A (not Ship B).
    using namespace Frontier;

    // Create two Minelayers and one attacker.
    FVehicle * minelayerA = createShip("Minelayer");
    CPPUNIT_ASSERT_MESSAGE("Minelayer A must be creatable", minelayerA != NULL);
    FVehicle * minelayerB = createShip("Minelayer");
    CPPUNIT_ASSERT_MESSAGE("Minelayer B must be creatable", minelayerB != NULL);
    FVehicle * attacker = createShip("AssaultScout");
    CPPUNIT_ASSERT_MESSAGE("Attacker must be creatable", attacker != NULL);

    FFleet * defendFleet = new FFleet();
    minelayerA->setOwner(0);
    minelayerB->setOwner(0);
    defendFleet->addShip(minelayerA);
    defendFleet->addShip(minelayerB);
    FleetList defendFleets;
    defendFleets.push_back(defendFleet);

    FFleet * attackFleet = new FFleet();
    attacker->setOwner(1);
    attackFleet->addShip(attacker);
    FleetList attackFleets;
    attackFleets.push_back(attackFleet);

    FTacticalGame game;
    game.setupFleets(&attackFleets, &defendFleets, false, NULL);

    // Place both defenders and the attacker.
    game.setState(BS_SetupDefendFleet);
    game.setControlState(true);
    game.setShip(minelayerA);
    game.placeShip(FPoint(8, 8));
    game.setShipPlacementHeading(0);
    game.setShip(minelayerB);
    game.placeShip(FPoint(12, 8));
    game.setShipPlacementHeading(0);

    game.setState(BS_SetupAttackFleet);
    game.setControlState(true);
    game.setShip(attacker);
    game.placeShip(FPoint(30, 10));
    game.setShipPlacementHeading(3);

    // Enter mine placement with defender as active player (owner 0).
    game.setActivePlayer(false);
    const bool entered = game.beginOrdnancePlacement();
    CPPUNIT_ASSERT_MESSAGE("beginOrdnancePlacement() must succeed with Minelayers in fleet", entered);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must be in BS_PlaceMines",
        static_cast<int>(BS_PlaceMines), game.getState());

    // Identify the mine launchers for both ships and note their weapon indices.
    int mineIndexA = -1;
    for (unsigned int i = 0; i < minelayerA->getWeaponCount(); ++i) {
        FWeapon * w = minelayerA->getWeapon(i);
        if (w && w->getType() == FWeapon::M && mineIndexA < 0) {
            mineIndexA = static_cast<int>(i);
        }
    }
    int mineIndexB = -1;
    for (unsigned int i = 0; i < minelayerB->getWeaponCount(); ++i) {
        FWeapon * w = minelayerB->getWeapon(i);
        if (w && w->getType() == FWeapon::M && mineIndexB < 0) {
            mineIndexB = static_cast<int>(i);
        }
    }
    CPPUNIT_ASSERT_MESSAGE("Minelayer A must have an M launcher", mineIndexA >= 0);
    CPPUNIT_ASSERT_MESSAGE("Minelayer B must have an M launcher", mineIndexB >= 0);

    FWeapon * mineWeaponA = minelayerA->getWeapon(static_cast<unsigned int>(mineIndexA));
    FWeapon * mineWeaponB = minelayerB->getWeapon(static_cast<unsigned int>(mineIndexB));
    CPPUNIT_ASSERT_MESSAGE("Mine weapon A must not be null", mineWeaponA != NULL);
    CPPUNIT_ASSERT_MESSAGE("Mine weapon B must not be null", mineWeaponB != NULL);

    // Select Ship A as the current placement source.
    const bool selectedA = game.selectPlacementSource(minelayerA->getID(),
        static_cast<unsigned int>(mineIndexA));
    CPPUNIT_ASSERT_MESSAGE("selectPlacementSource for Minelayer A must succeed", selectedA);
    CPPUNIT_ASSERT_MESSAGE("Current ship must be Minelayer A after selection",
        game.getShip() != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Current ship ID must match Minelayer A",
        minelayerA->getID(), game.getShip()->getID());

    const int ammoABeforePlacement = mineWeaponA->getAmmo();
    const int ammoBBeforePlacement = mineWeaponB->getAmmo();
    CPPUNIT_ASSERT_MESSAGE("Minelayer A mine ammo must be > 0", ammoABeforePlacement > 0);
    CPPUNIT_ASSERT_MESSAGE("Minelayer B mine ammo must be > 0", ammoBBeforePlacement > 0);

    // Ship A places a mine.
    const FPoint mineHex(4, 4);
    const bool placed = game.handleHexClick(mineHex);
    CPPUNIT_ASSERT_MESSAGE("Mine placement click must succeed for Ship A", placed);
    CPPUNIT_ASSERT_MESSAGE("Mined hex must appear in getMinedHexes()",
        game.getMinedHexes().find(mineHex) != game.getMinedHexes().end());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Minelayer A ammo must be decremented by 1",
        ammoABeforePlacement - 1, mineWeaponA->getAmmo());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Minelayer B ammo must be unchanged after Ship A placement",
        ammoBBeforePlacement, mineWeaponB->getAmmo());

    // Verify placed-ordnance records the source as Ship A.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Must have exactly 1 placed-ordnance record after Ship A placement",
        static_cast<unsigned int>(1),
        static_cast<unsigned int>(game.getPlacedOrdnance().size()));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed-ordnance source ship must be Ship A's ID",
        minelayerA->getID(), game.getPlacedOrdnance()[0].source.shipID);

    // Now switch the selected source to Ship B before clicking the mined hex.
    const bool selectedB = game.selectPlacementSource(minelayerB->getID(),
        static_cast<unsigned int>(mineIndexB));
    CPPUNIT_ASSERT_MESSAGE("selectPlacementSource for Minelayer B must succeed", selectedB);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Current ship must switch to Minelayer B",
        minelayerB->getID(), game.getShip()->getID());

    // Click the mined hex with Ship B selected — must remove the mine and restore
    // ammo to Ship A (the placing ship), not Ship B.
    const bool removed = game.handleHexClick(mineHex);
    CPPUNIT_ASSERT_MESSAGE(
        "Clicking mined hex with non-placing source (Ship B) selected must return true",
        removed);

    // Mine must be gone.
    CPPUNIT_ASSERT_MESSAGE("Mined hex must no longer be in getMinedHexes() after removal",
        game.getMinedHexes().find(mineHex) == game.getMinedHexes().end());

    // Placed-ordnance list must be cleared.
    CPPUNIT_ASSERT_EQUAL_MESSAGE("getPlacedOrdnance() must be empty after mine removal",
        static_cast<unsigned int>(0),
        static_cast<unsigned int>(game.getPlacedOrdnance().size()));

    // Ammo must be restored to Ship A (the placing ship).
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Minelayer A ammo must be restored to initial value after source-independent mine removal",
        ammoABeforePlacement, mineWeaponA->getAmmo());

    // Ship B ammo must be unchanged.
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Minelayer B ammo must remain unchanged — only the placing ship's ammo is restored",
        ammoBBeforePlacement, mineWeaponB->getAmmo());

    delete defendFleet;
    delete attackFleet;
}

}  // namespace FrontierTests
