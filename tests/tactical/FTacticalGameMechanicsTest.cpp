/**
 * @file FTacticalGameMechanicsTest.cpp
 * @brief Implementation tests for FTacticalGame additive mechanics container
 */

#include "FTacticalGameMechanicsTest.h"

#include <cstdlib>
#include <fstream>
#include <iterator>

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

assertContains(header, "int setupFleets(FleetList * aList, FleetList * dList, bool planet = false, FVehicle * station = NULL);");
assertContains(header, "void setState(int s);");
assertContains(header, "void setPhase(int p);");
assertContains(header, "void resetMovementState();");
assertContains(header, "void finalizeMovementState();");
assertContains(header, "void clearMovementHighlights();");
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
assertContains(body, "m_closeInProgress = false;");
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
	assertContains(header, "void completeMinePlacement();");
	assertContains(header, "void completeMovePhase();");
	assertContains(header, "FTacticalCombatReportSummary resolveCurrentFirePhase();");
	assertContains(header, "void completeDefensiveFirePhase();");
	assertContains(header, "void completeOffensiveFirePhase();");
	assertContains(header, "void computeWeaponRange();");
	assertContains(header, "bool assignTargetFromHex(const FPoint & hex);");
	assertContains(header, "bool placeMineAtHex(const FPoint & hex);");
	assertContains(header, "bool isHexMinable(const FPoint & hex);");

	assertContains(header, "const VehicleList & getHexOccupants(const FPoint & hex) const;");
	assertContains(header, "const std::vector<FPoint> & getMovementHexes() const");
	assertContains(header, "const std::vector<FPoint> & getLeftTurnHexes() const");
	assertContains(header, "const std::vector<FPoint> & getRightTurnHexes() const");
	assertContains(header, "const PointSet & getTargetHexes() const");
	assertContains(header, "const PointSet & getHeadOnHexes() const");
	assertContains(header, "const PointSet & getMinedHexes() const");
	assertContains(header, "const FHexMap & getMineTargets() const");
	assertContains(header, "unsigned int getMineOwner() const");
	assertContains(header, "const std::map<unsigned int, FTacticalTurnData> & getTurnInfo() const");
	assertContains(header, "bool hasShipPlacementPendingRotation() const");
	assertContains(header, "const FPoint & getSelectedShipHex() const");
	assertContains(header, "const VehicleList & getShipsWithMines() const");
	assertContains(header, "bool isHexInBounds(const FPoint & hex) const;");
	assertContains(header, "bool isHexOccupied(const FPoint & hex) const;");

	assertContains(source, "bool FTacticalGame::selectWeapon(unsigned int weaponIndex)");
	assertContains(source, "bool FTacticalGame::selectDefense(unsigned int defenseIndex)");
	assertContains(source, "bool FTacticalGame::handleHexClick(const FPoint & hex)");
	assertContains(source, "const VehicleList & FTacticalGame::getHexOccupants(const FPoint & hex) const");
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
}

}
