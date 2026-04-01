/**
 * @file FTacticalBattleScreenDelegationTest.cpp
 * @brief Implementation file for the FTacticalBattleScreenDelegationTest class
 */

#include "FTacticalBattleScreenDelegationTest.h"

#include <fstream>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleScreenDelegationTest );

void FTacticalBattleScreenDelegationTest::setUp() {
}

void FTacticalBattleScreenDelegationTest::tearDown() {
}

std::string FTacticalBattleScreenDelegationTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalBattleScreenDelegationTest::extractFunctionBody(const std::string & source, const std::string & signature) {
std::string::size_type signaturePos = source.find(signature);
CPPUNIT_ASSERT(signaturePos != std::string::npos);

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

void FTacticalBattleScreenDelegationTest::assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

void FTacticalBattleScreenDelegationTest::testBattleScreenOwnsAndInstallsTacticalModelAndUI() {
// AC: Milestone 7 delegates runtime ownership to FTacticalGame and WX tactical adapter.
const std::string header = readFile(repoFile("include/tactical/FBattleScreen.h"));
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string ctorBody = extractFunctionBody(source,
"FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style )");

assertContains(header, "class FTacticalGame;");
assertContains(header, "class WX");
assertContains(header, "FTacticalGame * m_tacticalGame;");
assertContains(header, "TacticalUI * m_tacticalUI;");

assertContains(source, "#include \"tactical/FTacticalGame.h\"");
assertContains(source, "#include \"gui/WX");
assertContains(ctorBody, "m_tacticalGame = new FTacticalGame();");
assertContains(ctorBody, "m_tacticalUI = new WX");
assertContains(ctorBody, "m_tacticalGame->installUI(m_tacticalUI);");
}

void FTacticalBattleScreenDelegationTest::testBattleScreenDetachesTacticalUIBeforeTeardown() {
// AC: FBattleScreen detaches ITacticalUI seam before deleting owned tactical objects.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string dtorBody = extractFunctionBody(source, "FBattleScreen::~FBattleScreen()");

assertContains(dtorBody, "if (m_tacticalGame) {");
assertContains(dtorBody, "m_tacticalGame->installUI(NULL);");
assertContains(dtorBody, "if (m_tacticalUI) { delete m_tacticalUI; }");
assertContains(dtorBody, "if (m_tacticalGame) { delete m_tacticalGame; }");
}

void FTacticalBattleScreenDelegationTest::testBattleScreenStateAndSelectionApiDelegatesToTacticalModel() {
// AC: state and selection APIs are forwarded from FBattleScreen to FTacticalGame.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));

assertContains(extractFunctionBody(source, "const int & FBattleScreen::getState() const"), "return m_tacticalGame->getState();");
assertContains(extractFunctionBody(source, "void FBattleScreen::setState(int s)"), "m_tacticalGame->setState(s);");
assertContains(extractFunctionBody(source, "FVehicle * FBattleScreen::getShip() const"), "return m_tacticalGame->getShip();");
assertContains(extractFunctionBody(source, "void FBattleScreen::setShip(FVehicle * s)"), "m_tacticalGame->setShip(s);");
assertContains(extractFunctionBody(source, "FWeapon * FBattleScreen::getWeapon()"), "return m_tacticalGame->getWeapon();");
	assertContains(extractFunctionBody(source, "void FBattleScreen::setWeapon(FWeapon * w)"), "m_tacticalGame->setWeapon(w);");
}

void FTacticalBattleScreenDelegationTest::testBattleScreenForwardsInteractionAndRendererAccessorsToModel() {
// AC: FBattleScreen forwards model APIs for interactions and renderer state with redraw behavior where expected.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));

	const std::string selectWeaponBody = extractFunctionBody(source, "bool FBattleScreen::selectWeapon(unsigned int weaponIndex)");
	assertContains(selectWeaponBody, "const bool changed = m_tacticalGame->selectWeapon(weaponIndex);");
	assertContains(selectWeaponBody, "if (changed) {");
	assertContains(selectWeaponBody, "reDraw();");

	const std::string selectDefenseBody = extractFunctionBody(source, "bool FBattleScreen::selectDefense(unsigned int defenseIndex)");
	assertContains(selectDefenseBody, "const bool changed = m_tacticalGame->selectDefense(defenseIndex);");
	assertContains(selectDefenseBody, "reDraw();");

	const std::string handleHexBody = extractFunctionBody(source, "bool FBattleScreen::handleHexClick(const FPoint & hex)");
	assertContains(handleHexBody, "const bool changed = m_tacticalGame->handleHexClick(hex);");
	assertContains(handleHexBody, "reDraw();");

	const std::string completeMoveBody = extractFunctionBody(source, "void FBattleScreen::completeMovePhase()");
	assertContains(completeMoveBody, "m_tacticalGame->completeMovePhase();");
	assertContains(completeMoveBody, "reDraw();");

	const std::string resolveFireBody = extractFunctionBody(source, "FTacticalCombatReportSummary FBattleScreen::resolveCurrentFirePhase()");
	assertContains(resolveFireBody, "FTacticalCombatReportSummary summary = m_tacticalGame->resolveCurrentFirePhase();");
	assertContains(resolveFireBody, "reDraw();");

	assertContains(extractFunctionBody(source, "bool FBattleScreen::isHexMinable(const FPoint & hex)"),
		"return m_tacticalGame->isHexMinable(hex);");
	assertContains(extractFunctionBody(source, "const VehicleList & FBattleScreen::getHexOccupants(const FPoint & hex) const"),
		"return m_tacticalGame->getHexOccupants(hex);");
	assertContains(extractFunctionBody(source, "const std::vector<FPoint> & FBattleScreen::getMovementHexes() const"),
		"return m_tacticalGame->getMovementHexes();");
	assertContains(extractFunctionBody(source, "const std::vector<FPoint> & FBattleScreen::getLeftTurnHexes() const"),
		"return m_tacticalGame->getLeftTurnHexes();");
	assertContains(extractFunctionBody(source, "const std::vector<FPoint> & FBattleScreen::getRightTurnHexes() const"),
		"return m_tacticalGame->getRightTurnHexes();");
	assertContains(extractFunctionBody(source, "const PointSet & FBattleScreen::getTargetHexes() const"),
		"return m_tacticalGame->getTargetHexes();");
	assertContains(extractFunctionBody(source, "const PointSet & FBattleScreen::getHeadOnHexes() const"),
		"return m_tacticalGame->getHeadOnHexes();");
	assertContains(extractFunctionBody(source, "const PointSet & FBattleScreen::getMinedHexes() const"),
		"return m_tacticalGame->getMinedHexes();");
	assertContains(extractFunctionBody(source, "const FHexMap & FBattleScreen::getMineTargets() const"),
		"return m_tacticalGame->getMineTargets();");
	assertContains(extractFunctionBody(source, "unsigned int FBattleScreen::getMineOwner() const"),
		"return m_tacticalGame->getMineOwner();");
	assertContains(extractFunctionBody(source, "const std::map<unsigned int, FTacticalTurnData> & FBattleScreen::getTurnInfo() const"),
		"return m_tacticalGame->getTurnInfo();");
	assertContains(extractFunctionBody(source, "bool FBattleScreen::hasShipPlacementPendingRotation() const"),
		"return m_tacticalGame->hasShipPlacementPendingRotation();");
	assertContains(extractFunctionBody(source, "const FPoint & FBattleScreen::getSelectedShipHex() const"),
		"return m_tacticalGame->getSelectedShipHex();");
	assertContains(extractFunctionBody(source, "const VehicleList & FBattleScreen::getShipsWithMines() const"),
		"return m_tacticalGame->getShipsWithMines();");
	assertContains(extractFunctionBody(source, "bool FBattleScreen::isHexInBounds(const FPoint & hex) const"),
		"return m_tacticalGame->isHexInBounds(hex);");
	assertContains(extractFunctionBody(source, "bool FBattleScreen::isHexOccupied(const FPoint & hex) const"),
		"return m_tacticalGame->isHexOccupied(hex);");
}

void FTacticalBattleScreenDelegationTest::testBattleScreenDamageSummaryDialogDelegatesThroughInstalledUI() {
// AC: FBattleScreen uses ITacticalUI seam owned by FTacticalGame for summary dialogs.
	const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string body = extractFunctionBody(source,
"int FBattleScreen::showTacticalDamageSummaryDialog(const FTacticalCombatReportSummary & summary)");

assertContains(body, "ITacticalUI * tacticalUI = m_tacticalGame->getUI();");
assertContains(body, "return tacticalUI->showDamageSummary(summary);");
assertContains(body, "return 1;");
}

}
