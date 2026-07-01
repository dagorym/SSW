/**
 * @file FTacticalGameMechanicsTest.cpp
 * @brief Implementation tests for FTacticalGame additive mechanics container
 */

#include "FTacticalGameMechanicsTest.h"

#include <cstdlib>
#include <fstream>
#include <iterator>

#include "ships/FVehicle.h"
#include "strategic/FFleet.h"
#include "tactical/FTacticalGame.h"

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalGameMechanicsTest );

void FTacticalGameMechanicsTest::setUp() {
}

void FTacticalGameMechanicsTest::tearDown() {
}

std::string FTacticalGameMechanicsTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalGameMechanicsTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

void FTacticalGameMechanicsTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalGameMechanicsTest::testFTacticalGameImplementationCompilesStandalone() {
// AC: src/tactical/FTacticalGame.cpp exists and compiles cleanly.
const std::string sourcePath = repoFile("src/tactical/FTacticalGame.cpp");
const std::string includePath = repoFile("include");
const std::string cmd =
"g++ -x c++ -I" + includePath + " -c " + sourcePath + " -o /dev/null";
const int rc = std::system(cmd.c_str());
CPPUNIT_ASSERT_EQUAL(0, rc);
}

void FTacticalGameMechanicsTest::testHeaderExposesAdditiveMechanicsApiSurface() {
// AC: additive mechanics methods exist for setup/state, movement, report lifecycle, fire, and winner helpers.
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));

assertContains(header, "} FTacticalOrdnanceSource;");
assertContains(header, "unsigned int shipID;");
assertContains(header, "int weaponIndex;");
assertContains(header, "unsigned int weaponID;");
assertContains(header, "} FTacticalPlacedOrdnance;");
assertContains(header, "FWeapon::Weapon weaponType;");
assertContains(header, "unsigned int ownerID;");
assertContains(header, "FTacticalOrdnanceSource source;");
assertContains(header, "FPoint hex;");
assertContains(header, "int displayColorIndex;");
assertContains(header, "int displayMarkerIndex;");
assertContains(header, "} FTacticalSeekerMissileState;");
assertContains(header, "unsigned int seekerID;");
assertContains(header, "int heading;");
assertContains(header, "bool active;");
assertContains(header, "int movementTurn;");
assertContains(header, "int movementAllowance;");
assertContains(header, "bool hasSource;");
assertContains(header, "int setupFleets(FleetList * aList, FleetList * dList, bool planet = false, FVehicle * station = NULL);");
assertContains(header, "void setState(int s);");
assertContains(header, "void setPhase(int p);");
assertContains(header, "void resetMovementState();");
assertContains(header, "void finalizeMovementState();");
assertContains(header, "void clearMovementHighlights();");
assertContains(header, "} FTacticalMovePreviewRoute;");
assertContains(header, "void resetTurnInfoForCurrentMover();");
assertContains(header, "FTacticalTurnData * findTurnData(unsigned int shipID);");
assertContains(header, "void beginTacticalReport(const FTacticalCombatReportContext & context);");
assertContains(header, "void appendTacticalAttackReport(const FTacticalAttackReport & attack);");
assertContains(header, "void appendTacticalReportEvent(const FTacticalReportEvent & event);");
assertContains(header, "FTacticalCombatReportSummary buildCurrentTacticalReportSummary() const;");
assertContains(header, "void clearTacticalReport();");
assertContains(header, "FTacticalCombatReportSummary fireAllWeapons();");
assertContains(header, "int clearDestroyedShips();");
assertContains(header, "bool isCombatOver() const;");
assertContains(header, "bool hasWinner() const { return m_hasWinner; }");
assertContains(header, "unsigned int getWinnerID() const { return m_winnerID; }");
assertContains(header, "void clearWinner();");
assertContains(header, "const std::vector<FTacticalPlacedOrdnance> & getPlacedOrdnance() const { return m_placedOrdnance; }");
assertContains(header, "const std::vector<FTacticalSeekerMissileState> & getSeekerMissiles() const { return m_seekerMissiles; }");
assertContains(header, "std::vector<FTacticalPlacedOrdnance> getPlacedOrdnanceAtHex(const FPoint & hex) const;");
assertContains(header, "std::vector<FTacticalSeekerMissileState> getSeekerMissilesAtHex(const FPoint & hex, bool activeOnly = false) const;");
assertContains(header, "std::vector<FTacticalSeekerMissileState> getSeekerMissilesForOwner(unsigned int ownerID, bool activeOnly = false) const;");
assertContains(header, "std::vector<FTacticalPlacedOrdnance> m_placedOrdnance;");
assertContains(header, "std::vector<FTacticalSeekerMissileState> m_seekerMissiles;");
}

void FTacticalGameMechanicsTest::testResetInitializesSafeLegacyCompatibleDefaults() {
// AC: implementation initializes tactical state to safe defaults equivalent to legacy defaults.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "void FTacticalGame::reset()");

assertContains(body, "m_state = BS_Unknown;");
assertContains(body, "m_phase = PH_NONE;");
assertContains(body, "m_activePlayer = false;");
assertContains(body, "m_movingPlayer = true;");
assertContains(body, "m_moveComplete = true;");
assertContains(body, "m_done = false;");
assertContains(body, "m_hasWinner = false;");
assertContains(body, "m_winnerID = 0;");
assertContains(body, "m_planetChoice = -1;");
assertContains(body, "m_planetPos.setPoint(-1, -1);");
assertContains(body, "m_stationPos.setPoint(-1, -1);");
assertContains(body, "m_station = NULL;");
assertContains(body, "m_playerID[0] = 0;");
assertContains(body, "m_playerID[1] = 1;");
assertContains(body, "m_curShip = NULL;");
assertContains(body, "m_curWeapon = NULL;");
assertContains(body, "clearICMVector(m_ICMData);");
assertContains(body, "m_tacticalReport.clear();");
assertContains(body, "m_turnInfo.clear();");
assertContains(body, "m_mineOwner = 99;");
assertContains(body, "m_placedOrdnance.clear();");
assertContains(body, "m_seekerMissiles.clear();");
}

void FTacticalGameMechanicsTest::testSeekerActivationPhaseEntryAndAutoSkipFlow() {
// AC: movement phase entry routes through seeker activation and auto-skip still resolves active seekers.
	const std::string frontierHeader = readFile(repoFile("include/Frontier.h"));
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
	const std::string setPhaseBody = extractFunctionBody(source, "void FTacticalGame::setPhase(int p)");
	const std::string beginSeekerBody = extractFunctionBody(source, "void FTacticalGame::beginSeekerActivationPhase()");
	const std::string beginMoveBody = extractFunctionBody(source, "void FTacticalGame::beginMovePhase()");
	const std::string resolveActiveBody = extractFunctionBody(source, "void FTacticalGame::resolveActiveSeekersForMovingPlayer()");

	assertContains(frontierHeader, "PH_SET_SPEED,");
	assertContains(frontierHeader, "PH_SEEKER_ACTIVATION,");
	assertContains(frontierHeader, "PH_MOVE,");
	CPPUNIT_ASSERT(frontierHeader.find("PH_SEEKER_ACTIVATION,") < frontierHeader.find("PH_MOVE,"));

	assertContains(setPhaseBody, "if (p == PH_MOVE) {");
	assertContains(setPhaseBody, "beginSeekerActivationPhase();");
	assertContains(setPhaseBody, "if (p == PH_SEEKER_ACTIVATION) {");
	assertContains(setPhaseBody, "m_selectedSeekerActivationHex.setPoint(-1, -1);");

	assertContains(beginSeekerBody, "const std::vector<FPoint> inactiveHexes = getInactiveSeekerActivationHexes();");
	assertContains(beginSeekerBody, "if (inactiveHexes.empty()) {");
	assertContains(beginSeekerBody, "resolveActiveSeekersForMovingPlayer();");
	assertContains(beginSeekerBody, "beginMovePhase();");
	assertContains(beginSeekerBody, "m_phase = PH_SEEKER_ACTIVATION;");
	assertContains(beginSeekerBody, "m_selectedSeekerActivationHex = inactiveHexes[0];");

	assertContains(beginMoveBody, "m_phase = PH_MOVE;");
	assertContains(beginMoveBody, "applyFireDamage();");
	assertContains(beginMoveBody, "toggleActivePlayer();");
	assertContains(beginMoveBody, "resetMovementState();");

	assertContains(resolveActiveBody, "TSM-004 placeholder seam:");
}

void FTacticalGameMechanicsTest::testTacticalReportLifecycleUsesSharedReportTypes() {
// AC: reuses FTacticalCombatReport types and lifecycle helpers without duplicate structures.
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));

assertContains(header, "#include \"tactical/FTacticalCombatReport.h\"");
assertContains(header, "FTacticalCombatReport m_tacticalReport;");
assertContains(source, "void FTacticalGame::beginTacticalReport(const FTacticalCombatReportContext & context)");
assertContains(source, "void FTacticalGame::appendTacticalAttackReport(const FTacticalAttackReport & attack)");
assertContains(source, "void FTacticalGame::appendTacticalReportEvent(const FTacticalReportEvent & event)");
assertContains(source, "FTacticalCombatReportSummary FTacticalGame::buildCurrentTacticalReportSummary() const");
assertContains(source, "return buildTacticalCombatReportSummary(m_tacticalReport);");
assertContains(source, "void FTacticalGame::clearTacticalReport()");
}

void FTacticalGameMechanicsTest::testFireAllWeaponsOwnsCombatReportLifecycleAndCleanup() {
// AC: fireAllWeapons owns report context, per-weapon fire aggregation, summary, and cleanup.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string body = extractFunctionBody(source, "FTacticalCombatReportSummary FTacticalGame::fireAllWeapons()");

assertContains(body, "if (m_phase == PH_DEFENSE_FIRE) {");
assertContains(body, "reportType = TRT_DefensiveFire;");
assertContains(body, "} else if (m_phase == PH_ATTACK_FIRE) {");
assertContains(body, "reportType = TRT_OffensiveFire;");
assertContains(body, "beginTacticalReport(context);");
assertContains(body, "fireICM();");
assertContains(body, "FTacticalAttackResult result = (*itr)->getWeapon(i)->fire();");
assertContains(body, "if (!result.fired()) {");
assertContains(body, "appendTacticalAttackReport(buildTacticalAttackReport(result));");
assertContains(body, "FTacticalCombatReportSummary summary = buildCurrentTacticalReportSummary();");
assertContains(body, "clearTacticalReport();");
assertContains(body, "clearDestroyedShips();");
assertContains(body, "clearICMVector(m_ICMData);");
assertContains(body, "return summary;");
}

void FTacticalGameMechanicsTest::testDestroyedShipBookkeepingLifecycleContractIsExplicit() {
// AC: model owns destroyed-ship capture and exposes explicit post-wx clear seam.
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string resetBody = extractFunctionBody(source, "void FTacticalGame::reset()");
const std::string fireBody = extractFunctionBody(source, "FTacticalCombatReportSummary FTacticalGame::fireAllWeapons()");
const std::string clearDestroyedBody = extractFunctionBody(source, "int FTacticalGame::clearDestroyedShips()");
const std::string clearBookkeepingBody = extractFunctionBody(source, "void FTacticalGame::clearLastDestroyedShipIDs()");

assertContains(header, "const std::vector<unsigned int> & getLastDestroyedShipIDs() const { return m_lastDestroyedShipIDs; }");
assertContains(header, "void clearLastDestroyedShipIDs();");
assertContains(header, "bool m_lastDestroyedShipIDsConsumed;");
assertContains(resetBody, "m_lastDestroyedShipIDsConsumed = true;");
assertContains(fireBody, "if (m_lastDestroyedShipIDsConsumed) {");
assertContains(fireBody, "m_lastDestroyedShipIDs.clear();");
assertContains(fireBody, "clearDestroyedShips();");
assertContains(clearDestroyedBody, "if (m_lastDestroyedShipIDsConsumed) {");
assertContains(clearDestroyedBody, "m_lastDestroyedShipIDs.clear();");
assertContains(clearDestroyedBody, "m_lastDestroyedShipIDsConsumed = false;");
assertContains(clearDestroyedBody, "m_lastDestroyedShipIDs.push_back((*itr)->getID());");
assertContains(clearBookkeepingBody, "m_lastDestroyedShipIDs.clear();");
assertContains(clearBookkeepingBody, "m_lastDestroyedShipIDsConsumed = true;");
}

void FTacticalGameMechanicsTest::testMovementHelpersResetAndFinalizeTurnData() {
// AC: movement-state helper methods reset and finalize movement/turn bookkeeping.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string resetMoveBody = extractFunctionBody(source, "void FTacticalGame::resetMovementState()");
const std::string finalizeBody = extractFunctionBody(source, "void FTacticalGame::finalizeMovementState()");
const std::string clearBody = extractFunctionBody(source, "void FTacticalGame::clearMovementHighlights()");
const std::string resetTurnInfoBody = extractFunctionBody(source, "void FTacticalGame::resetTurnInfoForCurrentMover()");

assertContains(resetMoveBody, "clearMovementHighlights();");
assertContains(resetMoveBody, "m_turnInfo.clear();");
assertContains(resetMoveBody, "m_gravityTurnFlag = false;");
assertContains(resetMoveBody, "m_gravityTurns.clear();");
assertContains(resetMoveBody, "setMoveComplete(false);");
assertContains(resetMoveBody, "resetTurnInfoForCurrentMover();");
assertContains(resetMoveBody, "checkMoveStatus();");

assertContains(clearBody, "m_movementHexes.clear();");
assertContains(clearBody, "m_leftHexes.clear();");
assertContains(clearBody, "m_rightHexes.clear();");
assertContains(clearBody, "clearStoppedShipPreviewRoutes();");
assertContains(clearBody, "m_targetHexes.clear();");
assertContains(clearBody, "m_headOnHexes.clear();");
assertContains(clearBody, "m_drawRoute = false;");
assertContains(clearBody, "m_moved = 0;");

assertContains(finalizeBody, "(*itr)->setSpeed(tItr->second.nMoved);");
assertContains(finalizeBody, "(*itr)->setHeading(tItr->second.curHeading);");
assertContains(finalizeBody, "(*itr)->setHeading(tItr->second.finalHeading);");
assertContains(finalizeBody, "tItr->second.hasMoved = true;");
assertContains(finalizeBody, "setMoveComplete(true);");

assertContains(resetTurnInfoBody, "d.hasMoved = false;");
assertContains(resetTurnInfoBody, "d.startHeading = d.curHeading;");
assertContains(resetTurnInfoBody, "d.finalHeading = d.curHeading;");
assertContains(resetTurnInfoBody, "d.nMoved = 0;");
assertContains(resetTurnInfoBody, "m_turnInfo[(*itr)->getID()] = d;");
}

void FTacticalGameMechanicsTest::testWinnerAndCombatEndHelpersResolveBattleState() {
// AC: end-of-combat/winner helper support exists for destroyed-ship cleanup and winner state.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string clearDestroyedBody = extractFunctionBody(source, "int FTacticalGame::clearDestroyedShips()");
const std::string isCombatOverBody = extractFunctionBody(source, "bool FTacticalGame::isCombatOver() const");
const std::string clearWinnerBody = extractFunctionBody(source, "void FTacticalGame::clearWinner()");

assertContains(clearDestroyedBody, "toggleActivePlayer();");
assertContains(clearDestroyedBody, "if ((*itr)->getHP() <= 0) {");
assertContains(clearDestroyedBody, "itr = sList->erase(itr);");
assertContains(clearDestroyedBody, "if (!liveShips) {");
assertContains(clearDestroyedBody, "m_hasWinner = true;");
assertContains(clearDestroyedBody, "m_winnerID = getActivePlayerID();");

assertContains(isCombatOverBody, "if (m_hasWinner) {");
assertContains(isCombatOverBody, "bool attackersAlive = false;");
assertContains(isCombatOverBody, "bool defendersAlive = false;");
assertContains(isCombatOverBody, "return !(attackersAlive && defendersAlive);");

assertContains(clearWinnerBody, "m_hasWinner = false;");
assertContains(clearWinnerBody, "m_winnerID = 0;");
}

void FTacticalGameMechanicsTest::testDestroyedShipCleanupPurgesHexAndTurnBookkeeping() {
// AC: clearDestroyedShips purges dead ships from side list, m_hexData occupancy, and m_turnInfo.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string clearDestroyedBody = extractFunctionBody(source, "int FTacticalGame::clearDestroyedShips()");
const std::string removeHexBody = extractFunctionBody(source, "void FTacticalGame::removeShipFromHexOccupancy(unsigned int shipID)");
const std::string removeTurnBody = extractFunctionBody(source, "void FTacticalGame::removeShipFromTurnInfo(unsigned int shipID)");
const std::string removeModelBody = extractFunctionBody(source, "void FTacticalGame::removeShipFromModelState(unsigned int shipID)");

assertContains(clearDestroyedBody, "removeShipFromModelState((*itr)->getID());");
assertContains(clearDestroyedBody, "itr = sList->erase(itr);");

assertContains(removeHexBody, "for (int i = 0; i < 100; ++i) {");
assertContains(removeHexBody, "for (int j = 0; j < 100; ++j) {");
assertContains(removeHexBody, "if ((*itr)->getID() == shipID) {");
assertContains(removeHexBody, "itr = ships.erase(itr);");

assertContains(removeTurnBody, "m_turnInfo.erase(shipID);");

assertContains(removeModelBody, "removeShipFromHexOccupancy(shipID);");
assertContains(removeModelBody, "removeShipFromTurnInfo(shipID);");
}

void FTacticalGameMechanicsTest::testFireICMCollectsOnlyActionableHexInterceptions() {
// AC: fireICM clears stale queue data, resolves via m_hexData scanning, and only enqueues actionable non-null interceptions.
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
const std::string fireIcmBody = extractFunctionBody(source, "void FTacticalGame::fireICM()");
const std::string findOccupantsBody = extractFunctionBody(source, "VehicleList * FTacticalGame::findHexOccupantsForShip(unsigned int shipID)");
const std::string hasUsableBody = extractFunctionBody(source, "bool FTacticalGame::hasUsableICMDefenderInHex(const VehicleList & vehicles, unsigned int defendingSideID) const");

assertContains(fireIcmBody, "clearICMVector(m_ICMData);");
assertContains(fireIcmBody, "VehicleList * targetHexShips = findHexOccupantsForShip(w->getTarget()->getID());");
assertContains(fireIcmBody, "if (targetHexShips == NULL) {");
assertContains(fireIcmBody, "if (!hasUsableICMDefenderInHex(*targetHexShips, w->getTarget()->getOwner())) {");
assertContains(fireIcmBody, "d->vehicles = targetHexShips;");
assertContains(fireIcmBody, "m_ICMData.push_back(d);");

assertContains(findOccupantsBody, "for (int i = 0; i < 100; ++i) {");
assertContains(findOccupantsBody, "for (int j = 0; j < 100; ++j) {");
assertContains(findOccupantsBody, "VehicleList & ships = m_hexData[i][j].ships;");
assertContains(findOccupantsBody, "if ((*itr)->getID() == shipID) {");
assertContains(findOccupantsBody, "return &ships;");

assertContains(hasUsableBody, "if (candidate->getOwner() != defendingSideID) {");
assertContains(hasUsableBody, "unsigned int index = candidate->hasDefense(FDefense::ICM);");
assertContains(hasUsableBody, "if (index == 0) {");
assertContains(hasUsableBody, "FDefense * defense = candidate->getDefense(index);");
assertContains(hasUsableBody, "if (defense == NULL) {");
assertContains(hasUsableBody, "if (defense->getAmmo()");
assertContains(hasUsableBody, "&& !candidate->isPowerSystemDamaged()");
assertContains(hasUsableBody, "&& !defense->isDamaged()) {");
	assertContains(hasUsableBody, "return true;");
}

void FTacticalGameMechanicsTest::testInteractionApisAndRendererAccessorsAreExposed() {
// AC: FTacticalGame exposes interaction and renderer-facing tactical state accessor APIs.
	const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
	const std::string buildPathHeadingsBody =
		extractFunctionBody(source, "void buildPathHeadings(const FTacticalTurnData & turnData, PointList & path, std::vector<int> & headings)");
	const std::string computeRangeBody = extractFunctionBody(source, "void FTacticalGame::computeWeaponRange()");
	const std::string setIfValidTargetBody = extractFunctionBody(source, "bool FTacticalGame::setIfValidTarget(FVehicle * target, const FPoint & targetHex)");
	const std::string ordnanceAtHexBody = extractFunctionBody(source, "std::vector<FTacticalPlacedOrdnance> FTacticalGame::getPlacedOrdnanceAtHex(const FPoint & hex) const");
	const std::string seekersAtHexBody = extractFunctionBody(source, "std::vector<FTacticalSeekerMissileState> FTacticalGame::getSeekerMissilesAtHex(");
	const std::string seekersForOwnerBody = extractFunctionBody(source, "std::vector<FTacticalSeekerMissileState> FTacticalGame::getSeekerMissilesForOwner(");

	assertContains(header, "bool selectWeapon(unsigned int weaponIndex);");
	assertContains(header, "bool selectDefense(unsigned int defenseIndex);");
	assertContains(header, "bool selectShipFromHex(const FPoint & hex);");
	assertContains(header, "bool handleHexClick(const FPoint & hex);");
	assertContains(header, "bool placePlanet(const FPoint & hex);");
	assertContains(header, "bool placeStation(const FPoint & hex);");
	assertContains(header, "bool placeShip(const FPoint & hex);");
	assertContains(header, "bool setShipPlacementHeading(int heading);");
	assertContains(header, "bool setShipPlacementHeadingByHex(const FPoint & hex);");
	assertContains(header, "bool beginMinePlacement();");
	assertContains(header, "bool beginOrdnancePlacement();");
	assertContains(header, "bool selectPlacementSource(unsigned int shipID, unsigned int weaponIndex);");
	assertContains(header, "bool selectPlacementSourceByIndex(unsigned int sourceIndex);");
	assertContains(header, "int getSelectedPlacementSourceIndex() const");
	assertContains(header, "const std::vector<FTacticalDeploymentSource> & getDeployablePlacementSources() const");
	assertContains(header, "void completeMinePlacement();");
	assertContains(header, "void completeMovePhase();");
	assertContains(header, "FTacticalCombatReportSummary resolveCurrentFirePhase();");
	assertContains(header, "void completeDefensiveFirePhase();");
	assertContains(header, "void completeOffensiveFirePhase();");
	assertContains(header, "void computeWeaponRange();");
	assertContains(header, "bool assignTargetFromHex(const FPoint & hex);");
	assertContains(header, "bool placeMineAtHex(const FPoint & hex);");
	assertContains(header, "bool placeOrdnanceAtHex(const FPoint & hex);");
	assertContains(header, "bool isHexDeployable(const FPoint & hex);");
	assertContains(header, "bool isHexMinable(const FPoint & hex);");

	assertContains(header, "const VehicleList & getHexOccupants(const FPoint & hex) const;");
	assertContains(header, "const std::vector<FPoint> & getMovementHexes() const");
	assertContains(header, "const std::vector<FPoint> & getLeftTurnHexes() const");
	assertContains(header, "const std::vector<FPoint> & getRightTurnHexes() const");
	assertContains(header, "const std::vector<FTacticalMovePreviewRoute> & getStoppedShipPreviewRoutes() const");
	assertContains(header, "const std::vector<int> & getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const;");
	assertContains(header, "const PointSet & getTargetHexes() const");
	assertContains(header, "const PointSet & getHeadOnHexes() const");
	assertContains(header, "const PointSet & getMinedHexes() const");
	assertContains(header, "const FHexMap & getMineTargets() const");
	assertContains(header, "unsigned int getMineOwner() const");
	assertContains(header, "const std::map<unsigned int, FTacticalTurnData> & getTurnInfo() const");
	assertContains(header, "bool hasShipPlacementPendingRotation() const");
	assertContains(header, "const FPoint & getSelectedShipHex() const");
	assertContains(header, "const VehicleList & getShipsWithMines() const");
	assertContains(header, "const std::vector<FTacticalPlacedOrdnance> & getPlacedOrdnance() const");
	assertContains(header, "const std::vector<FTacticalSeekerMissileState> & getSeekerMissiles() const");
	assertContains(header, "std::vector<FTacticalPlacedOrdnance> getPlacedOrdnanceAtHex(const FPoint & hex) const;");
	assertContains(header, "std::vector<FTacticalSeekerMissileState> getSeekerMissilesAtHex(const FPoint & hex, bool activeOnly = false) const;");
	assertContains(header, "std::vector<FTacticalSeekerMissileState> getSeekerMissilesForOwner(unsigned int ownerID, bool activeOnly = false) const;");
	assertContains(header, "bool isHexInBounds(const FPoint & hex) const;");
	assertContains(header, "bool isHexOccupied(const FPoint & hex) const;");

	assertContains(source, "bool FTacticalGame::selectWeapon(unsigned int weaponIndex)");
	assertContains(source, "bool FTacticalGame::selectDefense(unsigned int defenseIndex)");
	assertContains(source, "bool FTacticalGame::beginOrdnancePlacement()");
	assertContains(source, "bool FTacticalGame::selectPlacementSource(unsigned int shipID, unsigned int weaponIndex)");
	assertContains(source, "bool FTacticalGame::selectPlacementSourceByIndex(unsigned int sourceIndex)");
	assertContains(source, "bool FTacticalGame::handleHexClick(const FPoint & hex)");
	assertContains(source, "bool FTacticalGame::placeOrdnanceAtHex(const FPoint & hex)");
	assertContains(source, "bool FTacticalGame::isHexDeployable(const FPoint & hex)");
	assertContains(source, "const VehicleList & FTacticalGame::getHexOccupants(const FPoint & hex) const");
	assertContains(source, "std::vector<FTacticalPlacedOrdnance> FTacticalGame::getPlacedOrdnanceAtHex(const FPoint & hex) const");
	assertContains(source, "std::vector<FTacticalSeekerMissileState> FTacticalGame::getSeekerMissilesAtHex(");
	assertContains(source, "std::vector<FTacticalSeekerMissileState> FTacticalGame::getSeekerMissilesForOwner(");
	assertContains(source, "const std::vector<int> & FTacticalGame::getStoppedShipPreviewHeadingsForHex(const FPoint & hex) const");
	assertContains(source, "return EMPTY_PREVIEW_HEADING_LIST;");
	assertContains(ordnanceAtHexBody, "if (itr->hex.getX() == hex.getX() && itr->hex.getY() == hex.getY()) {");
	assertContains(ordnanceAtHexBody, "ordnance.push_back(*itr);");
	assertContains(seekersAtHexBody, "if (itr->hex.getX() == hex.getX() && itr->hex.getY() == hex.getY() && (!activeOnly || itr->active)) {");
	assertContains(seekersAtHexBody, "seekers.push_back(*itr);");
	assertContains(seekersForOwnerBody, "if (itr->ownerID == ownerID && (!activeOnly || itr->active)) {");
	assertContains(seekersForOwnerBody, "seekers.push_back(*itr);");
	assertContains(buildPathHeadingsBody, "headings.push_back(lastPoint ? turnData.finalHeading : heading);");
	assertContains(buildPathHeadingsBody, "heading = FHexMap::computeHeading(path[i], path[i + 1]);");
	assertContains(buildPathHeadingsBody, "heading = turnData.finalHeading;");
	assertContains(computeRangeBody, "buildPathHeadings(*turnData, path, headings);");
	assertContains(computeRangeBody, "computeFFRange(path[i], m_targetHexes, m_headOnHexes, headings[i]);");
	assertContains(setIfValidTargetBody, "buildPathHeadings(*turnData, path, headings);");
	assertContains(setIfValidTargetBody, "computeFFRange(path[i], targetSet, headOnSet, headings[i]);");
}

void FTacticalGameMechanicsTest::testHexClickDispatchAndTargetSelectionRulesFlowThroughModelState() {
// AC: model-side selection/click handling enforces range and state rules without wx dependencies.
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
	const std::string moveSelectionBody = extractFunctionBody(source, "bool FTacticalGame::handleMoveHexSelection(const FPoint & hex)");

	const std::string assignBody = extractFunctionBody(source, "bool FTacticalGame::assignTargetFromHex(const FPoint & hex)");
	const std::string placementByHexBody = extractFunctionBody(source, "bool FTacticalGame::setShipPlacementHeadingByHex(const FPoint & hex)");
	assertContains(assignBody, "if (!isHexInBounds(hex) || m_curWeapon == NULL) {");
	assertContains(assignBody, "if (occupants.size() == 0) {");
	assertContains(assignBody, "if (candidate == NULL || candidate->getOwner() == getActivePlayerID()) {");
	assertContains(assignBody, "return setIfValidTarget(candidate, hex);");

	assertContains(moveSelectionBody, "if (turnData->path.isPointOnPath(hex)) {");
	assertContains(moveSelectionBody, "turnData->path.removeTrailingPoints(hex)");
	assertContains(moveSelectionBody, "turnData->nMoved = m_moved;");
	assertContains(moveSelectionBody, "found = findHexInList(m_movementHexes, hex, moved);");
	assertContains(moveSelectionBody, "turnData->path.addPoint(*itr);");
	assertContains(moveSelectionBody, "turnData->nMoved += moved;");
	assertContains(moveSelectionBody, "turnData->finalHeading = turnData->curHeading;");
	assertContains(moveSelectionBody, "computeRemainingMoves(turnData->path.endPoint());");

	const std::string clickBody = extractFunctionBody(source, "bool FTacticalGame::handleHexClick(const FPoint & hex)");
	assertContains(clickBody, "case BS_SetupPlanet:");
	assertContains(clickBody, "return placePlanet(hex);");
	assertContains(clickBody, "case BS_SetupStation:");
	assertContains(clickBody, "return placeStation(hex);");
	assertContains(clickBody, "case BS_SetupDefendFleet:");
	assertContains(clickBody, "case BS_SetupAttackFleet:");
	assertContains(clickBody, "if (getControlState()) {");
	assertContains(clickBody, "if (m_setRotation) {");
	assertContains(clickBody, "return setShipPlacementHeading(m_curShip->getHeading());");
	assertContains(clickBody, "return placeShip(hex);");
	assertContains(clickBody, "case BS_PlaceMines:");
	assertContains(clickBody, "return placeMineAtHex(hex);");
	assertContains(clickBody, "case BS_Battle:");
	assertContains(clickBody, "if (getPhase() == PH_MOVE) {");
	assertContains(clickBody, "if (m_drawRoute && handleMoveHexSelection(hex)) {");
	assertContains(clickBody, "const bool selected = selectShipFromHex(hex);");
	assertContains(clickBody, "if (getPhase() == PH_DEFENSE_FIRE || getPhase() == PH_ATTACK_FIRE) {");
	assertContains(clickBody, "if (m_curWeapon != NULL && assignTargetFromHex(hex)) {");
	assertContains(clickBody, "if (selectShipFromHex(hex)) {");
	assertContains(clickBody, "if (m_curShip != NULL) {");
	assertContains(clickBody, "setWeapon(NULL);");

	assertContains(placementByHexBody, "if (m_curShip == NULL || !m_setRotation || m_shipPos.getX() < 0 || m_shipPos.getY() < 0) {");
	assertContains(placementByHexBody, "const int heading = FHexMap::computeHeading(m_shipPos, hex);");
	assertContains(placementByHexBody, "if (m_curShip->getHeading() == heading) {");
	assertContains(placementByHexBody, "m_curShip->setHeading(heading);");
	CPPUNIT_ASSERT(placementByHexBody.find("toggleControlState();") == std::string::npos);
	CPPUNIT_ASSERT(placementByHexBody.find("setPhase(PH_SET_SPEED);") == std::string::npos);
	CPPUNIT_ASSERT(placementByHexBody.find("m_setRotation = false;") == std::string::npos);
}

void FTacticalGameMechanicsTest::testMinePlacementAndMoveFireProgressionUpdateModelState() {
// AC: mine placement, movement completion, and fire-phase progression mutate model state used by renderers.
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));

	const std::string mineBody = extractFunctionBody(source, "bool FTacticalGame::placeMineAtHex(const FPoint & hex)");
	assertContains(mineBody, "if (m_curWeapon == NULL || m_curShip == NULL || m_curWeapon->getType() != FWeapon::M) {");
	assertContains(mineBody, "if (m_minedHexList.find(hex) == m_minedHexList.end()) {");
	assertContains(mineBody, "m_minedHexList.insert(hex);");
	assertContains(mineBody, "m_curWeapon->setCurrentAmmo(m_curWeapon->getAmmo() - 1);");
	assertContains(mineBody, "m_mineOwner = m_curShip->getOwner();");
	assertContains(mineBody, "if (getState() == BS_PlaceMines && m_curWeapon->getAmmo() != m_curWeapon->getMaxAmmo()) {");
	assertContains(mineBody, "m_minedHexList.erase(hex);");
	assertContains(mineBody, "m_curWeapon->setCurrentAmmo(m_curWeapon->getAmmo() + 1);");

	const std::string minableBody = extractFunctionBody(source, "bool FTacticalGame::isHexMinable(const FPoint & hex)");
	assertContains(minableBody, "FTacticalTurnData * turnData = findTurnData(m_curShip->getID());");
	assertContains(minableBody, "return turnData->path.isPointOnPath(hex);");

	const std::string moveBody = extractFunctionBody(source, "void FTacticalGame::completeMovePhase()");
	assertContains(moveBody, "finalizeMovementState();");
	assertContains(moveBody, "checkForMines(*itr);");
	assertContains(moveBody, "applyMineDamage();");
	assertContains(moveBody, "m_drawRoute = false;");
	assertContains(moveBody, "setPhase(PH_DEFENSE_FIRE);");
	assertContains(moveBody, "setShip(NULL);");

	const std::string defensiveBody = extractFunctionBody(source, "void FTacticalGame::completeDefensiveFirePhase()");
	assertContains(defensiveBody, "clearMovementHighlights();");
	assertContains(defensiveBody, "setWeapon(NULL);");
	assertContains(defensiveBody, "setPhase(PH_ATTACK_FIRE);");

	const std::string offensiveBody = extractFunctionBody(source, "void FTacticalGame::completeOffensiveFirePhase()");
	assertContains(offensiveBody, "clearMovementHighlights();");
	assertContains(offensiveBody, "setWeapon(NULL);");
	assertContains(offensiveBody, "toggleMovingPlayer();");
	assertContains(offensiveBody, "setPhase(PH_MOVE);");
}

void FTacticalGameMechanicsTest::testOrdnancePlacementSourceTrackingAndCompatibilityFlows() {
// AC: generalized placement supports mine/seeker sources, source-slot ammo accounting, and compatibility wrappers.
	const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
	const std::string rebuildSourcesBody = extractFunctionBody(source, "void FTacticalGame::rebuildDeployablePlacementSources()");
	const std::string beginOrdnanceBody = extractFunctionBody(source, "bool FTacticalGame::beginOrdnancePlacement()");
	const std::string selectByIndexBody = extractFunctionBody(source, "bool FTacticalGame::selectPlacementSourceByIndex(unsigned int sourceIndex)");
	const std::string selectBySlotBody = extractFunctionBody(source, "bool FTacticalGame::selectPlacementSource(unsigned int shipID, unsigned int weaponIndex)");
	const std::string placeMineFromSelectionBody = extractFunctionBody(source, "bool FTacticalGame::placeMineFromSelection(");
	const std::string placeSeekerFromSelectionBody = extractFunctionBody(source, "bool FTacticalGame::placeSeekerFromSelection(");
	const std::string placeOrdnanceBody = extractFunctionBody(source, "bool FTacticalGame::placeOrdnanceAtHex(const FPoint & hex)");
	const std::string removePlacedBody = extractFunctionBody(source, "bool FTacticalGame::removePlacedOrdnanceForSelection(");
	const std::string restoreAmmoBody = extractFunctionBody(source, "bool FTacticalGame::restoreAmmoForSource(const FTacticalOrdnanceSource & source)");
	const std::string sourceMatchesSelectionBody = extractFunctionBody(source, "bool FTacticalGame::sourceMatchesSelection(const FTacticalOrdnanceSource & source) const");
	const std::string deployableBody = extractFunctionBody(source, "bool FTacticalGame::isHexDeployable(const FPoint & hex)");
	const std::string beginMineBody = extractFunctionBody(source, "bool FTacticalGame::beginMinePlacement()");
	const std::string placeMineBody = extractFunctionBody(source, "bool FTacticalGame::placeMineAtHex(const FPoint & hex)");
	const std::string checkForMinesBody = extractFunctionBody(source, "void FTacticalGame::checkForMines(FVehicle * ship)");
	const std::string applyMineDamageBody = extractFunctionBody(source, "void FTacticalGame::applyMineDamage()");

	assertContains(header, "} FTacticalDeploymentSource;");
	assertContains(header, "FWeapon::Weapon weaponType;");
	assertContains(header, "std::vector<FTacticalDeploymentSource> m_deployablePlacementSources;");
	assertContains(header, "int m_selectedPlacementSource;");

	assertContains(rebuildSourcesBody, "if (!isDeployableWeapon(weapon)) {");
	assertContains(rebuildSourcesBody, "source.weaponType = weapon->getType();");
	assertContains(rebuildSourcesBody, "source.source.shipID = (*shipItr)->getID();");
	assertContains(rebuildSourcesBody, "source.source.weaponIndex = static_cast<int>(i);");
	assertContains(rebuildSourcesBody, "source.source.weaponID = weapon->getID();");
	assertContains(rebuildSourcesBody, "m_deployablePlacementSources.push_back(source);");
	assertContains(rebuildSourcesBody, "if (weapon->getType() == FWeapon::M) {");
	assertContains(rebuildSourcesBody, "m_shipsWithMines.push_back(*shipItr);");

	assertContains(beginOrdnanceBody, "rebuildDeployablePlacementSources();");
	assertContains(beginOrdnanceBody, "if (weapon != NULL && weapon->getAmmo() > 0) {");
	assertContains(beginOrdnanceBody, "if (!selectPlacementSourceByIndex(selectedIndex)) {");
	assertContains(beginOrdnanceBody, "setState(BS_PlaceMines);");

	assertContains(selectByIndexBody, "m_selectedPlacementSource = static_cast<int>(sourceIndex);");
	assertContains(selectByIndexBody, "setShip(ship);");
	assertContains(selectByIndexBody, "setWeapon(weapon);");
	assertContains(selectBySlotBody, "if (source.source.shipID == shipID");
	assertContains(selectBySlotBody, "&& source.source.weaponIndex == static_cast<int>(weaponIndex)) {");
	assertContains(selectBySlotBody, "return selectPlacementSourceByIndex(i);");

	assertContains(placeMineFromSelectionBody, "if (m_minedHexList.find(hex) == m_minedHexList.end()) {");
	assertContains(placeMineFromSelectionBody, "weapon->setCurrentAmmo(weapon->getAmmo() - 1);");
	assertContains(placeMineFromSelectionBody, "appendPlacedOrdnanceRecord(FWeapon::M, hex, selectedSource.source);");
	assertContains(placeMineFromSelectionBody, "rebuildDeployablePlacementSourcesFiltered(FWeapon::M);");
	assertContains(placeMineFromSelectionBody, "selectPlacementSource(selectedSource.source.shipID,");
	assertContains(placeMineFromSelectionBody, "static_cast<unsigned int>(selectedSource.source.weaponIndex));");
	assertContains(placeSeekerFromSelectionBody, "weapon->setCurrentAmmo(weapon->getAmmo() - 1);");
	assertContains(placeSeekerFromSelectionBody, "seeker.active = false;");
	assertContains(placeSeekerFromSelectionBody, "seeker.source = selectedSource.source;");
	assertContains(placeSeekerFromSelectionBody, "m_seekerMissiles.push_back(seeker);");
	assertContains(placeSeekerFromSelectionBody, "appendPlacedOrdnanceRecord(FWeapon::SM, hex, selectedSource.source);");
	assertContains(placeSeekerFromSelectionBody, "selectPlacementSource(selectedSource.source.shipID,");
	assertContains(placeSeekerFromSelectionBody, "static_cast<unsigned int>(selectedSource.source.weaponIndex));");

	assertContains(removePlacedBody, "if (!sourceMatchesSelection(itr->source)) {");
	assertContains(sourceMatchesSelectionBody, "if (weapon == m_curWeapon) {");
	assertContains(sourceMatchesSelectionBody, "if (weapon != NULL && weapon->getID() == m_curWeapon->getID()) {");
	assertContains(sourceMatchesSelectionBody, "return sourceMatchesWeapon(source, m_curShip, m_curWeapon, selectedWeaponIndex);");
	assertContains(restoreAmmoBody, "FWeapon * weapon = findWeaponBySource(source, NULL);");
	assertContains(restoreAmmoBody, "weapon->setCurrentAmmo(ammo + 1);");
	assertContains(placeOrdnanceBody, "if (removePlacedOrdnanceForSelection(hex, removed)) {");
	assertContains(placeOrdnanceBody, "if (!restoreAmmoForSource(removed.source)) {");
	assertContains(placeOrdnanceBody, "if (removed.weaponType == FWeapon::M) {");
	assertContains(placeOrdnanceBody, "m_minedHexList.erase(hex);");
	assertContains(placeOrdnanceBody, "} else if (removed.weaponType == FWeapon::SM) {");
	assertContains(placeOrdnanceBody, "&& !itr->active");
	assertContains(placeOrdnanceBody, "&& itr->source.weaponIndex == removed.source.weaponIndex");
	assertContains(placeOrdnanceBody, "m_seekerMissiles.erase(itr);");
	assertContains(placeOrdnanceBody, "rebuildDeployablePlacementSources();");
	assertContains(placeOrdnanceBody, "selectPlacementSource(selectedSource.source.shipID, selectedSource.source.weaponIndex);");
	assertContains(placeOrdnanceBody, "if (selectedSource.weaponType == FWeapon::M) {");
	assertContains(placeOrdnanceBody, "if (selectedSource.weaponType == FWeapon::SM) {");

	assertContains(deployableBody, "if (selectedSource.weaponType == FWeapon::M) {");
	assertContains(deployableBody, "return isHexMinable(hex);");
	assertContains(deployableBody, "return true;");

	assertContains(beginMineBody, "return beginOrdnancePlacement();");
	assertContains(placeMineBody, "return placeOrdnanceAtHex(hex);");

	assertContains(checkForMinesBody, "for (PointSet::iterator itr = m_minedHexList.begin(); itr != m_minedHexList.end(); ++itr) {");
	assertContains(applyMineDamageBody, "removePlacedMineRecordsAtHex(*itr);");
}

void FTacticalGameMechanicsTest::testSeekerActivationApisExposeSelectionAndOneWayActivation() {
// AC: model-owned seeker activation APIs expose stack selection and enforce one-way activation behavior.
	const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
	const std::string inactiveHexesBody = extractFunctionBody(source, "std::vector<FPoint> FTacticalGame::getInactiveSeekerActivationHexes() const");
	const std::string selectHexBody = extractFunctionBody(source, "bool FTacticalGame::selectSeekerActivationHex(const FPoint & hex)");
	const std::string selectedStackBody =
		extractFunctionBody(source, "std::vector<FTacticalSeekerMissileState> FTacticalGame::getSelectedInactiveSeekerActivationStack() const");
	const std::string activateBody = extractFunctionBody(source, "bool FTacticalGame::activateSelectedInactiveSeeker(unsigned int seekerID)");
	const std::string completeActivationBody = extractFunctionBody(source, "void FTacticalGame::completeSeekerActivationPhase()");

	assertContains(header, "std::vector<FPoint> getInactiveSeekerActivationHexes() const;");
	assertContains(header, "bool selectSeekerActivationHex(const FPoint & hex);");
	assertContains(header, "const FPoint & getSelectedSeekerActivationHex() const { return m_selectedSeekerActivationHex; }");
	assertContains(header, "std::vector<FTacticalSeekerMissileState> getSelectedInactiveSeekerActivationStack() const;");
	assertContains(header, "bool activateSelectedInactiveSeeker(unsigned int seekerID);");
	assertContains(header, "void completeSeekerActivationPhase();");
	assertContains(header, "FPoint m_selectedSeekerActivationHex;");

	assertContains(inactiveHexesBody, "if (itr->ownerID != getMovingPlayerID() || itr->active) {");
	assertContains(inactiveHexesBody, "if (uniqueHexes.insert(itr->hex).second) {");
	assertContains(inactiveHexesBody, "hexes.push_back(itr->hex);");

	assertContains(selectHexBody, "if (!isHexInBounds(hex)) {");
	assertContains(selectHexBody, "if (itr->ownerID == getMovingPlayerID()");
	assertContains(selectHexBody, "&& !itr->active");
	assertContains(selectHexBody, "m_selectedSeekerActivationHex = hex;");

	assertContains(selectedStackBody, "if (!isHexInBounds(m_selectedSeekerActivationHex)) {");
	assertContains(selectedStackBody, "if (itr->ownerID == getMovingPlayerID()");
	assertContains(selectedStackBody, "&& !itr->active");
	assertContains(selectedStackBody, "seekers.push_back(*itr);");

	assertContains(activateBody, "if (!isHexInBounds(m_selectedSeekerActivationHex)) {");
	assertContains(activateBody, "|| itr->active");
	assertContains(activateBody, "itr->active = true;");
	CPPUNIT_ASSERT(activateBody.find("itr->active = false;") == std::string::npos);

	assertContains(completeActivationBody, "if (m_phase != PH_SEEKER_ACTIVATION) {");
	assertContains(completeActivationBody, "resolveActiveSeekersForMovingPlayer();");
	assertContains(completeActivationBody, "m_selectedSeekerActivationHex.setPoint(-1, -1);");
	assertContains(completeActivationBody, "beginMovePhase();");
}

void FTacticalGameMechanicsTest::testStoppedShipFreeRotationGuardsAndFacingSelectionFlow() {
// AC: stopped-ship free rotation is gated to speed==0, nMoved==0, path length 1, and MR>0.
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
	const std::string freeRotationGuardBody =
		extractFunctionBody(source, "bool canUseStoppedShipFreeRotation(const FVehicle * ship, FTacticalTurnData * turnData)");
	const std::string setInitialRouteBody = extractFunctionBody(source, "void FTacticalGame::setInitialRoute()");
	const std::string remainingMovesBody = extractFunctionBody(source, "void FTacticalGame::computeRemainingMoves(FPoint start)");
	const std::string rebuildPreviewBody = extractFunctionBody(source, "void FTacticalGame::rebuildStoppedShipPreviewRoutes()");
	const std::string clearPreviewBody = extractFunctionBody(source, "void FTacticalGame::clearStoppedShipPreviewRoutes()");
	const std::string moveSelectionBody = extractFunctionBody(source, "bool FTacticalGame::handleMoveHexSelection(const FPoint & hex)");

	assertContains(freeRotationGuardBody, "turnData->speed == 0");
	assertContains(freeRotationGuardBody, "turnData->nMoved == 0");
	assertContains(freeRotationGuardBody, "turnData->path.getPathLength() == 1");
	assertContains(freeRotationGuardBody, "ship->getMR() > 0");

	assertContains(remainingMovesBody, "if (canUseStoppedShipFreeRotation(m_curShip, turnData)) {");
	assertContains(remainingMovesBody, "buildStoppedShipTurnOptions(start, forward, m_leftHexes, m_rightHexes);");
	assertContains(remainingMovesBody, "rebuildStoppedShipPreviewRoutes();");
	assertContains(setInitialRouteBody, "clearStoppedShipPreviewRoutes();");
	assertContains(setInitialRouteBody, "rebuildStoppedShipPreviewRoutes();");

	assertContains(clearPreviewBody, "m_stoppedShipPreviewRoutes.clear();");
	assertContains(clearPreviewBody, "m_stoppedShipPreviewHeadingsByHex.clear();");

	assertContains(rebuildPreviewBody, "if (!canUseStoppedShipFreeRotation(m_curShip, turnData)) {");
	assertContains(rebuildPreviewBody, "for (int heading = 0; heading < 6; ++heading) {");
	assertContains(rebuildPreviewBody, "preview.startHeading = heading;");
	assertContains(rebuildPreviewBody, "preview.facingHex = facingHex;");
	assertContains(rebuildPreviewBody, "preview.routeHexes.push_back(current);");
	assertContains(rebuildPreviewBody, "if (preview.routeHexes.empty()) {");
	assertContains(rebuildPreviewBody, "preview.routeHexes.push_back(facingHex);");
	assertContains(rebuildPreviewBody, "std::vector<int> & headings = m_stoppedShipPreviewHeadingsByHex[*itr];");
	assertContains(rebuildPreviewBody, "m_stoppedShipPreviewRoutes.push_back(preview);");
	CPPUNIT_ASSERT(rebuildPreviewBody.find("if (heading == turnData->curHeading)") == std::string::npos);

	assertContains(moveSelectionBody, "if (canUseStoppedShipFreeRotation(m_curShip, turnData)) {");
	assertContains(moveSelectionBody, "if (isAdjacentFacingSelection(m_shipPos, hex, selectedHeading)");
	assertContains(moveSelectionBody, "&& selectedHeading != turnData->curHeading) {");
	assertContains(moveSelectionBody, "turnData->startHeading = selectedHeading;");
	assertContains(moveSelectionBody, "turnData->curHeading = selectedHeading;");
	assertContains(moveSelectionBody, "turnData->finalHeading = selectedHeading;");
	assertContains(moveSelectionBody, "turnData->nMoved = 0;");
	assertContains(moveSelectionBody, "turnData->path.clear();");
	assertContains(moveSelectionBody, "turnData->path.addPoint(m_shipPos);");
	assertContains(moveSelectionBody, "m_moved = 0;");
	assertContains(moveSelectionBody, "computeRemainingMoves(m_shipPos);");
	assertContains(moveSelectionBody, "const std::vector<int> & previewHeadings = getStoppedShipPreviewHeadingsForHex(hex);");
	assertContains(moveSelectionBody, "const int originalStartHeading = turnData->startHeading;");
	assertContains(moveSelectionBody, "const FHexPath originalPath = turnData->path;");
	assertContains(moveSelectionBody, "for (std::vector<int>::const_iterator headingItr = previewHeadings.begin();");
	assertContains(moveSelectionBody, "turnData->startHeading = *headingItr;");
	assertContains(moveSelectionBody, "bool foundPreviewHex = findHexInList(m_movementHexes, hex, previewMoved);");
	assertContains(moveSelectionBody, "foundPreviewHex = findHexInList(m_leftHexes, hex, previewMoved);");
	assertContains(moveSelectionBody, "foundPreviewHex = findHexInList(m_rightHexes, hex, previewMoved);");
	assertContains(moveSelectionBody, "if (!resolvedPreviewHeading) {");
	assertContains(moveSelectionBody, "turnData->startHeading = originalStartHeading;");
	assertContains(moveSelectionBody, "turnData->path = originalPath;");
}

void FTacticalGameMechanicsTest::testImplementationRemainsSelfContainedWithoutLegacyWxRewire() {
// AC: implementation remains additive/self-contained and does not require FBattleScreen/Board/Display rewiring.
	const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));

CPPUNIT_ASSERT(header.find("#include \"tactical/FBattleScreen.h\"") == std::string::npos);
CPPUNIT_ASSERT(header.find("#include \"tactical/FBattleBoard.h\"") == std::string::npos);
CPPUNIT_ASSERT(header.find("#include \"tactical/FBattleDisplay.h\"") == std::string::npos);
CPPUNIT_ASSERT(source.find("#include \"tactical/FBattleScreen.h\"") == std::string::npos);
CPPUNIT_ASSERT(source.find("#include \"tactical/FBattleBoard.h\"") == std::string::npos);
CPPUNIT_ASSERT(source.find("#include \"tactical/FBattleDisplay.h\"") == std::string::npos);
CPPUNIT_ASSERT(source.find("FBattleScreen::") == std::string::npos);
CPPUNIT_ASSERT(source.find("FBattleBoard::") == std::string::npos);
CPPUNIT_ASSERT(source.find("FBattleDisplay::") == std::string::npos);
	CPPUNIT_ASSERT(header.find("#include <wx") == std::string::npos);
	CPPUNIT_ASSERT(header.find("#include \"wx/") == std::string::npos);
	CPPUNIT_ASSERT(source.find("#include <wx") == std::string::npos);
	CPPUNIT_ASSERT(source.find("#include \"wx/") == std::string::npos);
}

void FTacticalGameMechanicsTest::testSeekerDeploymentPhaseStateMachineTransitions() {
// AC: SMF-01 seeker deployment state machine - BS_PlaceSeekers state and transition contracts.
	const std::string frontier = readFile(repoFile("include/Frontier.h"));
	const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));
	const std::string battleScreenHeader = readFile(repoFile("include/tactical/FBattleScreen.h"));
	const std::string battleScreenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));

	// AC-1: BS_PlaceSeekers is declared in Frontier.h after BS_PlaceMines.
	const std::string::size_type placeMinesPos = frontier.find("BS_PlaceMines");
	const std::string::size_type placeSeekersPos = frontier.find("BS_PlaceSeekers");
	CPPUNIT_ASSERT_MESSAGE("BS_PlaceMines must be declared in Frontier.h", placeMinesPos != std::string::npos);
	CPPUNIT_ASSERT_MESSAGE("BS_PlaceSeekers must be declared in Frontier.h", placeSeekersPos != std::string::npos);
	CPPUNIT_ASSERT_MESSAGE("BS_PlaceSeekers must appear after BS_PlaceMines in Frontier.h", placeSeekersPos > placeMinesPos);

	// AC-2: beginOrdnancePlacement() erases SM sources before the ammo-check loop (mine-only filter).
	const std::string beginOrdnanceBody = extractFunctionBody(source, "bool FTacticalGame::beginOrdnancePlacement()");
	assertContains(beginOrdnanceBody, "rebuildDeployablePlacementSources();");
	// Filter removes non-M sources before the ammo-check loop.
	assertContains(beginOrdnanceBody, "weaponType != FWeapon::M");
	assertContains(beginOrdnanceBody, "setState(BS_PlaceMines);");
	// beginOrdnancePlacement returns false when source list is empty after filtering.
	assertContains(beginOrdnanceBody, "if (m_deployablePlacementSources.empty()) {");
	assertContains(beginOrdnanceBody, "return false;");

	// AC-3: beginSeekerPlacement() filters to SM only via rebuildDeployablePlacementSourcesFiltered.
	assertContains(source, "bool FTacticalGame::beginSeekerPlacement()");
	const std::string beginSeekerBody = extractFunctionBody(source, "bool FTacticalGame::beginSeekerPlacement()");
	assertContains(beginSeekerBody, "rebuildDeployablePlacementSourcesFiltered(FWeapon::SM);");
	assertContains(beginSeekerBody, "setState(BS_PlaceSeekers);");
	// beginSeekerPlacement returns false when no SM sources exist.
	assertContains(beginSeekerBody, "if (m_deployablePlacementSources.empty()) {");
	assertContains(beginSeekerBody, "return false;");

	// AC-4 & AC-5: completeMinePlacement() chains to BS_PlaceSeekers or skips to BS_SetupAttackFleet.
	assertContains(source, "void FTacticalGame::completeMinePlacement()");
	const std::string completeMinePlacementBody = extractFunctionBody(source, "void FTacticalGame::completeMinePlacement()");
	// Resets ship/weapon before attempting seeker placement.
	assertContains(completeMinePlacementBody, "setShip(NULL);");
	assertContains(completeMinePlacementBody, "setWeapon(NULL);");
	// Tries seeker placement; on failure, advances directly to BS_SetupAttackFleet.
	assertContains(completeMinePlacementBody, "beginSeekerPlacement()");
	assertContains(completeMinePlacementBody, "setState(BS_SetupAttackFleet);");
	assertContains(completeMinePlacementBody, "toggleActivePlayer();");
	// toggleActivePlayer is only called on BS_SetupAttackFleet branch (not on seeker-phase chain).
	// The implementation uses: if (!beginSeekerPlacement()) { setState(BS_SetupAttackFleet); toggleActivePlayer(); }
	// so both setState and toggleActivePlayer appear inside the false branch.

	// AC-6: completeSeekerPlacement() transitions to BS_SetupAttackFleet with toggleActivePlayer.
	assertContains(source, "void FTacticalGame::completeSeekerPlacement()");
	const std::string completeSeekerPlacementBody = extractFunctionBody(source, "void FTacticalGame::completeSeekerPlacement()");
	assertContains(completeSeekerPlacementBody, "setState(BS_SetupAttackFleet);");
	assertContains(completeSeekerPlacementBody, "toggleActivePlayer();");
	assertContains(completeSeekerPlacementBody, "setShip(NULL);");
	assertContains(completeSeekerPlacementBody, "setWeapon(NULL);");

	// AC-7: rebuildDeployablePlacementSourcesFiltered exists as protected helper.
	assertContains(header, "void rebuildDeployablePlacementSourcesFiltered(FWeapon::Weapon filter);");
	assertContains(source, "void FTacticalGame::rebuildDeployablePlacementSourcesFiltered(FWeapon::Weapon filter)");
	const std::string rebuildFilteredBody = extractFunctionBody(source, "void FTacticalGame::rebuildDeployablePlacementSourcesFiltered(FWeapon::Weapon filter)");
	// Only sources whose weaponType matches filter are kept.
	assertContains(rebuildFilteredBody, "if (weapon->getType() != filter) {");
	assertContains(rebuildFilteredBody, "continue;");

	// AC-8: FBattleScreen public API exposes beginSeekerPlacement and completeSeekerPlacement.
	assertContains(battleScreenHeader, "bool beginSeekerPlacement();");
	assertContains(battleScreenHeader, "void completeSeekerPlacement();");

	// AC-8 (delegation): FBattleScreen::beginSeekerPlacement delegates to model and calls reDraw.
	assertContains(battleScreenSource, "bool FBattleScreen::beginSeekerPlacement()");
	const std::string bsBeginSeekerBody = extractFunctionBody(battleScreenSource, "bool FBattleScreen::beginSeekerPlacement()");
	assertContains(bsBeginSeekerBody, "m_tacticalGame->beginSeekerPlacement()");
	assertContains(bsBeginSeekerBody, "reDraw()");

	// AC-8 (delegation): FBattleScreen::completeSeekerPlacement delegates to model and calls reDraw.
	assertContains(battleScreenSource, "void FBattleScreen::completeSeekerPlacement()");
	const std::string bsCompleteSeekerBody = extractFunctionBody(battleScreenSource, "void FBattleScreen::completeSeekerPlacement()");
	assertContains(bsCompleteSeekerBody, "m_tacticalGame->completeSeekerPlacement()");
	assertContains(bsCompleteSeekerBody, "reDraw()");

	// AC-9: No wx headers in FTacticalGame model files (reinforces testImplementationRemainsSelfContainedWithoutLegacyWxRewire).
	CPPUNIT_ASSERT(header.find("#include <wx") == std::string::npos);
	CPPUNIT_ASSERT(header.find("#include \"wx/") == std::string::npos);
	CPPUNIT_ASSERT(source.find("#include <wx") == std::string::npos);
	CPPUNIT_ASSERT(source.find("#include \"wx/") == std::string::npos);
}

void FTacticalGameMechanicsTest::testSeekerActivationPhaseIndexStampingAndFiltering() {
// AC: SMF-04 per-activation-phase seeker tracking — field declaration, counter lifecycle,
//     activation stamping, and phase-filtered accessor.
	const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
	const std::string source = readFile(repoFile("src/tactical/FTacticalGame.cpp"));

	// AC-1: activationPhaseIndex field is declared inside FTacticalSeekerMissileState
	// (non-persisted, lives before the closing typedef tag).
	assertContains(header, "int activationPhaseIndex;");
	// The field must appear before the struct closing tag.
	const std::string::size_type fieldPos = header.find("int activationPhaseIndex;");
	const std::string::size_type structEndPos = header.find("} FTacticalSeekerMissileState;");
	CPPUNIT_ASSERT_MESSAGE(
		"activationPhaseIndex must appear before } FTacticalSeekerMissileState;",
		fieldPos < structEndPos);

	// AC-2: m_seekerActivationPhaseIndex private member is declared in the class.
	assertContains(header, "int m_seekerActivationPhaseIndex;");

	// AC-3: getActiveSeekersByMovingPlayerThisPhase() is declared as a public const method
	//        returning by value (not by reference) in FTacticalGame.
	assertContains(header, "std::vector<FTacticalSeekerMissileState> getActiveSeekersByMovingPlayerThisPhase() const;");

	// AC-4: reset() initializes m_seekerActivationPhaseIndex to 0.
	const std::string resetBody = extractFunctionBody(source, "void FTacticalGame::reset()");
	assertContains(resetBody, "m_seekerActivationPhaseIndex = 0;");

	// AC-5: beginSeekerActivationPhase() increments the counter before the rest of the phase setup.
	const std::string beginActivationBody =
		extractFunctionBody(source, "void FTacticalGame::beginSeekerActivationPhase()");
	assertContains(beginActivationBody, "++m_seekerActivationPhaseIndex;");

	// AC-6: activateSelectedInactiveSeeker() stamps activationPhaseIndex on the seeker.
	const std::string activateSelectedBody =
		extractFunctionBody(source, "bool FTacticalGame::activateSelectedInactiveSeeker(unsigned int seekerID)");
	assertContains(activateSelectedBody, "itr->activationPhaseIndex = m_seekerActivationPhaseIndex;");

	// AC-7: activateInactiveSeekerAtHex() stamps activationPhaseIndex on the seeker.
	// Note: this function uses targetItr (the lowest-seekerID candidate) rather than itr.
	const std::string activateAtHexBody =
		extractFunctionBody(source, "bool FTacticalGame::activateInactiveSeekerAtHex(const FPoint & hex)");
	assertContains(activateAtHexBody, "activationPhaseIndex = m_seekerActivationPhaseIndex;");
	// Also verify the existing one-way activation contract remains intact.
	assertContains(activateAtHexBody, "targetItr->active = true;");

	// AC-8: getActiveSeekersByMovingPlayerThisPhase() filters on activationPhaseIndex == m_seekerActivationPhaseIndex.
	const std::string thisPhaseBody =
		extractFunctionBody(source, "std::vector<FTacticalSeekerMissileState> FTacticalGame::getActiveSeekersByMovingPlayerThisPhase() const");
	assertContains(thisPhaseBody, "itr->ownerID == getMovingPlayerID()");
	assertContains(thisPhaseBody, "itr->active");
	assertContains(thisPhaseBody, "itr->activationPhaseIndex == m_seekerActivationPhaseIndex");
	assertContains(thisPhaseBody, "result.push_back(*itr);");

	// AC-9: getActiveSeekersByMovingPlayer() (unfiltered) remains unchanged — no phase filter.
	const std::string allActiveBody =
		extractFunctionBody(source, "std::vector<FTacticalSeekerMissileState> FTacticalGame::getActiveSeekersByMovingPlayer() const");
	CPPUNIT_ASSERT_MESSAGE(
		"getActiveSeekersByMovingPlayer() must not filter by activationPhaseIndex",
		allActiveBody.find("activationPhaseIndex") == std::string::npos);

	// AC-10: activationPhaseIndex is not written in any save() call (persistence compatible).
	// The field carries "not persisted" by design — check that the word does not appear in
	// any of the persistence-entry-point bodies present in the source.
	// We confirm by checking all occurrences of "activationPhaseIndex" in FTacticalGame.cpp
	// are limited to reset, beginSeekerActivationPhase, the two activate helpers, and the
	// filtering accessor — none of which are save/load methods.
	// Rather than reconstructing a save body (there may be none for this struct), we verify
	// that any "save" function bodies visible in the source do not reference the field.
	// Source-text search: find every line that mentions "save" and verify none of those
	// lines also reference "activationPhaseIndex" by checking the source-level constraint.
	const std::string::size_type savePos = source.find("void FTacticalGame::save(");
	if (savePos != std::string::npos) {
		const std::string saveBody = extractFunctionBody(source, "void FTacticalGame::save(");
		CPPUNIT_ASSERT_MESSAGE(
			"activationPhaseIndex must not be persisted in save()",
			saveBody.find("activationPhaseIndex") == std::string::npos);
	}
	// Regardless, confirm the struct comment marks it non-persisted.
	assertContains(header, "not persisted");
}

void FTacticalGameMechanicsTest::testFBattleScreenGetActiveSeekersByMovingPlayerThisPhaseDelegate() {
// AC: SMF-04 FBattleScreen read-only delegation for per-phase accessor.
	const std::string bsHeader = readFile(repoFile("include/tactical/FBattleScreen.h"));
	const std::string bsSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));

	// AC-1: Declaration is a const method returning by value (not by reference).
	assertContains(bsHeader,
		"std::vector<FTacticalSeekerMissileState> getActiveSeekersByMovingPlayerThisPhase() const;");

	// AC-2: Implementation delegates straight to the model method, no local logic.
	const std::string delegateBody =
		extractFunctionBody(bsSource,
			"std::vector<FTacticalSeekerMissileState> FBattleScreen::getActiveSeekersByMovingPlayerThisPhase() const");
	assertContains(delegateBody, "m_tacticalGame->getActiveSeekersByMovingPlayerThisPhase()");
	// The body should be a direct return with no filtering logic of its own.
	assertContains(delegateBody, "return");

	// AC-3: wx-free — FTacticalGame header and source carry no wx includes
	//        (FBattleScreen inherits from wx but the delegation path itself must not
	//         introduce wx into the model header; verify the model header is clean).
	const std::string tgHeader = readFile(repoFile("include/tactical/FTacticalGame.h"));
	CPPUNIT_ASSERT(tgHeader.find("#include <wx") == std::string::npos);
	CPPUNIT_ASSERT(tgHeader.find("#include \"wx/") == std::string::npos);
}

void FTacticalGameMechanicsTest::testPreGameOrdnancePlacementRecordingBehavior() {
// AC: SMFR-02 — clicking a valid hex during BS_PlaceMines and BS_PlaceSeekers
//     records placement, decrements ammo, and updates model state.
//     This behavioral test must fail against the regression (missing BS_PlaceSeekers
//     case in handleHexClick) and pass after the fix.
	using namespace Frontier;

	// Build a Minelayer fleet for the defending side.  The Minelayer carries
	// SM(x4) and M(x20) launchers so both placement phases can be exercised.
	FVehicle * minelayer = createShip("Minelayer");
	CPPUNIT_ASSERT_MESSAGE("Minelayer ship must be creatable", minelayer != NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Minelayer must have 4 weapons", static_cast<unsigned int>(4), minelayer->getWeaponCount());

	// Locate the mine launcher (M) and seeker launcher (SM) weapon slots.
	int mineLauncherIndex = -1;
	int seekerLauncherIndex = -1;
	for (unsigned int i = 0; i < minelayer->getWeaponCount(); ++i) {
		FWeapon * w = minelayer->getWeapon(i);
		if (w != NULL && w->getType() == FWeapon::M && mineLauncherIndex < 0) {
			mineLauncherIndex = static_cast<int>(i);
		}
		if (w != NULL && w->getType() == FWeapon::SM && seekerLauncherIndex < 0) {
			seekerLauncherIndex = static_cast<int>(i);
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Minelayer must have an M launcher", mineLauncherIndex >= 0);
	CPPUNIT_ASSERT_MESSAGE("Minelayer must have an SM launcher", seekerLauncherIndex >= 0);

	FWeapon * mineLauncher = minelayer->getWeapon(static_cast<unsigned int>(mineLauncherIndex));
	FWeapon * seekerLauncher = minelayer->getWeapon(static_cast<unsigned int>(seekerLauncherIndex));
	const int initialMineAmmo = mineLauncher->getAmmo();
	const int initialSeekerAmmo = seekerLauncher->getAmmo();
	CPPUNIT_ASSERT_MESSAGE("Mine launcher must start with ammo > 0", initialMineAmmo > 0);
	CPPUNIT_ASSERT_MESSAGE("Seeker launcher must start with ammo > 0", initialSeekerAmmo > 0);

	// Build fleets and initialize the tactical game.
	FFleet * defendFleet = new FFleet();
	minelayer->setOwner(0);
	defendFleet->addShip(minelayer);
	FleetList defendFleets;
	defendFleets.push_back(defendFleet);

	FFleet * attackFleet = new FFleet();
	FVehicle * attacker = createShip("AssaultScout");
	CPPUNIT_ASSERT_MESSAGE("AttackScout must be creatable", attacker != NULL);
	attacker->setOwner(1);
	attackFleet->addShip(attacker);
	FleetList attackFleets;
	attackFleets.push_back(attackFleet);

	FTacticalGame game;
	game.setupFleets(&attackFleets, &defendFleets, false, NULL);

	// Place ships so they have turn-data and occupancy registered.
	// The minelayer is on the defending side (owner 0 = defender).
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

	// Enter mine placement phase.  The defender is the active player (owner 0).
	// Reset active player to defender (owner 0) before entering placement.
	game.setActivePlayer(false);  // false = defender = owner 0

	const bool enteredMines = game.beginOrdnancePlacement();
	CPPUNIT_ASSERT_MESSAGE("beginOrdnancePlacement() must succeed with a Minelayer in fleet", enteredMines);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Model must be in BS_PlaceMines after beginOrdnancePlacement()",
		static_cast<int>(BS_PlaceMines), game.getState());
	CPPUNIT_ASSERT_MESSAGE("Mine launcher must be selected as current weapon after entry",
		game.getWeapon() != NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Selected weapon type must be M",
		FWeapon::M, game.getWeapon()->getType());

	// AC: Click a valid hex — handleHexClick must route to placeMineAtHex/placeOrdnanceAtHex.
	// Use a hex not occupied by any ship; (5, 5) is clear.
	const FPoint mineHex(5, 5);
	const bool mineClicked = game.handleHexClick(mineHex);
	CPPUNIT_ASSERT_MESSAGE("handleHexClick during BS_PlaceMines must return true for a valid hex", mineClicked);

	// AC: Mine launcher ammo must be decremented by one.
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Mine launcher ammo must decrement by 1 after placement",
		initialMineAmmo - 1, mineLauncher->getAmmo());

	// AC: The hex must appear in getMinedHexes().
	const PointSet & minedHexes = game.getMinedHexes();
	CPPUNIT_ASSERT_MESSAGE("Placed mine hex must appear in getMinedHexes()",
		minedHexes.find(mineHex) != minedHexes.end());

	// AC: A placed-ordnance record must be created.
	CPPUNIT_ASSERT_EQUAL_MESSAGE("getPlacedOrdnance() must contain exactly one record after one mine placement",
		static_cast<unsigned int>(1), static_cast<unsigned int>(game.getPlacedOrdnance().size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed ordnance record must have weapon type M",
		FWeapon::M, game.getPlacedOrdnance()[0].weaponType);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed ordnance record must reference the mine hex",
		mineHex.getX(), game.getPlacedOrdnance()[0].hex.getX());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed ordnance record must reference the mine hex Y",
		mineHex.getY(), game.getPlacedOrdnance()[0].hex.getY());

	// Advance to seeker placement via completeMinePlacement().
	game.completeMinePlacement();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("completeMinePlacement() must advance model to BS_PlaceSeekers",
		static_cast<int>(BS_PlaceSeekers), game.getState());
	CPPUNIT_ASSERT_MESSAGE("Seeker launcher must be selected as current weapon in BS_PlaceSeekers",
		game.getWeapon() != NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Selected weapon type in BS_PlaceSeekers must be SM",
		FWeapon::SM, game.getWeapon()->getType());

	// AC: Click a valid hex during BS_PlaceSeekers.
	// Use a different hex from the mine so placement is unambiguous.
	const FPoint seekerHex(7, 7);
	const bool seekerClicked = game.handleHexClick(seekerHex);
	CPPUNIT_ASSERT_MESSAGE("handleHexClick during BS_PlaceSeekers must return true (regression: missing case)", seekerClicked);

	// AC: Seeker launcher ammo must be decremented by one.
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker launcher ammo must decrement by 1 after placement",
		initialSeekerAmmo - 1, seekerLauncher->getAmmo());

	// AC: An inactive seeker record must be created in getSeekerMissiles().
	const std::vector<FTacticalSeekerMissileState> & seekers = game.getSeekerMissiles();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("getSeekerMissiles() must contain exactly one record after seeker placement",
		static_cast<unsigned int>(1), static_cast<unsigned int>(seekers.size()));
	CPPUNIT_ASSERT_MESSAGE("Placed seeker must be inactive",
		!seekers[0].active);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed seeker must be at the clicked hex X",
		seekerHex.getX(), seekers[0].hex.getX());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed seeker must be at the clicked hex Y",
		seekerHex.getY(), seekers[0].hex.getY());

	// AC: A second placed-ordnance record (SM type) must be appended.
	CPPUNIT_ASSERT_EQUAL_MESSAGE("getPlacedOrdnance() must contain two records after mine + seeker placement",
		static_cast<unsigned int>(2), static_cast<unsigned int>(game.getPlacedOrdnance().size()));
	const FTacticalPlacedOrdnance & seekerOrdnance = game.getPlacedOrdnance()[1];
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Second placed-ordnance record must have weapon type SM",
		FWeapon::SM, seekerOrdnance.weaponType);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Second placed-ordnance record must reference the seeker hex X",
		seekerHex.getX(), seekerOrdnance.hex.getX());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Second placed-ordnance record must reference the seeker hex Y",
		seekerHex.getY(), seekerOrdnance.hex.getY());

	// AC: Launcher ammo consistency — seeker ammo is now initialSeekerAmmo - 1,
	//     not re-incremented (the placed seeker is consumed from available ammo).
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker launcher ammo must stay decremented (consistent between placement and battle)",
		initialSeekerAmmo - 1, seekerLauncher->getAmmo());

	// Clean up heap-allocated objects.
	delete defendFleet;
	delete attackFleet;
}

void FTacticalGameMechanicsTest::testPreGameMinePlacementPreservesShipAfterBeginMinePlacement() {
// AC: PGS-01 — After beginMinePlacement() succeeds, m_curShip and m_curWeapon must
//     not be null when the first board click arrives.  This test reproduces the bug
//     where onSetSpeed() called setShip(NULL) unconditionally, simulates that buggy
//     path (setShip(NULL) after beginMinePlacement) to confirm placeMineAtHex fails,
//     then confirms the fixed path (no nulling) records the mine successfully.
	using namespace Frontier;

	// Build a Minelayer (M + SM weapons) fleet for the defending side (owner 0).
	FVehicle * minelayer = createShip("Minelayer");
	CPPUNIT_ASSERT_MESSAGE("Minelayer ship must be creatable", minelayer != NULL);
	int mineLauncherIndex = -1;
	for (unsigned int i = 0; i < minelayer->getWeaponCount(); ++i) {
		FWeapon * w = minelayer->getWeapon(i);
		if (w != NULL && w->getType() == FWeapon::M && mineLauncherIndex < 0) {
			mineLauncherIndex = static_cast<int>(i);
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Minelayer must have an M launcher", mineLauncherIndex >= 0);

	FFleet * defendFleet = new FFleet();
	minelayer->setOwner(0);
	defendFleet->addShip(minelayer);
	FleetList defendFleets;
	defendFleets.push_back(defendFleet);

	FFleet * attackFleet = new FFleet();
	FVehicle * attacker = createShip("AssaultScout");
	CPPUNIT_ASSERT_MESSAGE("AttackScout must be creatable", attacker != NULL);
	attacker->setOwner(1);
	attackFleet->addShip(attacker);
	FleetList attackFleets;
	attackFleets.push_back(attackFleet);

	FTacticalGame game;
	game.setupFleets(&attackFleets, &defendFleets, false, NULL);

	// Place ships so they have turn-data and occupancy registered.
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

	// Reset active player to defender (owner 0) to simulate the pre-game mine
	// placement entry point.  Do NOT call setState(BS_SetupDefendFleet) here —
	// with only 1 defender ship, that setState call auto-advances to
	// BS_SetupAttackFleet and toggles the active player, breaking the test.
	game.setActivePlayer(false);  // defender = owner 0

	// Part 1: Simulate the buggy path — beginMinePlacement() succeeds but
	// setShip(NULL) is then called before the first click.
	const bool entered1 = game.beginMinePlacement();
	CPPUNIT_ASSERT_MESSAGE("beginMinePlacement() must succeed with a Minelayer in fleet (Part 1)", entered1);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Model must be in BS_PlaceMines after beginMinePlacement()",
		static_cast<int>(BS_PlaceMines), game.getState());
	// Simulate the old bug: setShip(NULL) nulls m_curShip before the first board click.
	game.setShip(NULL);
	CPPUNIT_ASSERT_MESSAGE("Simulated bug: getShip() must be NULL after setShip(NULL)", game.getShip() == NULL);

	// placeMineAtHex must fail when m_curShip is NULL (confirming the bug would have occurred).
	const FPoint bugHex(5, 5);
	const bool bugResult = game.placeMineAtHex(bugHex);
	CPPUNIT_ASSERT_MESSAGE("placeMineAtHex must return false when m_curShip is NULL (simulating pre-fix bug)", !bugResult);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("getMinedHexes must be empty after failed buggy placement",
		static_cast<unsigned int>(0), static_cast<unsigned int>(game.getMinedHexes().size()));

	// Part 2: Fixed path — beginMinePlacement() succeeds and setShip(NULL) is NOT called.
	// Reset to defender-active state again (same comment: no setState to avoid auto-advance).
	game.setActivePlayer(false);

	const bool entered2 = game.beginMinePlacement();
	CPPUNIT_ASSERT_MESSAGE("beginMinePlacement() must succeed in fixed path (Part 2)", entered2);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Model must be in BS_PlaceMines in fixed path",
		static_cast<int>(BS_PlaceMines), game.getState());

	// In the fixed path, m_curShip and m_curWeapon must be non-null.
	CPPUNIT_ASSERT_MESSAGE("Fixed path: getShip() must be non-NULL after beginMinePlacement()", game.getShip() != NULL);
	CPPUNIT_ASSERT_MESSAGE("Fixed path: getWeapon() must be non-NULL after beginMinePlacement()", game.getWeapon() != NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Fixed path: selected weapon type must be M",
		FWeapon::M, game.getWeapon()->getType());

	FWeapon * mineLauncher = minelayer->getWeapon(static_cast<unsigned int>(mineLauncherIndex));
	const int initialAmmo = mineLauncher->getAmmo();
	CPPUNIT_ASSERT_MESSAGE("Mine launcher must have ammo > 0", initialAmmo > 0);

	// AC: clicking a valid hex must succeed and record the mine.
	const FPoint mineHex(5, 5);
	const bool placed = game.placeMineAtHex(mineHex);
	CPPUNIT_ASSERT_MESSAGE("Fixed path: placeMineAtHex must succeed when m_curShip is non-NULL", placed);

	// AC: ammo decremented by 1.
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Mine launcher ammo must decrement by 1 after placement",
		initialAmmo - 1, mineLauncher->getAmmo());

	// AC: hex recorded in getMinedHexes().
	const PointSet & minedHexes = game.getMinedHexes();
	CPPUNIT_ASSERT_MESSAGE("Placed mine hex must appear in getMinedHexes()",
		minedHexes.find(mineHex) != minedHexes.end());

	// AC: placed-ordnance record appended.
	CPPUNIT_ASSERT_EQUAL_MESSAGE("getPlacedOrdnance() must contain one record after successful placement",
		static_cast<unsigned int>(1), static_cast<unsigned int>(game.getPlacedOrdnance().size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Placed ordnance record must have weapon type M",
		FWeapon::M, game.getPlacedOrdnance()[0].weaponType);

	delete defendFleet;
	delete attackFleet;
}

void FTacticalGameMechanicsTest::testPreGameSeekerPlacementIsAdditive() {
// AC: PGS-03 — During BS_PlaceSeekers, clicking the same hex multiple times must
//     accumulate one inactive seeker per click without toggling or removing any
//     previously placed seeker.  Ammo must decrement by one per click and
//     getPlacedOrdnance() must grow by one SM record per click.
//     This behavioral test MUST fail against the pre-fix toggle path (second click
//     on the same hex would remove the first seeker) and pass after the fix.
	using namespace Frontier;

	// Build a Minelayer fleet for the defending side.  The Minelayer carries
	// SM(x4) and M(x20) launchers so both placement phases can be exercised.
	FVehicle * minelayer = createShip("Minelayer");
	CPPUNIT_ASSERT_MESSAGE("Minelayer ship must be creatable", minelayer != NULL);

	// Locate the mine launcher (M) and seeker launcher (SM) weapon slots.
	int mineLauncherIndex = -1;
	int seekerLauncherIndex = -1;
	for (unsigned int i = 0; i < minelayer->getWeaponCount(); ++i) {
		FWeapon * w = minelayer->getWeapon(i);
		if (w != NULL && w->getType() == FWeapon::M && mineLauncherIndex < 0) {
			mineLauncherIndex = static_cast<int>(i);
		}
		if (w != NULL && w->getType() == FWeapon::SM && seekerLauncherIndex < 0) {
			seekerLauncherIndex = static_cast<int>(i);
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Minelayer must have an M launcher", mineLauncherIndex >= 0);
	CPPUNIT_ASSERT_MESSAGE("Minelayer must have an SM launcher", seekerLauncherIndex >= 0);

	FWeapon * seekerLauncher = minelayer->getWeapon(static_cast<unsigned int>(seekerLauncherIndex));
	const int initialSeekerAmmo = seekerLauncher->getAmmo();
	CPPUNIT_ASSERT_MESSAGE("Seeker launcher must start with ammo >= 3", initialSeekerAmmo >= 3);

	// Build fleets and initialize the tactical game.
	FFleet * defendFleet = new FFleet();
	minelayer->setOwner(0);
	defendFleet->addShip(minelayer);
	FleetList defendFleets;
	defendFleets.push_back(defendFleet);

	FFleet * attackFleet = new FFleet();
	FVehicle * attacker = createShip("AssaultScout");
	CPPUNIT_ASSERT_MESSAGE("AssaultScout must be creatable", attacker != NULL);
	attacker->setOwner(1);
	attackFleet->addShip(attacker);
	FleetList attackFleets;
	attackFleets.push_back(attackFleet);

	FTacticalGame game;
	game.setupFleets(&attackFleets, &defendFleets, false, NULL);

	// Place ships so they have turn-data and occupancy registered.
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

	// Enter mine placement phase first (required to advance to BS_PlaceSeekers).
	game.setActivePlayer(false);  // defender = owner 0
	const bool enteredMines = game.beginOrdnancePlacement();
	CPPUNIT_ASSERT_MESSAGE("beginOrdnancePlacement() must succeed with a Minelayer in fleet", enteredMines);

	// Place one mine to satisfy the mine phase, then advance to seeker placement.
	const FPoint mineHex(5, 5);
	const bool mineClicked = game.handleHexClick(mineHex);
	CPPUNIT_ASSERT_MESSAGE("First mine placement must succeed", mineClicked);

	game.completeMinePlacement();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("completeMinePlacement() must advance to BS_PlaceSeekers",
		static_cast<int>(BS_PlaceSeekers), game.getState());
	CPPUNIT_ASSERT_MESSAGE("Seeker launcher must be selected after entering BS_PlaceSeekers",
		game.getWeapon() != NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Selected weapon must be SM type in BS_PlaceSeekers",
		FWeapon::SM, game.getWeapon()->getType());

	// The placed-ordnance list already has 1 mine record.  Verify the baseline.
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Baseline: exactly 1 placed-ordnance record (the mine) before seeker placement",
		static_cast<unsigned int>(1), static_cast<unsigned int>(game.getPlacedOrdnance().size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Baseline: no seekers in getSeekerMissiles() before seeker placement",
		static_cast<unsigned int>(0), static_cast<unsigned int>(game.getSeekerMissiles().size()));

	// AC: Click the same hex 3 times; each must add one inactive seeker (additive, not toggle).
	const FPoint seekerHex(7, 7);

	// Click 1
	const bool click1 = game.handleHexClick(seekerHex);
	CPPUNIT_ASSERT_MESSAGE("Click 1 on seekerHex must return true", click1);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 1: getSeekerMissiles() must have 1 seeker",
		static_cast<unsigned int>(1), static_cast<unsigned int>(game.getSeekerMissiles().size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 1: seeker ammo must be decremented by 1",
		initialSeekerAmmo - 1, seekerLauncher->getAmmo());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 1: getPlacedOrdnance() must have 2 records (mine + seeker)",
		static_cast<unsigned int>(2), static_cast<unsigned int>(game.getPlacedOrdnance().size()));

	// Click 2 on the same hex — pre-fix toggle behavior would REMOVE the first seeker here.
	const bool click2 = game.handleHexClick(seekerHex);
	CPPUNIT_ASSERT_MESSAGE("Click 2 on same seekerHex must return true (additive, not toggle)", click2);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 2: getSeekerMissiles() must have 2 seekers (not 0 from toggle)",
		static_cast<unsigned int>(2), static_cast<unsigned int>(game.getSeekerMissiles().size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 2: seeker ammo must be decremented by 2 total",
		initialSeekerAmmo - 2, seekerLauncher->getAmmo());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 2: getPlacedOrdnance() must have 3 records (mine + 2 seekers)",
		static_cast<unsigned int>(3), static_cast<unsigned int>(game.getPlacedOrdnance().size()));

	// Click 3 on the same hex.
	const bool click3 = game.handleHexClick(seekerHex);
	CPPUNIT_ASSERT_MESSAGE("Click 3 on same seekerHex must return true (additive)", click3);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 3: getSeekerMissiles() must have 3 seekers",
		static_cast<unsigned int>(3), static_cast<unsigned int>(game.getSeekerMissiles().size()));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 3: seeker ammo must be decremented by 3 total",
		initialSeekerAmmo - 3, seekerLauncher->getAmmo());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("After click 3: getPlacedOrdnance() must have 4 records (mine + 3 seekers)",
		static_cast<unsigned int>(4), static_cast<unsigned int>(game.getPlacedOrdnance().size()));

	// Verify all seekers are inactive and placed at the same hex.
	const std::vector<FTacticalSeekerMissileState> & seekers = game.getSeekerMissiles();
	for (unsigned int i = 0; i < seekers.size(); ++i) {
		CPPUNIT_ASSERT_MESSAGE("Every placed seeker must be inactive", !seekers[i].active);
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Every placed seeker must be at seekerHex X",
			seekerHex.getX(), seekers[i].hex.getX());
		CPPUNIT_ASSERT_EQUAL_MESSAGE("Every placed seeker must be at seekerHex Y",
			seekerHex.getY(), seekers[i].hex.getY());
	}

	// Verify all SM placed-ordnance records reference the seeker hex.
	const std::vector<FTacticalPlacedOrdnance> & ordnance = game.getPlacedOrdnance();
	unsigned int smRecordCount = 0;
	for (unsigned int i = 0; i < ordnance.size(); ++i) {
		if (ordnance[i].weaponType == FWeapon::SM) {
			++smRecordCount;
			CPPUNIT_ASSERT_EQUAL_MESSAGE("SM placed-ordnance record must reference seekerHex X",
				seekerHex.getX(), ordnance[i].hex.getX());
			CPPUNIT_ASSERT_EQUAL_MESSAGE("SM placed-ordnance record must reference seekerHex Y",
				seekerHex.getY(), ordnance[i].hex.getY());
		}
	}
	CPPUNIT_ASSERT_EQUAL_MESSAGE("getPlacedOrdnance() must contain exactly 3 SM records",
		static_cast<unsigned int>(3), smRecordCount);

	// AC: Ammo exhaustion — clicking again when ammo reaches zero must not add another seeker.
	// Drain remaining ammo by placing additional seekers (at different hexes to avoid any
	// per-hex limit), then verify that one more click on the same hex is rejected.
	const int remainingAmmo = seekerLauncher->getAmmo();
	for (int k = 0; k < remainingAmmo; ++k) {
		// Use distinct hexes to drain ammo cleanly.
		const FPoint drainHex(8 + k, 8);
		game.handleHexClick(drainHex);
	}
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker ammo must be 0 after draining",
		0, seekerLauncher->getAmmo());

	const unsigned int seekerCountBeforeExhaustedClick = static_cast<unsigned int>(game.getSeekerMissiles().size());
	const bool exhaustedClick = game.handleHexClick(seekerHex);
	CPPUNIT_ASSERT_MESSAGE("Click when ammo == 0 must return false", !exhaustedClick);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Seeker count must not increase when ammo is exhausted",
		seekerCountBeforeExhaustedClick, static_cast<unsigned int>(game.getSeekerMissiles().size()));

	// Clean up heap-allocated objects.
	delete defendFleet;
	delete attackFleet;
}

void FTacticalGameMechanicsTest::testDefenderMovePhaseGatingRejectsCompletionBeforeMinimumMove() {
// AC: TMF-04 — At the start of the defender's move phase, before any hex-click,
// a Minelayer with speed=10 / ADF=1 must NOT be allowed to complete its move phase
// (isMoveComplete() must return false) because it still needs to move 9 hexes.
//
// The pre-fix bug: checkMoveStatus() used the stale m_shipPos (the attacker's last
// placement position) instead of the defender ship's actual board position.
// When m_shipPos=(54,10) and the Minelayer's heading is 3, findNextHex((54,10),3)
// returns (55,10), which fails the in-bounds check (x<55 is false), leaving
// finished=true and spuriously setting isMoveComplete()=true.
//
// Post-fix behavior: checkMoveStatus() calls findShipHex() which returns the
// Minelayer's actual position (10,10); findNextHex((10,10),3)=(11,10) is in bounds;
// finished=false; isMoveComplete()=false — correct.
//
// This test MUST fail against the pre-fix code and pass after the fix.
	using namespace Frontier;

	// Build a Minelayer (defender) with speed=10, ADF=1 (minMove = 9 hexes required).
	FVehicle * minelayer = createShip("Minelayer");
	CPPUNIT_ASSERT_MESSAGE("Minelayer ship must be creatable", minelayer != NULL);
	minelayer->setSpeed(10);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Minelayer ADF must be 1", 1, minelayer->getADF());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Minelayer speed must be 10", 10, minelayer->getSpeed());

	FFleet * defendFleet = new FFleet();
	minelayer->setOwner(0);
	defendFleet->addShip(minelayer);
	FleetList defendFleets;
	defendFleets.push_back(defendFleet);

	// Build an AssaultScout (attacker) — default speed=0, so its move phase is
	// trivially complete (minMove = 0 - ADF < 0, no ship blocks completion).
	FVehicle * attacker = createShip("AssaultScout");
	CPPUNIT_ASSERT_MESSAGE("AssaultScout ship must be creatable", attacker != NULL);
	FFleet * attackFleet = new FFleet();
	attacker->setOwner(1);
	attackFleet->addShip(attacker);
	FleetList attackFleets;
	attackFleets.push_back(attackFleet);

	FTacticalGame game;
	game.setupFleets(&attackFleets, &defendFleets, false, NULL);

	// Place the Minelayer at (10,10) with heading 3 (pointing right, toward
	// increasing x).  In the hex grid, heading 3 from (10,10) points to (11,10).
	// From (54,10) heading 3 points to (55,10) which is OUT OF BOUNDS (x >= 55).
	// This asymmetry is the crux of the pre-fix / post-fix difference.
	game.setState(BS_SetupDefendFleet);
	game.setControlState(true);
	game.setShip(minelayer);
	game.placeShip(FPoint(10, 10));
	game.setShipPlacementHeading(3);   // heading 3 = "right" in hex grid

	// Place the AssaultScout LAST at (54,10) heading 3.
	// After this call, m_shipPos = (54,10) — the stale value that the pre-fix bug
	// inherits when checkMoveStatus() runs for the defender's move phase.
	game.setState(BS_SetupAttackFleet);
	game.setControlState(true);
	game.setShip(attacker);
	game.placeShip(FPoint(54, 10));
	game.setShipPlacementHeading(3);

	// Enter BS_Battle with the defender as the moving player, then trigger the
	// defender's move phase via the normal setPhase(PH_MOVE) entry point.
	// setPhase(PH_MOVE) → beginSeekerActivationPhase() → (no seekers) →
	// beginMovePhase() → toggleActivePlayer() + resetMovementState() →
	// resetTurnInfoForCurrentMover() + checkMoveStatus().
	//
	// At this point m_shipPos = (54,10) (stale from the attacker's placement).
	//
	// Pre-fix checkMoveStatus() for the Minelayer (heading=3, minMove=9):
	//   path.getPathLength()==0 → pos = m_shipPos = (54,10)
	//   findNextHex((54,10), 3) = (55,10)  →  55 >= 55  → out of bounds
	//   finished stays true → setMoveComplete(true)  [WRONG]
	//
	// Post-fix checkMoveStatus():
	//   path.getPathLength()==0 → findShipHex(minelayer_ID, pos) = (10,10)
	//   findNextHex((10,10), 3) = (11,10)  →  11 < 55  → valid
	//   finished = false → setMoveComplete(false)  [CORRECT]
	game.setState(BS_Battle);
	game.setMovingPlayer(false);   // false → defender is the moving player
	game.setPhase(PH_MOVE);

	// BEHAVIORAL ASSERTION 1 (AC #1, #2): isMoveComplete() must be false at
	// defender move-phase entry — before any hex-click — because the Minelayer
	// has speed=10 / ADF=1 and has not yet moved 9 hexes.
	CPPUNIT_ASSERT_MESSAGE(
		"TMF-04: isMoveComplete() must be false at defender move-phase entry "
		"when Minelayer has speed=10/ADF=1 and zero hexes moved (pre-fix bug "
		"would incorrectly return true here)",
		!game.isMoveComplete());

	// BEHAVIORAL ASSERTION 2 (AC #2, #3): select the Minelayer and move it the
	// minimum 9 hexes; isMoveComplete() must become true once the minimum is met.
	// Click on the Minelayer's hex to select it (sets m_drawRoute=true).
	const bool selected = game.handleHexClick(FPoint(10, 10));
	CPPUNIT_ASSERT_MESSAGE(
		"handleHexClick on Minelayer hex must select the ship in PH_MOVE",
		selected);

	// The initial movement hex list should have at least 9 entries
	// (Minelayer speed=10, ADF=1 → speed+ADF = 11 entries generated).
	const std::vector<FPoint> & movHexes = game.getMovementHexes();
	CPPUNIT_ASSERT_MESSAGE(
		"Movement hex list must have at least 9 entries after selecting Minelayer "
		"(speed=10, ADF=1 generates up to 11 forward hexes)",
		movHexes.size() >= 9);

	// Click the 9th hex (index 8) to move exactly 9 hexes in one click.
	// handleHexClick → handleMoveHexSelection finds it at list position 9 →
	// sets nMoved=9 → checkMoveStatus: 9 < 9 is false → finished=true →
	// setMoveComplete(true).
	const FPoint target9hex = movHexes[8];
	const bool moved = game.handleHexClick(target9hex);
	CPPUNIT_ASSERT_MESSAGE(
		"handleHexClick on the 9th movement hex must succeed (move 9 hexes)",
		moved);

	CPPUNIT_ASSERT_MESSAGE(
		"TMF-04: isMoveComplete() must be true after Minelayer moves the minimum 9 hexes",
		game.isMoveComplete());

	delete defendFleet;
	delete attackFleet;
}

}
