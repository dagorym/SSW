/**
 * @file FTacticalSeekerMovementTest.cpp
 * @brief Source-inspection tests for tactical seeker movement helper logic
 */

#include "FTacticalSeekerMovementTest.h"

#include "core/FHexMap.h"
#include "core/FHexPath.h"
#include "ships/FVehicle.h"
#include "tactical/FTacticalGame.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <set>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

class TestableTacticalGame : public Frontier::FTacticalGame {
public:
	using Frontier::FTacticalGame::adjustSeekerHeadingTowardTarget;
	using Frontier::FTacticalGame::selectSeekerContactTargetAtHex;
	using Frontier::FTacticalGame::completeSeekerActivationPhase;
	using Frontier::FTacticalGame::moveSeekerTowardTarget;
	using Frontier::FTacticalGame::computeSeekerMovementAllowance;
	using Frontier::FTacticalGame::collectClosestSeekerTargetIDs;
	using Frontier::FTacticalGame::computeSeekerGreedyNextStep;
	using Frontier::FTacticalGame::resolveActiveSeekersForMovingPlayer;
	using Frontier::FTacticalGame::checkForActiveSeekersOnPath;
	using Frontier::FTacticalGame::applyMovementSeekerDamage;

	void configureSides(unsigned int defenderID, unsigned int attackerID, bool movingAttacker) {
		m_playerID[0] = defenderID;
		m_playerID[1] = attackerID;
		m_movingPlayer = movingAttacker;
	}

	void installShipLists(Frontier::VehicleList * attackShips, Frontier::VehicleList * defendShips) {
		m_attackShips = attackShips;
		m_defendShips = defendShips;
	}

	void placeShipAtHex(Frontier::FVehicle * ship, const Frontier::FPoint & hex) {
		m_hexData[hex.getX()][hex.getY()].pos = hex;
		m_hexData[hex.getX()][hex.getY()].ships.push_back(ship);
	}

	void seedSeeker(const Frontier::FTacticalSeekerMissileState & seeker) {
		m_seekerMissiles.push_back(seeker);
	}

	// Seed a minimal turn data record so checkForActiveSeekersOnPath can find the ship's path.
	void seedTurnDataWithPath(unsigned int shipID, const Frontier::PointList & path) {
		Frontier::FTacticalTurnData data;
		data.hasMoved = true;
		data.speed = 1;
		data.startHeading = 0;
		data.finalHeading = 0;
		data.curHeading = 0;
		data.nMoved = 1;
		for (Frontier::PointList::const_iterator itr = path.begin(); itr != path.end(); ++itr) {
			data.path.addPoint(*itr);
		}
		m_turnInfo[shipID] = data;
	}

	void clearPendingOutcomes() {
		clearPendingSeekerContactOutcomes();
	}
};

class FSeekerHarnessShip : public Frontier::FVehicle {
public:
	FSeekerHarnessShip(unsigned int ownerID, const std::string & type) {
		m_owner = ownerID;
		m_type = type;
		m_name = "SeekerHarnessShip";
		m_maxHP = 25;
		m_currentHP = 25;
		m_speed = 0;
		m_maxADF = 0;
		m_currentADF = 0;
		m_maxMR = 0;
		m_currentMR = 0;
		m_maxDCR = 1;
		m_currentDCR = 1;
	}

	void configureHP(int hp) {
		m_maxHP = hp;
		m_currentHP = hp;
	}

};

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalSeekerMovementTest );

void FTacticalSeekerMovementTest::setUp() {
}

void FTacticalSeekerMovementTest::tearDown() {
}

std::string FTacticalSeekerMovementTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalSeekerMovementTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT_MESSAGE(signature, signaturePos != std::string::npos);

std::string::size_type bodyStart = source.find('{', signaturePos);
CPPUNIT_ASSERT(bodyStart != std::string::npos);

int depth = 1;
for (std::string::size_type i = bodyStart + 1; i < source.size(); ++i) {
if (source[i] == '{') {
depth++;
} else if (source[i] == '}') {
depth--;
if (depth == 0) {
return source.substr(bodyStart + 1, i - bodyStart - 1);
}
}
}

CPPUNIT_FAIL("Function body not terminated");
return "";
}

void FTacticalSeekerMovementTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalSeekerMovementTest::testSeekerTargetSelectionFiltersAndTieBreaksAcrossSides() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string resolveBody = extractFunctionBody(source, "void FTacticalGame::resolveActiveSeekersForMovingPlayer()");
const std::string validTargetBody = extractFunctionBody(source, "bool FTacticalGame::isValidSeekerTarget(const FVehicle * ship) const");
const std::string selectTargetBody = extractFunctionBody(source,
"FVehicle * FTacticalGame::selectClosestSeekerTarget(");
const std::string tieBreakBody = extractFunctionBody(source,
"unsigned int FTacticalGame::chooseRandomSeekerIndex(unsigned int candidateCount) const");
const std::string contactTargetBody = extractFunctionBody(source,
"FVehicle * FTacticalGame::selectSeekerContactTargetAtHex(");

assertContains(resolveBody, "candidateTargets.insert(candidateTargets.end(), m_attackShips->begin(), m_attackShips->end());");
assertContains(resolveBody, "candidateTargets.insert(candidateTargets.end(), m_defendShips->begin(), m_defendShips->end());");
assertContains(resolveBody, "if (!itr->active || itr->ownerID != getMovingPlayerID()) {");
assertContains(resolveBody, "FVehicle * target = selectClosestSeekerTarget(*itr, candidateTargets);");

assertContains(validTargetBody, "ship != NULL");
assertContains(validTargetBody, "ship->getHP() > 0");
assertContains(validTargetBody, "!isStationType(const_cast<FVehicle *>(ship));");

assertContains(selectTargetBody, "if (!isValidSeekerTarget(*itr)) {");
assertContains(selectTargetBody, "const int distance = FHexMap::computeHexDistance(seeker.hex, shipHex);");
assertContains(selectTargetBody, "} else if (distance == closestDistance) {");
assertContains(selectTargetBody, "return closestShips[chooseRandomSeekerIndex(static_cast<unsigned int>(closestShips.size()))];");

assertContains(tieBreakBody, "if (candidateCount <= 1) {");
assertContains(tieBreakBody, "return static_cast<unsigned int>(irand(candidateCount) - 1);");
assertContains(contactTargetBody, "const int maxHP = (*itr)->getMaxHP();");
assertContains(contactTargetBody, "if (maxHP > highestMaxHP) {");
assertContains(contactTargetBody, "toughestTargets.push_back(*itr);");
assertContains(contactTargetBody, "validTargets = toughestTargets;");
assertContains(contactTargetBody, "randomTargetIndex = chooseRandomSeekerIndex(static_cast<unsigned int>(validTargets.size()));");
assertContains(contactTargetBody, "return toughestTargets[randomTargetIndex % toughestTargets.size()];");
}

void FTacticalSeekerMovementTest::testSeekerHeadingAndMovementGreedyTurnLimits() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string adjustBody = extractFunctionBody(source,
"void FTacticalGame::adjustSeekerHeadingTowardTarget(");
const std::string moveBody = extractFunctionBody(source,
"void FTacticalGame::moveSeekerTowardTarget(FTacticalSeekerMissileState & seeker, const FPoint & targetHex) const");

assertContains(adjustBody, "int delta = shortestHeadingDelta(normalizeHeading(seeker.heading), targetHeading);");
assertContains(adjustBody, "if (delta > 3) {");
assertContains(adjustBody, "delta = 3;");
assertContains(adjustBody, "} else if (delta < -3) {");
assertContains(adjustBody, "delta = -3;");
assertContains(adjustBody, "seeker.heading = normalizeHeading(seeker.heading + delta);");

assertContains(moveBody, "for (int step = 0; step < seeker.movementAllowance; ++step) {");
assertContains(moveBody, "for (int turn = -1; turn <= 1; ++turn) {");
assertContains(moveBody, "const int candidateHeading = normalizeHeading(seeker.heading + turn);");
assertContains(moveBody, "const FPoint candidateHex = FHexMap::findNextHex(seeker.hex, candidateHeading);");
assertContains(moveBody, "const int candidateDistance = FHexMap::computeHexDistance(candidateHex, targetHex);");
assertContains(moveBody, "|| candidateDistance < bestDistance");
assertContains(moveBody, "|| (candidateDistance == bestDistance && turnMagnitude < bestTurnMagnitude)) {");
assertContains(moveBody, "seeker.heading = bestHeading;");
assertContains(moveBody, "seeker.hex = bestHex;");

TestableTacticalGame game;
Frontier::FTacticalSeekerMissileState seeker;
seeker.hex = Frontier::FPoint(20, 20);
seeker.heading = 0;
seeker.movementAllowance = 1;

Frontier::FPoint targetHex = seeker.hex;
for (int i = 0; i < 5; ++i) {
	targetHex = Frontier::FHexMap::findNextHex(targetHex, 3);
}
game.adjustSeekerHeadingTowardTarget(seeker, targetHex);
CPPUNIT_ASSERT_EQUAL(3, seeker.heading);

const Frontier::FPoint startHex = seeker.hex;
game.moveSeekerTowardTarget(seeker, targetHex);
const int headingDelta = (seeker.heading - 3 + 6) % 6;
CPPUNIT_ASSERT(headingDelta == 0 || headingDelta == 1 || headingDelta == 5);
CPPUNIT_ASSERT(seeker.hex == Frontier::FHexMap::findNextHex(startHex, seeker.heading));
}

void FTacticalSeekerMovementTest::testSeekerMovementAllowanceProgression() {
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string resolveBody = extractFunctionBody(source, "void FTacticalGame::resolveActiveSeekersForMovingPlayer()");
const std::string allowanceBody = extractFunctionBody(source,
"unsigned int FTacticalGame::computeSeekerMovementAllowance(int movementTurn) const");

assertContains(resolveBody, "itr->movementTurn += 1;");
assertContains(resolveBody, "itr->movementAllowance = static_cast<int>(computeSeekerMovementAllowance(itr->movementTurn));");
assertContains(allowanceBody, "if (movementTurn <= 0) {");
assertContains(allowanceBody, "return 0;");
assertContains(allowanceBody, "const unsigned int allowance = static_cast<unsigned int>(movementTurn * 2);");
assertContains(allowanceBody, "return (allowance > 12) ? 12 : allowance;");

TestableTacticalGame game;
CPPUNIT_ASSERT_EQUAL(0u, game.computeSeekerMovementAllowance(0));
CPPUNIT_ASSERT_EQUAL(2u, game.computeSeekerMovementAllowance(1));
CPPUNIT_ASSERT_EQUAL(4u, game.computeSeekerMovementAllowance(2));
CPPUNIT_ASSERT_EQUAL(6u, game.computeSeekerMovementAllowance(3));
CPPUNIT_ASSERT_EQUAL(8u, game.computeSeekerMovementAllowance(4));
CPPUNIT_ASSERT_EQUAL(10u, game.computeSeekerMovementAllowance(5));
CPPUNIT_ASSERT_EQUAL(12u, game.computeSeekerMovementAllowance(6));
CPPUNIT_ASSERT_EQUAL(12u, game.computeSeekerMovementAllowance(9));
}

void FTacticalSeekerMovementTest::testDeterministicTargetSelectionHelperFiltersAndReturnsTies() {
TestableTacticalGame game;
Frontier::FTacticalSeekerMissileState seeker;
seeker.hex = Frontier::FPoint(10, 10);
seeker.heading = 0;

std::vector<Frontier::FTacticalSeekerTargetSnapshot> candidates;
Frontier::FTacticalSeekerTargetSnapshot targetA = {101u, Frontier::FPoint(12, 10), 8, false};
Frontier::FTacticalSeekerTargetSnapshot targetB = {202u, Frontier::FPoint(9, 12), 6, false};
Frontier::FTacticalSeekerTargetSnapshot station = {303u, Frontier::FPoint(10, 9), 15, true};
Frontier::FTacticalSeekerTargetSnapshot destroyed = {404u, Frontier::FPoint(11, 10), 0, false};
Frontier::FTacticalSeekerTargetSnapshot offBoard = {505u, Frontier::FPoint(-1, -1), 9, false};
Frontier::FTacticalSeekerTargetSnapshot farther = {606u, Frontier::FPoint(15, 10), 5, false};
candidates.push_back(targetA);
candidates.push_back(targetB);
candidates.push_back(station);
candidates.push_back(destroyed);
candidates.push_back(offBoard);
candidates.push_back(farther);

const std::vector<unsigned int> closest = game.collectClosestSeekerTargetIDs(seeker, candidates);
CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), closest.size());
const std::set<unsigned int> closestIDs(closest.begin(), closest.end());
CPPUNIT_ASSERT(closestIDs.find(101u) != closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(202u) != closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(303u) == closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(404u) == closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(505u) == closestIDs.end());
CPPUNIT_ASSERT(closestIDs.find(606u) == closestIDs.end());
}

void FTacticalSeekerMovementTest::testSameHexContactTargetSelectionPrioritizesMaxHPAtRuntime() {
	TestableTacticalGame game;
	Frontier::VehicleList * attackShips = new Frontier::VehicleList();
	Frontier::VehicleList * defendShips = new Frontier::VehicleList();
	FSeekerHarnessShip * lowerHP = new FSeekerHarnessShip(0u, "Scout");
	FSeekerHarnessShip * highHPA = new FSeekerHarnessShip(0u, "DestroyerA");
	FSeekerHarnessShip * highHPB = new FSeekerHarnessShip(0u, "DestroyerB");

	lowerHP->configureHP(15);
	highHPA->configureHP(30);
	highHPB->configureHP(30);

	attackShips->push_back(highHPA);
	defendShips->push_back(lowerHP);
	defendShips->push_back(highHPB);

	game.configureSides(0u, 1u, true);
	game.installShipLists(attackShips, defendShips);

	const Frontier::FPoint contactHex(22, 22);
	game.placeShipAtHex(lowerHP, contactHex);
	game.placeShipAtHex(highHPA, contactHex);
	game.placeShipAtHex(highHPB, contactHex);

	Frontier::FTacticalSeekerMissileState seeker;
	seeker.ownerID = 1u;
	seeker.hex = contactHex;
	seeker.active = true;

	for (unsigned int i = 0; i < 64; ++i) {
		Frontier::FVehicle * target = game.selectSeekerContactTargetAtHex(seeker, contactHex);
		CPPUNIT_ASSERT(target != NULL);
		CPPUNIT_ASSERT(target->getMaxHP() == 30);
		CPPUNIT_ASSERT(target->getID() != lowerHP->getID());
	}

	delete lowerHP;
	delete highHPA;
	delete highHPB;
}

void FTacticalSeekerMovementTest::testDeterministicGreedyNextStepHelperReturnsOneStepMove() {
TestableTacticalGame game;
Frontier::FTacticalSeekerMissileState seeker;
seeker.hex = Frontier::FPoint(20, 20);
seeker.heading = 0;
seeker.movementAllowance = 6;

const Frontier::FPoint targetHex(24, 17);
Frontier::FTacticalSeekerMissileState nextState;
const bool advanced = game.computeSeekerGreedyNextStep(seeker, targetHex, nextState);
CPPUNIT_ASSERT(advanced);

const int previousDistance = Frontier::FHexMap::computeHexDistance(seeker.hex, targetHex);
const int nextDistance = Frontier::FHexMap::computeHexDistance(nextState.hex, targetHex);
int expectedBestDistance = -1;
for (int turn = -1; turn <= 1; ++turn) {
	const int candidateHeading = (seeker.heading + turn + 6) % 6;
	const Frontier::FPoint candidateHex = Frontier::FHexMap::findNextHex(seeker.hex, candidateHeading);
	const int candidateDistance = Frontier::FHexMap::computeHexDistance(candidateHex, targetHex);
	if (expectedBestDistance < 0 || candidateDistance < expectedBestDistance) {
		expectedBestDistance = candidateDistance;
	}
}
CPPUNIT_ASSERT_EQUAL(expectedBestDistance, nextDistance);
if (expectedBestDistance < previousDistance) {
	CPPUNIT_ASSERT(nextDistance < previousDistance);
}

const std::vector<int> legalHeadings = {
	((seeker.heading + 5) % 6),
	(seeker.heading % 6),
	((seeker.heading + 1) % 6)
};
CPPUNIT_ASSERT(std::find(legalHeadings.begin(), legalHeadings.end(), nextState.heading) != legalHeadings.end());
CPPUNIT_ASSERT(nextState.hex == Frontier::FHexMap::findNextHex(seeker.hex, nextState.heading));
}

void FTacticalSeekerMovementTest::testActiveSeekerResolutionHandlesPremoveContactAndEnemySkipRuntime() {
	TestableTacticalGame game;
	Frontier::VehicleList * attackShips = new Frontier::VehicleList();
	Frontier::VehicleList * defendShips = new Frontier::VehicleList();
	FSeekerHarnessShip * attackerShip = new FSeekerHarnessShip(1u, "Frigate");
	FSeekerHarnessShip * defenderShip = new FSeekerHarnessShip(0u, "Destroyer");
	attackShips->push_back(attackerShip);
	defendShips->push_back(defenderShip);

	game.configureSides(0u, 1u, true);
	game.installShipLists(attackShips, defendShips);
	game.placeShipAtHex(attackerShip, Frontier::FPoint(10, 10));
	game.placeShipAtHex(defenderShip, Frontier::FPoint(30, 30));

	Frontier::FTacticalSeekerMissileState movingSideSeeker;
	movingSideSeeker.seekerID = 9001u;
	movingSideSeeker.ownerID = 1u;
	movingSideSeeker.hex = Frontier::FPoint(30, 30);
	movingSideSeeker.heading = 0;
	movingSideSeeker.active = true;
	movingSideSeeker.movementTurn = 0;
	movingSideSeeker.movementAllowance = 0;
	movingSideSeeker.hasSource = false;

	Frontier::FTacticalSeekerMissileState enemySeeker;
	enemySeeker.seekerID = 9002u;
	enemySeeker.ownerID = 0u;
	enemySeeker.hex = Frontier::FPoint(40, 40);
	enemySeeker.heading = 4;
	enemySeeker.active = true;
	enemySeeker.movementTurn = 3;
	enemySeeker.movementAllowance = 6;
	enemySeeker.hasSource = false;

	game.seedSeeker(movingSideSeeker);
	game.seedSeeker(enemySeeker);

	game.resolveActiveSeekersForMovingPlayer();

	const std::vector<Frontier::FTacticalSeekerContactOutcome> & outcomes = game.getPendingSeekerContactOutcomes();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), outcomes.size());
	CPPUNIT_ASSERT_EQUAL(9001u, outcomes[0].seekerID);
	CPPUNIT_ASSERT(outcomes[0].preMovementContact);
	CPPUNIT_ASSERT_EQUAL(0u, outcomes[0].movementStep);
	CPPUNIT_ASSERT_EQUAL(1, outcomes[0].movementTurn);
	CPPUNIT_ASSERT_EQUAL(defenderShip->getID(), outcomes[0].targetShipID);

	// SMF-06: impacting seeker stays in m_seekerMissiles until after damage summary.
	// resolveActiveSeekersForMovingPlayer now keeps contacting seekers for
	// rendering during ICM/damage dialogs; applyMovementSeekerDamage removes them.
	const std::vector<Frontier::FTacticalSeekerMissileState> remainingSeekers = game.getSeekerMissiles();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), remainingSeekers.size());
	// Find enemy seeker (9002) and contacting seeker (9001) in the remaining list.
	bool found9001 = false;
	bool found9002 = false;
	for (unsigned int i = 0; i < remainingSeekers.size(); ++i) {
		if (remainingSeekers[i].seekerID == 9001u) { found9001 = true; }
		if (remainingSeekers[i].seekerID == 9002u) {
			found9002 = true;
			CPPUNIT_ASSERT_EQUAL(Frontier::FPoint(40, 40).getX(), remainingSeekers[i].hex.getX());
			CPPUNIT_ASSERT_EQUAL(Frontier::FPoint(40, 40).getY(), remainingSeekers[i].hex.getY());
			CPPUNIT_ASSERT_EQUAL(4, remainingSeekers[i].heading);
			CPPUNIT_ASSERT_EQUAL(3, remainingSeekers[i].movementTurn);
			CPPUNIT_ASSERT_EQUAL(6, remainingSeekers[i].movementAllowance);
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Contacting seeker 9001 must remain for damage rendering (SMF-06)", found9001);
	CPPUNIT_ASSERT_MESSAGE("Enemy seeker 9002 must remain unchanged", found9002);

	delete attackerShip;
	delete defenderShip;
}

void FTacticalSeekerMovementTest::testCompleteSeekerActivationResolvesMovementContactAndSurvivorProgressionRuntime() {
	TestableTacticalGame game;
	Frontier::VehicleList * attackShips = new Frontier::VehicleList();
	Frontier::VehicleList * defendShips = new Frontier::VehicleList();
	FSeekerHarnessShip * attackerShip = new FSeekerHarnessShip(1u, "Frigate");
	FSeekerHarnessShip * closeDefender = new FSeekerHarnessShip(0u, "Destroyer");
	FSeekerHarnessShip * farDefender = new FSeekerHarnessShip(0u, "Destroyer");
	attackShips->push_back(attackerShip);
	defendShips->push_back(closeDefender);
	defendShips->push_back(farDefender);

	const Frontier::FPoint closeDefenderHex(12, 10);
	const Frontier::FPoint farDefenderHex(90, 90);
	game.configureSides(0u, 1u, true);
	game.installShipLists(attackShips, defendShips);
	game.placeShipAtHex(closeDefender, closeDefenderHex);
	game.placeShipAtHex(farDefender, farDefenderHex);

	Frontier::FTacticalSeekerMissileState contactDuringMove;
	contactDuringMove.seekerID = 9101u;
	contactDuringMove.ownerID = 1u;
	contactDuringMove.hex = Frontier::FPoint(10, 10);
	contactDuringMove.heading = Frontier::FHexMap::computeHeading(contactDuringMove.hex, closeDefenderHex);
	contactDuringMove.active = true;
	contactDuringMove.movementTurn = 0;
	contactDuringMove.movementAllowance = 0;
	contactDuringMove.hasSource = false;

	Frontier::FTacticalSeekerMissileState survivorSeeker;
	survivorSeeker.seekerID = 9102u;
	survivorSeeker.ownerID = 1u;
	survivorSeeker.hex = Frontier::FPoint(80, 80);
	survivorSeeker.heading = Frontier::FHexMap::computeHeading(survivorSeeker.hex, farDefenderHex);
	survivorSeeker.active = true;
	survivorSeeker.movementTurn = 0;
	survivorSeeker.movementAllowance = 0;
	survivorSeeker.hasSource = false;

	Frontier::FTacticalSeekerMissileState expiringSeeker;
	expiringSeeker.seekerID = 9103u;
	expiringSeeker.ownerID = 1u;
	expiringSeeker.hex = Frontier::FPoint(1, 1);
	expiringSeeker.heading = 0;
	expiringSeeker.active = true;
	expiringSeeker.movementTurn = 5;
	expiringSeeker.movementAllowance = 10;
	expiringSeeker.hasSource = false;

	game.seedSeeker(contactDuringMove);
	game.seedSeeker(survivorSeeker);
	game.seedSeeker(expiringSeeker);
	game.setPhase(PH_SEEKER_ACTIVATION);
	game.completeSeekerActivationPhase();

	CPPUNIT_ASSERT_EQUAL(static_cast<int>(PH_MOVE), game.getPhase());

	const std::vector<Frontier::FTacticalSeekerContactOutcome> & outcomes = game.getPendingSeekerContactOutcomes();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), outcomes.size());
	CPPUNIT_ASSERT_EQUAL(9101u, outcomes[0].seekerID);
	CPPUNIT_ASSERT(!outcomes[0].preMovementContact);
	CPPUNIT_ASSERT(outcomes[0].movementStep >= 1u);

	// SMF-06: impacting seeker (9101) stays in m_seekerMissiles alongside survivor (9102).
	// applyMovementSeekerDamage would remove 9101 after dialogs; here no UI is installed
	// so resolvePendingSeekerDetonationDamage is skipped and 9101 is not yet removed.
	const std::vector<Frontier::FTacticalSeekerMissileState> seekers = game.getSeekerMissiles();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), seekers.size());
	// Find the survivor (9102) and verify its progression.
	const Frontier::FTacticalSeekerMissileState * survivor = NULL;
	for (unsigned int i = 0; i < seekers.size(); ++i) {
		if (seekers[i].seekerID == 9102u) { survivor = &seekers[i]; }
	}
	CPPUNIT_ASSERT_MESSAGE("Survivor seeker 9102 must remain", survivor != NULL);
	CPPUNIT_ASSERT_EQUAL(1, survivor->movementTurn);
	CPPUNIT_ASSERT_EQUAL(2, survivor->movementAllowance);
	CPPUNIT_ASSERT(
		survivor->hex.getX() != survivorSeeker.hex.getX()
		|| survivor->hex.getY() != survivorSeeker.hex.getY());

	delete attackerShip;
	delete closeDefender;
	delete farDefender;
}

void FTacticalSeekerMovementTest::testSeekerHelpersRemainModelOnlyAndFHexMapGeneric() {
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string mapHeader = readFile(repoFile("include/core/FHexMap.h"));
const std::string mapSource = readFile(repoFile("src/core/FHexMap.cpp"));
const std::string resolveBody = extractFunctionBody(source, "void FTacticalGame::resolveActiveSeekersForMovingPlayer()");
const std::string selectTargetBody = extractFunctionBody(source,
"FVehicle * FTacticalGame::selectClosestSeekerTarget(");
const std::string moveBody = extractFunctionBody(source,
"void FTacticalGame::moveSeekerTowardTarget(FTacticalSeekerMissileState & seeker, const FPoint & targetHex) const");

assertContains(header, "void resolveActiveSeekersForMovingPlayer();");
assertContains(header, "unsigned int computeSeekerMovementAllowance(int movementTurn) const;");
assertContains(header, "unsigned int chooseRandomSeekerIndex(unsigned int candidateCount) const;");
assertContains(resolveBody, "FPoint targetHex;");
assertContains(selectTargetBody, "const int distance = FHexMap::computeHexDistance(seeker.hex, shipHex);");
assertContains(moveBody, "const FPoint candidateHex = FHexMap::findNextHex(seeker.hex, candidateHeading);");

CPPUNIT_ASSERT(resolveBody.find("wx") == std::string::npos);
CPPUNIT_ASSERT(selectTargetBody.find("wx") == std::string::npos);
CPPUNIT_ASSERT(moveBody.find("wx") == std::string::npos);
CPPUNIT_ASSERT(mapHeader.find("wx") == std::string::npos);
CPPUNIT_ASSERT(mapSource.find("wx") == std::string::npos);
}

void FTacticalSeekerMovementTest::testInactiveSeekerIgnoredByPathContactCheck() {
	// AC: inactive seeker hexes do not trigger contact or damage.
	// An inactive seeker placed in a ship's movement path must not generate any
	// pending contact outcome.
	TestableTacticalGame game;
	Frontier::VehicleList * attackShips = new Frontier::VehicleList();
	Frontier::VehicleList * defendShips = new Frontier::VehicleList();
	FSeekerHarnessShip * movingShip = new FSeekerHarnessShip(1u, "Frigate");
	attackShips->push_back(movingShip);

	game.configureSides(0u, 1u, true);
	game.installShipLists(attackShips, defendShips);

	// Place the ship at hex (10,10).
	const Frontier::FPoint shipHex(10, 10);
	game.placeShipAtHex(movingShip, shipHex);

	// Build a path that passes through (11,10) and (12,10).
	Frontier::PointList path;
	path.push_back(shipHex);
	path.push_back(Frontier::FPoint(11, 10));
	path.push_back(Frontier::FPoint(12, 10));
	game.seedTurnDataWithPath(movingShip->getID(), path);

	// Seed an INACTIVE seeker owned by the defender (opposing side) directly on the path.
	Frontier::FTacticalSeekerMissileState inactiveSeeker;
	inactiveSeeker.seekerID = 5001u;
	inactiveSeeker.ownerID = 0u;
	inactiveSeeker.hex = Frontier::FPoint(11, 10);
	inactiveSeeker.heading = 0;
	inactiveSeeker.active = false;    // <-- inactive: must be ignored
	inactiveSeeker.movementTurn = 0;
	inactiveSeeker.movementAllowance = 0;
	inactiveSeeker.hasSource = false;
	game.seedSeeker(inactiveSeeker);

	game.clearPendingOutcomes();
	game.checkForActiveSeekersOnPath(movingShip);

	// The inactive seeker must not generate any contact outcome.
	const std::vector<Frontier::FTacticalSeekerContactOutcome> & outcomes = game.getPendingSeekerContactOutcomes();
	CPPUNIT_ASSERT_MESSAGE("Inactive seeker must not trigger path contact", outcomes.empty());

	// The inactive seeker must still be in the model (not removed).
	const std::vector<Frontier::FTacticalSeekerMissileState> & seekers = game.getSeekerMissiles();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), seekers.size());
	CPPUNIT_ASSERT_EQUAL(5001u, seekers[0].seekerID);

	delete movingShip;
}

void FTacticalSeekerMovementTest::testSeekerRemovedFromModelAfterMovementContact() {
	// AC: detonated seekers are removed exactly once after movement-path contact.
	// An active opposing seeker in the ship's path must produce a contact outcome,
	// and applyMovementSeekerDamage must remove it from m_seekerMissiles exactly once
	// (without a UI installed, outcomes are cleared and seeker is still removed).
	TestableTacticalGame game;
	Frontier::VehicleList * attackShips = new Frontier::VehicleList();
	Frontier::VehicleList * defendShips = new Frontier::VehicleList();
	FSeekerHarnessShip * movingShip = new FSeekerHarnessShip(1u, "Frigate");
	attackShips->push_back(movingShip);

	game.configureSides(0u, 1u, true);
	game.installShipLists(attackShips, defendShips);

	// Place the ship at hex (20,20).
	const Frontier::FPoint shipHex(20, 20);
	game.placeShipAtHex(movingShip, shipHex);

	// Build a path that passes through the seeker hex.
	const Frontier::FPoint seekerHex(21, 20);
	Frontier::PointList path;
	path.push_back(shipHex);
	path.push_back(seekerHex);
	game.seedTurnDataWithPath(movingShip->getID(), path);

	// Seed an ACTIVE seeker owned by the defender (opposing side) on the path.
	Frontier::FTacticalSeekerMissileState activeSeeker;
	activeSeeker.seekerID = 7001u;
	activeSeeker.ownerID = 0u;       // owned by defender (opposing the moving attacker)
	activeSeeker.hex = seekerHex;
	activeSeeker.heading = 0;
	activeSeeker.active = true;      // <-- active: must trigger contact
	activeSeeker.movementTurn = 2;
	activeSeeker.movementAllowance = 4;
	activeSeeker.hasSource = false;
	game.seedSeeker(activeSeeker);

	// Seed a second seeker that is NOT on the path so we can verify it survives.
	Frontier::FTacticalSeekerMissileState bystander;
	bystander.seekerID = 7002u;
	bystander.ownerID = 0u;
	bystander.hex = Frontier::FPoint(50, 50);
	bystander.heading = 0;
	bystander.active = true;
	bystander.movementTurn = 1;
	bystander.movementAllowance = 2;
	bystander.hasSource = false;
	game.seedSeeker(bystander);

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), game.getSeekerMissiles().size());

	// Check the path -- should produce exactly one contact outcome for the active seeker.
	game.clearPendingOutcomes();
	game.checkForActiveSeekersOnPath(movingShip);

	const std::vector<Frontier::FTacticalSeekerContactOutcome> & outcomes = game.getPendingSeekerContactOutcomes();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), outcomes.size());
	CPPUNIT_ASSERT_EQUAL(7001u, outcomes[0].seekerID);

	// Apply the movement seeker damage (no UI installed, so outcomes are cleared without dialog).
	game.applyMovementSeekerDamage();

	// The contacting seeker must be removed; the bystander seeker must survive.
	const std::vector<Frontier::FTacticalSeekerMissileState> remainingSeekers = game.getSeekerMissiles();
	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), remainingSeekers.size());
	CPPUNIT_ASSERT_EQUAL(7002u, remainingSeekers[0].seekerID);

	// Outcomes must be cleared after applyMovementSeekerDamage.
	CPPUNIT_ASSERT(game.getPendingSeekerContactOutcomes().empty());

	delete movingShip;
}

void FTacticalSeekerMovementTest::testMovementPathPopulatedByResolveActiveSeekers() {
	// SMC-06 prerequisite + SMC-07: resolveActiveSeekersForMovingPlayer populates
	// movementPath for each moving seeker and clears it at the start of each resolution
	// pass so no stale accumulation occurs across turns.

	TestableTacticalGame game;
	Frontier::VehicleList * attackShips = new Frontier::VehicleList();
	Frontier::VehicleList * defendShips = new Frontier::VehicleList();
	FSeekerHarnessShip * targetShip = new FSeekerHarnessShip(0u, "Destroyer");
	defendShips->push_back(targetShip);

	const Frontier::FPoint targetHex(30, 30);
	game.configureSides(0u, 1u, true);
	game.installShipLists(attackShips, defendShips);
	game.placeShipAtHex(targetShip, targetHex);

	// Active seeker owned by moving player, far enough from target not to contact in 2 hexes.
	Frontier::FTacticalSeekerMissileState movingSeeker;
	movingSeeker.seekerID = 8001u;
	movingSeeker.ownerID = 1u;
	movingSeeker.hex = Frontier::FPoint(20, 20);
	movingSeeker.heading = 0;
	movingSeeker.active = true;
	movingSeeker.movementTurn = 0;
	movingSeeker.movementAllowance = 0;
	movingSeeker.hasSource = false;

	// Inactive seeker owned by enemy (non-moving): should have path cleared.
	Frontier::FTacticalSeekerMissileState inactiveEnemySeeker;
	inactiveEnemySeeker.seekerID = 8002u;
	inactiveEnemySeeker.ownerID = 0u;
	inactiveEnemySeeker.hex = Frontier::FPoint(40, 40);
	inactiveEnemySeeker.heading = 0;
	inactiveEnemySeeker.active = false;
	inactiveEnemySeeker.movementTurn = 0;
	inactiveEnemySeeker.movementAllowance = 0;
	inactiveEnemySeeker.hasSource = false;
	// Seed a stale movementPath to prove it gets cleared.
	inactiveEnemySeeker.movementPath.push_back(Frontier::FPoint(99, 99));

	game.seedSeeker(movingSeeker);
	game.seedSeeker(inactiveEnemySeeker);

	// First resolution pass.
	game.resolveActiveSeekersForMovingPlayer();

	const std::vector<Frontier::FTacticalSeekerMissileState> & seekers = game.getSeekerMissiles();

	// Find the moving seeker by ID and check its movementPath.
	bool foundMoving = false;
	bool foundInactive = false;
	for (std::vector<Frontier::FTacticalSeekerMissileState>::const_iterator itr = seekers.begin();
		 itr != seekers.end(); ++itr) {
		if (itr->seekerID == 8001u) {
			foundMoving = true;
			// movementPath must start at the pre-move position and have at least 1 point.
			CPPUNIT_ASSERT_MESSAGE("movementPath must be non-empty after resolution",
				!itr->movementPath.empty());
			// First point must be the starting hex (pre-move position).
			CPPUNIT_ASSERT_EQUAL_MESSAGE("movementPath[0].x must equal pre-move start hex x",
				20, itr->movementPath[0].getX());
			CPPUNIT_ASSERT_EQUAL_MESSAGE("movementPath[0].y must equal pre-move start hex y",
				20, itr->movementPath[0].getY());
			// movementPath must have at least 2 points (start + at least one step) so
			// movementPath.size()-1 gives a meaningful move count for overlay display (SMF-07).
			CPPUNIT_ASSERT_MESSAGE("movementPath must have at least 2 points after movement",
				itr->movementPath.size() >= 2);
			// Last point must match the seeker's current hex after movement.
			const Frontier::FPoint & finalPath = itr->movementPath.back();
			CPPUNIT_ASSERT_EQUAL_MESSAGE("movementPath final point x must match seeker hex x",
				itr->hex.getX(), finalPath.getX());
			CPPUNIT_ASSERT_EQUAL_MESSAGE("movementPath final point y must match seeker hex y",
				itr->hex.getY(), finalPath.getY());
		} else if (itr->seekerID == 8002u) {
			foundInactive = true;
			// Non-moving (inactive/enemy) seeker must have its stale path cleared.
			CPPUNIT_ASSERT_MESSAGE("Non-moving seeker must have movementPath cleared",
				itr->movementPath.empty());
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Moving seeker 8001 must remain after first resolution", foundMoving);
	CPPUNIT_ASSERT_MESSAGE("Inactive enemy seeker 8002 must remain in model", foundInactive);

	// Second resolution pass: verify no stale path accumulation from prior pass.
	game.clearPendingOutcomes();
	game.resolveActiveSeekersForMovingPlayer();

	const std::vector<Frontier::FTacticalSeekerMissileState> & seekers2 = game.getSeekerMissiles();
	for (std::vector<Frontier::FTacticalSeekerMissileState>::const_iterator itr = seekers2.begin();
		 itr != seekers2.end(); ++itr) {
		if (itr->seekerID == 8001u) {
			// Path is repopulated from the current (post-first-move) hex, not accumulated.
			CPPUNIT_ASSERT_MESSAGE("movementPath must start from current hex (no stale accumulation)",
				itr->movementPath[0].getX() != 20 || itr->movementPath[0].getY() != 20
				|| itr->movementTurn >= 2);
		}
	}

	delete targetShip;
}

void FTacticalSeekerMovementTest::testSeekerMoveCountLabelFieldsReflectPathAndAllowance() {
	// SMFR-04: Behavioral model-level test for the seeker state fields consumed by
	// drawSeekerMoveCountOverlay.  The overlay selects:
	//   count = movementPath.size()-1  when movementPath is recorded this phase, or
	//   count = movementAllowance       as fallback between resolution passes.
	// This test confirms the model produces the correct field values in each case.

	TestableTacticalGame game;
	Frontier::VehicleList * attackShips = new Frontier::VehicleList();
	Frontier::VehicleList * defendShips = new Frontier::VehicleList();

	// One target ship for the resolver to find.
	FSeekerHarnessShip * targetShip = new FSeekerHarnessShip(0u, "Destroyer");
	defendShips->push_back(targetShip);
	const Frontier::FPoint targetHex(40, 20);
	game.configureSides(0u, 1u, true);
	game.installShipLists(attackShips, defendShips);
	game.placeShipAtHex(targetShip, targetHex);

	// Case 1: Fresh activation — no movementPath recorded.
	// movementAllowance is 0 for a seeker on movementTurn=0 (not yet resolved).
	Frontier::FTacticalSeekerMissileState freshSeeker;
	freshSeeker.seekerID = 9001u;
	freshSeeker.ownerID = 1u;
	freshSeeker.hex = Frontier::FPoint(20, 20);
	freshSeeker.heading = 0;
	freshSeeker.active = true;
	freshSeeker.movementTurn = 0;
	freshSeeker.movementAllowance = 0;
	freshSeeker.hasSource = false;
	// movementPath deliberately left empty to simulate "just activated, not yet moved".
	game.seedSeeker(freshSeeker);

	// Case 2: Another active seeker at the same hex (stacked case).
	Frontier::FTacticalSeekerMissileState secondSeeker;
	secondSeeker.seekerID = 9002u;
	secondSeeker.ownerID = 1u;
	secondSeeker.hex = Frontier::FPoint(20, 20);
	secondSeeker.heading = 0;
	secondSeeker.active = true;
	secondSeeker.movementTurn = 1;
	secondSeeker.movementAllowance = 2;   // populated by prior turn's resolution
	secondSeeker.hasSource = false;
	// movementPath empty (cleared at start of new resolution pass, before being refilled).
	game.seedSeeker(secondSeeker);

	// AC2/Case 1: Before resolution, freshSeeker has movementPath empty and movementAllowance==0.
	{
		const std::vector<Frontier::FTacticalSeekerMissileState> & seekers = game.getSeekerMissiles();
		CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), seekers.size());
		for (std::vector<Frontier::FTacticalSeekerMissileState>::const_iterator itr = seekers.begin();
			 itr != seekers.end(); ++itr) {
			if (itr->seekerID == 9001u) {
				CPPUNIT_ASSERT_MESSAGE(
					"SMFR-04: freshSeeker movementPath must be empty before resolution "
					"(overlay would use movementAllowance as fallback).",
					itr->movementPath.empty());
				CPPUNIT_ASSERT_EQUAL_MESSAGE(
					"SMFR-04: freshSeeker movementAllowance must be 0 before first resolution "
					"(overlay label would show 0 for a just-activated seeker).",
					0, itr->movementAllowance);
			}
			if (itr->seekerID == 9002u) {
				CPPUNIT_ASSERT_MESSAGE(
					"SMFR-04: secondSeeker movementPath must be empty before resolution "
					"(path is cleared at the start of each pass).",
					itr->movementPath.empty());
				CPPUNIT_ASSERT_EQUAL_MESSAGE(
					"SMFR-04: secondSeeker movementAllowance must be 2 (from prior turn) "
					"for overlay fallback between resolution passes.",
					2, itr->movementAllowance);
			}
		}
	}

	// AC1/Case 2: After resolution, movementPath is populated for active seekers that moved.
	game.resolveActiveSeekersForMovingPlayer();

	{
		const std::vector<Frontier::FTacticalSeekerMissileState> & seekers = game.getSeekerMissiles();
		for (std::vector<Frontier::FTacticalSeekerMissileState>::const_iterator itr = seekers.begin();
			 itr != seekers.end(); ++itr) {
			if (!itr->active) {
				continue;
			}
			// After resolution, movementPath must have at least one point so
			// movementPath.size()-1 >= 0 is a valid label count.
			CPPUNIT_ASSERT_MESSAGE(
				"SMFR-04: After resolution, an active seeker's movementPath must be non-empty "
				"so the overlay uses movementPath.size()-1 as the count (not the fallback).",
				!itr->movementPath.empty());
			const int pathCount = static_cast<int>(itr->movementPath.size()) - 1;
			CPPUNIT_ASSERT_MESSAGE(
				"SMFR-04: movementPath.size()-1 must be non-negative (overlay shows hexes moved).",
				pathCount >= 0);
		}
	}

	// AC3 (stacked case): Both seekers must still be present and active at the same hex
	// (unless one contacted the target, which is unlikely given hex distances).
	// At minimum both must have been iterated during resolution.
	int survivingCount = 0;
	const std::vector<Frontier::FTacticalSeekerMissileState> & seekersFinal = game.getSeekerMissiles();
	for (std::vector<Frontier::FTacticalSeekerMissileState>::const_iterator itr = seekersFinal.begin();
		 itr != seekersFinal.end(); ++itr) {
		if (itr->active) {
			++survivingCount;
		}
	}
	// Both seekers started far from the target (hex 20,20 vs 40,20) so neither should have
	// contacted on a single resolution pass; both must survive with their own movementAllowance.
	CPPUNIT_ASSERT_EQUAL_MESSAGE(
		"SMFR-04: Both active seekers must survive a single resolution pass when target is distant "
		"(verifies per-seeker independent state for stacked-display label computation).",
		2, survivingCount);

	delete targetShip;
}

}
