/**
 * @file FTacticalBattleBoardRendererDelegationTest.cpp
 * @brief Implementation file for FBattleBoard renderer delegation tests
 */

#include "FTacticalBattleBoardRendererDelegationTest.h"

#include <fstream>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleBoardRendererDelegationTest );

void FTacticalBattleBoardRendererDelegationTest::setUp() {
}

void FTacticalBattleBoardRendererDelegationTest::tearDown() {
}

std::string FTacticalBattleBoardRendererDelegationTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalBattleBoardRendererDelegationTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT_MESSAGE(signature, signaturePos != std::string::npos);

std::string::size_type bodyStart = source.find('{', signaturePos);
CPPUNIT_ASSERT(bodyStart != std::string::npos);

int depth = 1;
for (std::string::size_type i = bodyStart + 1; i < source.size(); i++) {
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

void FTacticalBattleBoardRendererDelegationTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testDrawUsesBattleScreenModelAccessors() {
// AC: draw() renders from FTacticalGame-backed state exposed through FBattleScreen accessors.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::draw(wxDC &dc)");

assertContains(body, "drawGrid(dc);");
assertContains(body, "const FPoint & planetPos = m_parent->getPlanetPos();");
assertContains(body, "const int planetChoice = m_parent->getPlanetChoice();");
assertContains(body, "drawShips();");
assertContains(body, "if (m_parent->getState() == BS_Battle) {");
assertContains(body, "drawRoute(dc);");
assertContains(body, "if (m_parent->getState() == BS_PlaceMines || m_parent->getState() == BS_PlaceSeekers) {");
assertContains(body, "drawMinedHexes(dc);");
assertContains(body, "if (m_parent->getWeapon() != NULL) {");
assertContains(body, "drawWeaponRange(dc);");
assertContains(body, "drawTarget(dc);");
}

void FTacticalBattleBoardRendererDelegationTest::testOverlayDrawingUsesModelStateAccessors() {
// AC: route/range/mine overlays are sourced from model state accessors, not board-owned mechanics data.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));

const std::string shipsBody = extractFunctionBody(source, "void FBattleBoard::drawShips()");
assertContains(shipsBody, "const VehicleList & ships = m_parent->getHexOccupants(hex);");

const std::string routeBody = extractFunctionBody(source, "void FBattleBoard::drawRoute(wxDC &dc)");
assertContains(routeBody, "const std::map<unsigned int, FTacticalTurnData> & turnInfo = m_parent->getTurnInfo();");
assertContains(routeBody, "const bool isStoppedShipPreviewSelection = selectedTurnItr != turnInfo.end()");
	assertContains(routeBody, "&& selectedTurnItr->second.speed == 0");
	assertContains(routeBody, "&& selectedTurnItr->second.nMoved == 0");
	assertContains(routeBody, "&& m_parent->getShip()->getMR() > 0;");
	assertContains(routeBody, "if (isStoppedShipPreviewSelection) {");
	assertContains(routeBody, "const std::vector<FTacticalMovePreviewRoute> & previewRoutes = m_parent->getStoppedShipPreviewRoutes();");
	assertContains(routeBody, "for (std::vector<FTacticalMovePreviewRoute>::const_iterator previewItr = previewRoutes.begin();");
	assertContains(routeBody, "drawRouteHexes(dc, previewItr->routeHexes, 1);");
	assertContains(routeBody, "} else {");
	assertContains(routeBody, "drawRouteHexes(dc, m_parent->getMovementHexes(), movedCount+1);");
	assertContains(routeBody, "drawRouteHexes(dc, m_parent->getLeftTurnHexes(), movedCount+1);");
	assertContains(routeBody, "drawRouteHexes(dc, m_parent->getRightTurnHexes(), movedCount+1);");

const std::string rangeBody = extractFunctionBody(source, "void FBattleBoard::drawWeaponRange(wxDC &dc)");
assertContains(rangeBody, "for (PointSet::const_iterator itr = m_parent->getTargetHexes().begin();");
assertContains(rangeBody, "for (PointSet::const_iterator itr = m_parent->getHeadOnHexes().begin();");

const std::string mineBody = extractFunctionBody(source, "void FBattleBoard::drawMinedHexes(wxDC &dc)");
assertContains(mineBody, "for (PointSet::const_iterator itr = m_parent->getMinedHexes().begin();");
}

void FTacticalBattleBoardRendererDelegationTest::testDrawShipsUsesTemporaryFacingForStoppedSelectedMover() {
// AC: selected stopped mover in PH_MOVE draws with temporary facing while retaining normal heading fallback.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string helperBody = extractFunctionBody(source, "int getRenderedHeadingForShip(FBattleScreen * parent, FVehicle * ship)");
const std::string drawShipsBody = extractFunctionBody(source, "void FBattleBoard::drawShips()");

assertContains(helperBody, "if (parent->getPhase() != PH_MOVE || parent->getShip() == NULL");
assertContains(helperBody, "|| ship->getOwner() != parent->getMovingPlayerID()) {");
assertContains(helperBody, "if (turnData.speed == 0");
assertContains(helperBody, "&& turnData.nMoved == 0");
assertContains(helperBody, "&& ship->getMR() > 0");
assertContains(helperBody, "&& turnData.curHeading != ship->getHeading()) {");
assertContains(helperBody, "return turnData.curHeading;");
assertContains(helperBody, "return ship->getHeading();");

assertContains(drawShipsBody, "drawCenteredOnHex(*icon,hex,getRenderedHeadingForShip(m_parent, *itr));");
}

void FTacticalBattleBoardRendererDelegationTest::testDrawSeekerMissilesUsesActivationAndBattleVisibilityRules() {
// AC: seeker icon rendering switches from inactive activation stacks to active seeker-only battle rendering.
// SMC-02: during PH_SEEKER_ACTIVATION, BOTH inactive stacks and active seekers for the moving player are drawn.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleBoard::draw(wxDC &dc)");
const std::string seekerBody = extractFunctionBody(source, "void FBattleBoard::drawSeekerMissiles(wxDC &dc)");

assertContains(drawBody, "if (m_parent->getState() == BS_Battle) {");
assertContains(drawBody, "drawSeekerMissiles(dc);");
assertContains(seekerBody, "if (m_parent->getPhase() == PH_SEEKER_ACTIVATION) {");
// SMC-02 AC4: inactive stacks remain clickable during activation.
assertContains(seekerBody, "const std::vector<FPoint> inactiveHexes = m_parent->getInactiveSeekerActivationHexes();");
assertContains(seekerBody, "if (!m_parent->isHexInBounds(*itr)) {");
assertContains(seekerBody, "drawCenteredOnHex(*m_seekerMissileIcon, *itr);");
// SMC-02 AC4: active seekers for the moving player are also drawn during activation so activations are visible on the board.
assertContains(seekerBody, "m_parent->getActiveSeekersByMovingPlayer()");
assertContains(seekerBody, "drawCenteredOnHex(*m_seekerMissileIcon, itr->hex, itr->heading);");
assertContains(seekerBody, "return;");
// Battle-mode rendering: only active seekers, per-hex query.
assertContains(seekerBody, "const std::vector<FTacticalSeekerMissileState> activeSeekers = m_parent->getSeekerMissilesAtHex(hex, true);");
assertContains(seekerBody, "if (!activeSeekers.empty()) {");
// SMC-07: active seeker icon is rotated by its current heading (heading * pi/3 radians).
assertContains(seekerBody, "drawCenteredOnHex(*m_seekerMissileIcon, hex, activeSeekers[0].heading)");
CPPUNIT_ASSERT(seekerBody.find("getSeekerMissilesAtHex(hex, false)") == std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testDrawSeekerPathsIsCalledInMovePhaseWithCyanPen() {
// AC: drawSeekerPaths is called only in PH_MOVE and draws using a cyan pen with width 2.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleBoard::draw(wxDC &dc)");
const std::string seekerPathBody = extractFunctionBody(source, "void FBattleBoard::drawSeekerPaths(wxDC &dc)");

// SMF-06: drawSeekerPaths is called in PH_MOVE and PH_SEEKER_ACTIVATION
// so impacting seekers remain visible during ICM/damage dialogs.
assertContains(drawBody, "m_parent->getPhase() == PH_MOVE || m_parent->getPhase() == PH_SEEKER_ACTIVATION");
assertContains(drawBody, "drawSeekerPaths(dc);");

// SMC-07: path uses getSeekerMissiles() from parent (delegation through FBattleScreen)
assertContains(seekerPathBody, "m_parent->getSeekerMissiles()");

// SMC-07: cyan pen with width 2 distinct from ship paths
assertContains(seekerPathBody, "wxColour cyan(wxT(\"#00CCCC\"))");
assertContains(seekerPathBody, "dc.SetPen(wxPen(cyan, 2))");

// SMC-07: only active seekers with movementPath >= 2 points are drawn
assertContains(seekerPathBody, "if (!itr->active || itr->movementPath.size() < 2)");

// SMC-07: path draws stepped lines from hex-center to hex-center
assertContains(seekerPathBody, "dc.DrawLine(lx, ly, x, y)");
}

void FTacticalBattleBoardRendererDelegationTest::testOnLeftUpOnlyHitTestsAndForwardsHexClick() {
// AC: onLeftUp computes clicked hex and forwards to model click handler through FBattleScreen.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::onLeftUp(wxMouseEvent & event)");

assertContains(body, "event.GetPosition(&x,&y);");
assertContains(body, "if (getHex(x,y,a,b)){");
assertContains(body, "m_parent->handleHexClick(FPoint(a,b));");
assertContains(body, "event.Skip();");
CPPUNIT_ASSERT(body.find("placeShip(") == std::string::npos);
CPPUNIT_ASSERT(body.find("placePlanet(") == std::string::npos);
CPPUNIT_ASSERT(body.find("placeMineAtHex(") == std::string::npos);
CPPUNIT_ASSERT(body.find("setShipPlacementHeadingByHex(") == std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testOnLeftUpRoutesActivationPhaseThroughSeekerSelection() {
// AC: battle activation clicks route through seeker activation hex selection while preserving legacy source token.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::onLeftUp(wxMouseEvent & event)");

assertContains(body, "if (m_parent->getState() == BS_Battle && m_parent->getPhase() == PH_SEEKER_ACTIVATION) {");
assertContains(body, "m_parent->activateInactiveSeekerAtHex(hex);");
assertContains(body, "m_parent->reDraw();");
assertContains(body, "} else {");
assertContains(body, "m_parent->handleHexClick(hex);");
assertContains(body, "m_parent->handleHexClick(FPoint(a,b));");
}

void FTacticalBattleBoardRendererDelegationTest::testHeaderRemovesDuplicatedMechanicsState() {
// AC: FBattleBoard retains renderer geometry/hit-test state and no duplicate tactical mechanics containers.
const std::string header = readFile(repoFile("include/tactical/FBattleBoard.h"));

assertContains(header, "FPoint m_hexCenters[100][100];");
assertContains(header, "ImageList m_planetImages;");
assertContains(header, "wxImage * m_maskingScreenIcon;");
CPPUNIT_ASSERT(header.find("m_hexData") == std::string::npos);
CPPUNIT_ASSERT(header.find("m_turnInfo") == std::string::npos);
CPPUNIT_ASSERT(header.find("hexData") == std::string::npos);
CPPUNIT_ASSERT(header.find("turnData") == std::string::npos);
CPPUNIT_ASSERT(header.find("m_targetHexes") == std::string::npos);
CPPUNIT_ASSERT(header.find("m_minedHexList") == std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testDrawSeekerMissilesPendingOffensiveFireBranch() {
// SMC-05 AC: during PH_ATTACK_FIRE the renderer draws pending offensive-fire seeker hexes
// (no rotation), then active seekers with heading, then returns early.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string seekerBody = extractFunctionBody(source, "void FBattleBoard::drawSeekerMissiles(wxDC &dc)");

// AC1: PH_ATTACK_FIRE branch exists and calls getAllPendingOffensiveFireSeekerHexes()
assertContains(seekerBody, "if (m_parent->getPhase() == PH_ATTACK_FIRE) {");
assertContains(seekerBody, "m_parent->getAllPendingOffensiveFireSeekerHexes()");

// AC1: each in-bounds pending hex is drawn without rotation via drawCenteredOnHex(*m_seekerMissileIcon, *itr)
assertContains(seekerBody, "drawCenteredOnHex(*m_seekerMissileIcon, *itr);");

// AC2: after pending hexes, committed active seekers are also drawn with heading rotation
// so the existing active-seeker visual is preserved during PH_ATTACK_FIRE
assertContains(seekerBody, "m_parent->getSeekerMissilesAtHex(hex, true)");

// AC3: the branch ends with return so the outer battle-phase loop is skipped for PH_ATTACK_FIRE
// (confirmed by at least two 'return;' occurrences: PH_SEEKER_ACTIVATION and PH_ATTACK_FIRE)
std::string::size_type firstReturn = seekerBody.find("return;");
CPPUNIT_ASSERT_MESSAGE("Expected first return; in drawSeekerMissiles", firstReturn != std::string::npos);
std::string::size_type secondReturn = seekerBody.find("return;", firstReturn + 1);
CPPUNIT_ASSERT_MESSAGE("Expected second return; in drawSeekerMissiles for PH_ATTACK_FIRE early exit", secondReturn != std::string::npos);

// AC4: the delegation to FBattleScreen is correct — FBattleScreen::getAllPendingOffensiveFireSeekerHexes
// delegates to FTacticalGame (source-contract: thin forwarding function present)
const std::string screenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
assertContains(screenSource, "FBattleScreen::getAllPendingOffensiveFireSeekerHexes()");
assertContains(screenSource, "m_tacticalGame->getAllPendingOffensiveFireSeekerHexes()");

// AC4: the model method is read-only (const) and uses only std::vector<FPoint> — confirm in header
const std::string gameHeader = readFile(repoFile("include/tactical/FTacticalGame.h"));
assertContains(gameHeader, "std::vector<FPoint> getAllPendingOffensiveFireSeekerHexes() const;");
const std::string screenHeader = readFile(repoFile("include/tactical/FBattleScreen.h"));
assertContains(screenHeader, "std::vector<FPoint> getAllPendingOffensiveFireSeekerHexes() const;");
}

void FTacticalBattleBoardRendererDelegationTest::testDrawSeekerMoveCountOverlayCalledUnconditionallyInBSBattle() {
// SMFR-04 source-contract: drawSeekerMoveCountOverlay(dc) must be called inside the BS_Battle
// block in FBattleBoard::draw() without any intervening phase guard.  This locks the fix that
// makes the speed label visible in all battle phases (not only PH_MOVE).
// Also verifies that the overlay function body uses movementAllowance as the fallback when
// no movement path is recorded, and does NOT skip seekers whose movementPath.size() < 2.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleBoard::draw(wxDC &dc)");

// AC1: the call appears inside the BS_Battle block.
const std::string::size_type battleGuardPos = drawBody.find("if (m_parent->getState() == BS_Battle) {");
CPPUNIT_ASSERT_MESSAGE("BS_Battle guard must exist in draw()", battleGuardPos != std::string::npos);
const std::string::size_type callPos = drawBody.find("drawSeekerMoveCountOverlay(dc);");
CPPUNIT_ASSERT_MESSAGE("drawSeekerMoveCountOverlay(dc) must exist in draw()", callPos != std::string::npos);
CPPUNIT_ASSERT_MESSAGE(
	"drawSeekerMoveCountOverlay(dc) must appear after the BS_Battle guard in draw()",
	callPos > battleGuardPos);

// AC1: the call must NOT be inside the phase-gated drawSeekerPaths block.
// The phase guard "if (m_parent->getPhase() == PH_MOVE || m_parent->getPhase() == PH_SEEKER_ACTIVATION)"
// must NOT appear between the battle-guard start and the drawSeekerMoveCountOverlay call.
// Strategy: extract the fragment between the BS_Battle guard and the overlay call; it must not
// contain a phase guard that would prevent the overlay from executing in PH_ATTACK_FIRE, etc.
const std::string fragmentBetween = drawBody.substr(battleGuardPos, callPos - battleGuardPos);
// The fragment between BS_Battle guard open and the overlay call may contain the seeker-path
// phase guard, but the overlay call itself must NOT be nested inside it.  Verify this by
// checking that drawSeekerMoveCountOverlay(dc) is NOT inside the drawSeekerPaths branch:
// The simplest structural check: the overlay call must not be preceded by a phase guard
// whose closing brace is not yet encountered before the call.
// We verify this negatively: if the overlay call appears after the drawSeekerPaths closing
// brace then it is outside the phase-gated block.
// Locate the seeker-paths phase block end.
std::string::size_type seekerPathBlock = fragmentBetween.find("drawSeekerPaths(dc)");
if (seekerPathBlock != std::string::npos) {
	// Confirm the closing brace of the seeker-paths phase guard precedes the overlay call
	// by checking that a '}' appears after drawSeekerPaths(dc) in the fragment.
	std::string::size_type closingBraceAfterPaths = fragmentBetween.find('}', seekerPathBlock);
	CPPUNIT_ASSERT_MESSAGE(
		"The seeker-paths phase-gated block must close before drawSeekerMoveCountOverlay(dc).",
		closingBraceAfterPaths != std::string::npos);
}

// AC2: the overlay function body uses movementAllowance as the fallback count.
const std::string overlayBody = extractFunctionBody(source, "void FBattleBoard::drawSeekerMoveCountOverlay(wxDC &dc)");
assertContains(overlayBody, "movementAllowance");
// AC2: it uses movementPath.size()-1 when a path exists.
assertContains(overlayBody, "movementPath.size()");
// AC2: the fallback branch assigns movementAllowance (not just checks it).
assertContains(overlayBody, "count = itr->movementAllowance");

// AC3: the function does NOT skip seekers with movementPath.size() < 2 — it iterates
// every active seeker.  The old guard "if (!itr->active || itr->movementPath.size() < 2)"
// that was in drawSeekerPaths must NOT appear in the overlay function body.
CPPUNIT_ASSERT_MESSAGE(
	"drawSeekerMoveCountOverlay must not skip seekers based on movementPath.size() < 2 alone; "
	"it must include all active seekers regardless of whether they moved this phase.",
	overlayBody.find("movementPath.size() < 2") == std::string::npos);
// The only active-seeker filter should be the '!itr->active' skip.
assertContains(overlayBody, "if (!itr->active)");
}

void FTacticalBattleBoardRendererDelegationTest::testDrawSeekerMoveCountOverlayOwnershipGuardForActivationPhase() {
// SMRV-03 source-contract supplement: the overlay body must contain the
// activation-phase ownership guard that matches the sprite suppression in
// drawSeekerMissiles().
//
// Verifies:
// - The overlay detects the activation phase: 'getPhase() == PH_SEEKER_ACTIVATION'
//   or a local bool 'activationPhase'.
// - The overlay contains the per-seeker guard that skips opponent seekers:
//   'itr->ownerID != m_parent->getMovingPlayerID()'.
// - The guard is conditioned on the activation phase (ensuring other phases are unaffected).
//
// This source-contract test supplements the behavioral pixel-level test
// testSeekerMoveCountOverlaySupressesOpponentLabelsDuringActivation in TacticalGuiLiveTest.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string overlayBody = extractFunctionBody(source, "void FBattleBoard::drawSeekerMoveCountOverlay(wxDC &dc)");

// The activation-phase flag must be set from getPhase().
assertContains(overlayBody, "PH_SEEKER_ACTIVATION");

// The per-seeker ownership guard must reference getMovingPlayerID() and ownerID.
assertContains(overlayBody, "getMovingPlayerID()");
assertContains(overlayBody, "itr->ownerID");

// The ownership skip must use 'continue' so only the opponent's seeker is skipped.
assertContains(overlayBody, "continue");
}

void FTacticalBattleBoardRendererDelegationTest::testDrawTriggeredMineHexesCalledInsideBSBattleGuard() {
// SMFR-03: drawTriggeredMineHexes(dc) must be called from FBattleBoard::draw()
// inside the BS_Battle state guard so triggered-hex highlights are visible during
// battle-phase redraws while the mine damage summary dialog is shown.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string drawBody = extractFunctionBody(source, "void FBattleBoard::draw(wxDC &dc)");

// The call must appear somewhere in the draw body.
assertContains(drawBody, "drawTriggeredMineHexes(dc);");

// The call must appear inside the BS_Battle guard.
const std::string::size_type battleGuardPos = drawBody.find("if (m_parent->getState() == BS_Battle) {");
const std::string::size_type callPos = drawBody.find("drawTriggeredMineHexes(dc);");
CPPUNIT_ASSERT_MESSAGE("BS_Battle guard must precede drawTriggeredMineHexes call in draw()",
battleGuardPos != std::string::npos && callPos != std::string::npos && callPos > battleGuardPos);

// drawTriggeredMineHexes reads getLastTriggeredMineHexes from the parent and applies green shading.
const std::string triggeredBody = extractFunctionBody(source, "void FBattleBoard::drawTriggeredMineHexes(wxDC &dc)");
assertContains(triggeredBody, "m_parent->getLastTriggeredMineHexes()");
assertContains(triggeredBody, "m_parent->isHexInBounds(*itr)");
assertContains(triggeredBody, "drawShadedHex(");
assertContains(triggeredBody, "#00FF00");

// Delegation: FBattleScreen exposes getLastTriggeredMineHexes() for board use.
const std::string screenHeader = readFile(repoFile("include/tactical/FBattleScreen.h"));
assertContains(screenHeader, "getLastTriggeredMineHexes()");
}

void FTacticalBattleBoardRendererDelegationTest::testOnMotionUsesPlacementForwardersOnly() {
// AC: setup movement hover handling stays as UI hit-test/rotation forwarding.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleBoard::onMotion(wxMouseEvent & event)");

assertContains(body, "m_parent->hasShipPlacementPendingRotation()");
assertContains(body, "m_parent->setShipPlacementHeadingByHex(FPoint(a,b))");
assertContains(body, "Refresh();");
CPPUNIT_ASSERT(body.find("computeWeaponRange(") == std::string::npos);
CPPUNIT_ASSERT(body.find("resetMovementState(") == std::string::npos);
CPPUNIT_ASSERT(body.find("setShipPlacementHeading(") == std::string::npos);
CPPUNIT_ASSERT(body.find("handleHexClick(") == std::string::npos);
}

void FTacticalBattleBoardRendererDelegationTest::testBoardConstructorLoadsSeekerMissileIconThroughResolveAssetPath() {
// AC: seeker icon uses the shared asset path resolver in FBattleBoard constructor.
const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
const std::string ctorBody = extractFunctionBody(
	source,
	"FBattleBoard::FBattleBoard(wxWindow * parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString &name)");

assertContains(ctorBody, "FGameConfig &gc = FGameConfig::create();");
assertContains(ctorBody, "m_seekerMissileIcon = new wxImage(gc.resolveAssetPath(\"icons/SeekerMissile.png\"));");
}

}
