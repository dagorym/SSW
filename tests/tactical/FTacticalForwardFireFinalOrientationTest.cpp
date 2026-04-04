/**
 * @file FTacticalForwardFireFinalOrientationTest.cpp
 * @brief Runtime regression tests for forward-fire final-orientation behavior
 * @author Tom Stephens
 * @date Created: Apr 04, 2026
 */

#include "FTacticalForwardFireFinalOrientationTest.h"

#include "core/FHexMap.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

namespace FrontierTests {

namespace {

using namespace Frontier;

class FForwardFireWeaponHarness : public FWeapon {
public:
	FForwardFireWeaponHarness() {
		m_name = "FF";
		m_fullName = "Forward Fire Harness";
		m_type = FWeapon::LC;
		m_range = 4;
		m_nDice = 1;
		m_baseToHitProb = 100;
		m_FF = true;
	}
};

class FBatteryWeaponHarness : public FWeapon {
public:
	FBatteryWeaponHarness() {
		m_name = "BAT";
		m_fullName = "Battery Harness";
		m_type = FWeapon::LB;
		m_range = 4;
		m_nDice = 1;
		m_baseToHitProb = 100;
		m_FF = false;
	}
};

class FFireHeadingShipHarness : public FVehicle {
public:
	FFireHeadingShipHarness(const std::string & name) {
		m_name = name;
		m_type = "FireHeadingHarness";
		m_maxHP = 20;
		m_currentHP = 20;
		m_maxADF = 1;
		m_currentADF = 1;
		m_maxMR = 1;
		m_currentMR = 1;
		m_maxDCR = 1;
		m_currentDCR = 1;
		m_speed = 1;
	}

	void addWeapon(FWeapon * weapon) {
		m_weapons.push_back(weapon);
		weapon->setParent(this);
	}
};

struct FForwardFireFixture {
	FTacticalGame game;
	FleetList attackFleets;
	FleetList defendFleets;
	FFleet * attackFleet;
	FFleet * defendFleet;
	FFireHeadingShipHarness * attacker;
	FFireHeadingShipHarness * defenderAccept;
	FFireHeadingShipHarness * defenderReject;
	FForwardFireWeaponHarness * forwardWeapon;
	FBatteryWeaponHarness * batteryWeapon;
	FPoint startHex;
	FPoint endHex;
	int startHeading;
	int finalHeading;
};

void destroyFixture(FForwardFireFixture & fixture) {
	for (FleetList::iterator itr = fixture.attackFleets.begin(); itr != fixture.attackFleets.end(); ++itr) {
		delete *itr;
	}
	for (FleetList::iterator itr = fixture.defendFleets.begin(); itr != fixture.defendFleets.end(); ++itr) {
		delete *itr;
	}
	fixture.attackFleets.clear();
	fixture.defendFleets.clear();
}

void placeShip(FTacticalGame & game, FVehicle * ship, const FPoint & hex, int heading, int state) {
	game.setState(state);
	game.setControlState(true);
	game.setShip(ship);
	CPPUNIT_ASSERT(game.placeShip(hex));
	CPPUNIT_ASSERT(game.setShipPlacementHeading(heading));
}

void computeForwardFireCoverage(const FPoint & pos, int heading, unsigned int range, PointSet & targetHexes, PointSet & headOnHexes) {
	FPoint curHex = pos;
	for (unsigned int i = 0; i <= range; i++) {
		headOnHexes.insert(curHex);
		curHex = FHexMap::findNextHex(curHex, heading);
	}

	curHex = FHexMap::findNextHex(pos, heading);
	int rightHeading = (heading + 5) % 6;
	curHex = FHexMap::findNextHex(curHex, rightHeading);
	while (curHex.getX() >= 0 && curHex.getX() < 55 && curHex.getY() >= 0 && curHex.getY() < 39 &&
		   FHexMap::computeHexDistance(pos, curHex) <= static_cast<int>(range)) {
		targetHexes.insert(curHex);
		curHex = FHexMap::findNextHex(curHex, rightHeading);
	}

	curHex = FHexMap::findNextHex(pos, heading);
	int leftHeading = (heading + 1) % 6;
	curHex = FHexMap::findNextHex(curHex, leftHeading);
	while (curHex.getX() >= 0 && curHex.getX() < 55 && curHex.getY() >= 0 && curHex.getY() < 39 &&
		   FHexMap::computeHexDistance(pos, curHex) <= static_cast<int>(range)) {
		targetHexes.insert(curHex);
		curHex = FHexMap::findNextHex(curHex, leftHeading);
	}
}

bool containsHex(const PointSet & targetHexes, const PointSet & headOnHexes, const FPoint & hex) {
	return (targetHexes.find(hex) != targetHexes.end() || headOnHexes.find(hex) != headOnHexes.end());
}

bool samePoint(const FPoint & first, const FPoint & second) {
	return (first.getX() == second.getX() && first.getY() == second.getY());
}

bool equalPointSets(const PointSet & first, const PointSet & second) {
	if (first.size() != second.size()) {
		return false;
	}
	PointSet::const_iterator itrA = first.begin();
	PointSet::const_iterator itrB = second.begin();
	while (itrA != first.end() && itrB != second.end()) {
		if (!samePoint(*itrA, *itrB)) {
			return false;
		}
		++itrA;
		++itrB;
	}
	return (itrA == first.end() && itrB == second.end());
}

void applyPathWithFinalTurn(FTacticalTurnData * turnData, const FPoint & start, const FPoint & end, int startHeading, int finalHeading) {
	CPPUNIT_ASSERT(turnData != NULL);
	turnData->path.clear();
	turnData->path.addPoint(start);
	turnData->path.addPoint(end);
	turnData->startHeading = startHeading;
	turnData->curHeading = finalHeading;
	turnData->finalHeading = finalHeading;
	turnData->nMoved = 1;
}

bool selectWeaponForCurrentShip(FForwardFireFixture & fixture, unsigned int weaponIndex) {
	if (fixture.game.selectWeapon(weaponIndex)) {
		return true;
	}
	fixture.game.setActivePlayer(!fixture.game.getActivePlayer());
	fixture.game.setMovingPlayer(fixture.game.getActivePlayer());
	return fixture.game.selectWeapon(weaponIndex);
}

void setupFixture(FForwardFireFixture & fixture, const FPoint & acceptHex, const FPoint & rejectHex) {
	fixture.attackFleet = new FFleet();
	fixture.defendFleet = new FFleet();
	fixture.attackFleets.push_back(fixture.attackFleet);
	fixture.defendFleets.push_back(fixture.defendFleet);

	fixture.attacker = new FFireHeadingShipHarness("Attacker");
	fixture.defenderAccept = new FFireHeadingShipHarness("AcceptTarget");
	fixture.defenderReject = new FFireHeadingShipHarness("RejectTarget");
	fixture.forwardWeapon = new FForwardFireWeaponHarness();
	fixture.batteryWeapon = new FBatteryWeaponHarness();

	fixture.attacker->addWeapon(fixture.forwardWeapon);
	fixture.attacker->addWeapon(fixture.batteryWeapon);

	fixture.attackFleet->addShip(fixture.attacker);
	fixture.defendFleet->addShip(fixture.defenderAccept);
	fixture.defendFleet->addShip(fixture.defenderReject);

	fixture.game.setupFleets(&fixture.attackFleets, &fixture.defendFleets, false, NULL);

	placeShip(fixture.game, fixture.defenderAccept, acceptHex, 3, BS_SetupDefendFleet);
	placeShip(fixture.game, fixture.defenderReject, rejectHex, 3, BS_SetupDefendFleet);
	placeShip(fixture.game, fixture.attacker, fixture.startHex, fixture.startHeading, BS_SetupAttackFleet);

	const bool attackerSide = (fixture.attacker->getOwner() == fixture.game.getAttackerID());
	fixture.game.setState(BS_Battle);
	fixture.game.setMovingPlayer(attackerSide);
	fixture.game.setActivePlayer(attackerSide);
	fixture.game.setPhase(PH_ATTACK_FIRE);
	CPPUNIT_ASSERT(fixture.game.selectShipFromHex(fixture.startHex));
}

FPoint findExclusiveHex(const PointSet & includeTargets,
						const PointSet & includeHeadOn,
						const PointSet & excludeTargets,
						const PointSet & excludeHeadOn) {
	for (PointSet::const_iterator itr = includeHeadOn.begin(); itr != includeHeadOn.end(); ++itr) {
		if (!containsHex(excludeTargets, excludeHeadOn, *itr)) {
			return *itr;
		}
	}
	for (PointSet::const_iterator itr = includeTargets.begin(); itr != includeTargets.end(); ++itr) {
		if (!containsHex(excludeTargets, excludeHeadOn, *itr)) {
			return *itr;
		}
	}
	return FPoint(-1, -1);
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalForwardFireFinalOrientationTest );

void FTacticalForwardFireFinalOrientationTest::setUp() {
}

void FTacticalForwardFireFinalOrientationTest::tearDown() {
}

void FTacticalForwardFireFinalOrientationTest::testFinalTurnEndpointUsesFinalHeadingForHighlightsAndTargetValidation() {
	FForwardFireFixture fixture;
	const unsigned int weaponRange = 4;
	fixture.startHex = FPoint(20, 20);
	fixture.startHeading = 0;
	fixture.endHex = FHexMap::findNextHex(fixture.startHex, fixture.startHeading);
	fixture.finalHeading = (fixture.startHeading + 1) % 6;

	PointSet startTargets;
	PointSet startHeadOn;
	PointSet endTargetsFinal;
	PointSet endHeadOnFinal;
	PointSet endTargetsStale;
	PointSet endHeadOnStale;
	computeForwardFireCoverage(fixture.startHex, fixture.startHeading, weaponRange, startTargets, startHeadOn);
	computeForwardFireCoverage(fixture.endHex, fixture.finalHeading, weaponRange, endTargetsFinal, endHeadOnFinal);
	computeForwardFireCoverage(fixture.endHex, fixture.startHeading, weaponRange, endTargetsStale, endHeadOnStale);

	const FPoint acceptHex = findExclusiveHex(endTargetsFinal, endHeadOnFinal, endTargetsStale, endHeadOnStale);
	const FPoint rejectHex = findExclusiveHex(endTargetsStale, endHeadOnStale, endTargetsFinal, endHeadOnFinal);
	CPPUNIT_ASSERT(acceptHex.getX() >= 0);
	CPPUNIT_ASSERT(rejectHex.getX() >= 0);
	CPPUNIT_ASSERT(!samePoint(acceptHex, fixture.startHex));
	CPPUNIT_ASSERT(!samePoint(acceptHex, fixture.endHex));
	CPPUNIT_ASSERT(!samePoint(rejectHex, fixture.startHex));
	CPPUNIT_ASSERT(!samePoint(rejectHex, fixture.endHex));
	CPPUNIT_ASSERT(!samePoint(acceptHex, rejectHex));

	setupFixture(fixture, acceptHex, rejectHex);

	FTacticalTurnData * turnData = fixture.game.findTurnData(fixture.attacker->getID());
	applyPathWithFinalTurn(turnData, fixture.startHex, fixture.endHex, fixture.startHeading, fixture.finalHeading);

	CPPUNIT_ASSERT(selectWeaponForCurrentShip(fixture, 0));
	CPPUNIT_ASSERT(containsHex(fixture.game.getTargetHexes(), fixture.game.getHeadOnHexes(), acceptHex));
	CPPUNIT_ASSERT(!containsHex(fixture.game.getTargetHexes(), fixture.game.getHeadOnHexes(), rejectHex));

	CPPUNIT_ASSERT(!fixture.game.assignTargetFromHex(rejectHex));
	CPPUNIT_ASSERT(fixture.forwardWeapon->getTarget() == NULL);

	CPPUNIT_ASSERT(fixture.game.assignTargetFromHex(acceptHex));
	CPPUNIT_ASSERT_EQUAL(fixture.defenderAccept->getID(), fixture.forwardWeapon->getTarget()->getID());

	destroyFixture(fixture);
}

void FTacticalForwardFireFinalOrientationTest::testStraightLineForwardFireRemainsUnchanged() {
	FForwardFireFixture fixture;
	fixture.startHex = FPoint(18, 18);
	fixture.startHeading = 2;
	fixture.endHex = FHexMap::findNextHex(fixture.startHex, fixture.startHeading);
	fixture.finalHeading = fixture.startHeading;

	PointSet endTargets;
	PointSet endHeadOn;
	computeForwardFireCoverage(fixture.endHex, fixture.startHeading, 4, endTargets, endHeadOn);
	FPoint straightHex(-1, -1);
	for (PointSet::const_iterator itr = endHeadOn.begin(); itr != endHeadOn.end(); ++itr) {
		if (!samePoint(*itr, fixture.startHex) && !samePoint(*itr, fixture.endHex)) {
			straightHex = *itr;
			break;
		}
	}
	if (straightHex.getX() < 0) {
		for (PointSet::const_iterator itr = endTargets.begin(); itr != endTargets.end(); ++itr) {
			if (!samePoint(*itr, fixture.startHex) && !samePoint(*itr, fixture.endHex)) {
				straightHex = *itr;
				break;
			}
		}
	}
	CPPUNIT_ASSERT(straightHex.getX() >= 0);
	CPPUNIT_ASSERT(!samePoint(straightHex, fixture.startHex));
	CPPUNIT_ASSERT(!samePoint(straightHex, fixture.endHex));

	setupFixture(fixture, straightHex, FPoint(40, 30));

	FTacticalTurnData * turnData = fixture.game.findTurnData(fixture.attacker->getID());
	applyPathWithFinalTurn(turnData, fixture.startHex, fixture.endHex, fixture.startHeading, fixture.finalHeading);

	CPPUNIT_ASSERT(selectWeaponForCurrentShip(fixture, 0));
	CPPUNIT_ASSERT(containsHex(fixture.game.getTargetHexes(), fixture.game.getHeadOnHexes(), straightHex));
	CPPUNIT_ASSERT(fixture.game.assignTargetFromHex(straightHex));
	CPPUNIT_ASSERT_EQUAL(fixture.defenderAccept->getID(), fixture.forwardWeapon->getTarget()->getID());

	destroyFixture(fixture);
}

void FTacticalForwardFireFinalOrientationTest::testBatteryRangeRemainsHeadingIndependent() {
	FForwardFireFixture fixture;
	fixture.startHex = FPoint(30, 16);
	fixture.startHeading = 3;
	fixture.endHex = FHexMap::findNextHex(fixture.startHex, fixture.startHeading);
	fixture.finalHeading = (fixture.startHeading + 2) % 6;

	setupFixture(fixture, FPoint(24, 16), FPoint(36, 16));

	FTacticalTurnData * turnData = fixture.game.findTurnData(fixture.attacker->getID());
	applyPathWithFinalTurn(turnData, fixture.startHex, fixture.endHex, fixture.startHeading, fixture.finalHeading);

	CPPUNIT_ASSERT(selectWeaponForCurrentShip(fixture, 1));
	const PointSet firstTargets = fixture.game.getTargetHexes();
	const PointSet firstHeadOn = fixture.game.getHeadOnHexes();

	turnData->finalHeading = (fixture.finalHeading + 3) % 6;
	turnData->curHeading = turnData->finalHeading;
	CPPUNIT_ASSERT(selectWeaponForCurrentShip(fixture, 1));
	const PointSet secondTargets = fixture.game.getTargetHexes();
	const PointSet secondHeadOn = fixture.game.getHeadOnHexes();

	CPPUNIT_ASSERT(equalPointSets(firstTargets, secondTargets));
	CPPUNIT_ASSERT(equalPointSets(firstHeadOn, secondHeadOn));

	destroyFixture(fixture);
}

}
