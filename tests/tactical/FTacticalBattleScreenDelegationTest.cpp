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
