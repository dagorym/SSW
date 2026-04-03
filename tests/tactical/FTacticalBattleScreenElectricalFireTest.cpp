/**
 * @file FTacticalBattleScreenElectricalFireTest.cpp
 * @brief Implementation file for the FTacticalBattleScreenElectricalFireTest class
 */

#include "FTacticalBattleScreenElectricalFireTest.h"

#include <fstream>
#include <iterator>
#include <vector>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

void assertContains(const std::string & haystack, const std::string & needle) {
CPPUNIT_ASSERT_MESSAGE(
std::string("Expected to find '") + needle + "' in inspected source",
haystack.find(needle) != std::string::npos);
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBattleScreenElectricalFireTest );

void FTacticalBattleScreenElectricalFireTest::setUp() {
}

void FTacticalBattleScreenElectricalFireTest::tearDown() {
}

std::string FTacticalBattleScreenElectricalFireTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

std::string FTacticalBattleScreenElectricalFireTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

void FTacticalBattleScreenElectricalFireTest::assertAppearsInOrder(const std::string & haystack, const std::vector<std::string> & needles) {
std::string::size_type searchFrom = 0;
for (std::vector<std::string>::const_iterator itr = needles.begin(); itr != needles.end(); ++itr) {
std::string::size_type found = haystack.find(*itr, searchFrom);
CPPUNIT_ASSERT_MESSAGE(std::string("Expected to find '") + *itr + "' in the requested source order", found != std::string::npos);
searchFrom = found + itr->size();
}
}

unsigned int FTacticalBattleScreenElectricalFireTest::countOccurrences(const std::string & haystack, const std::string & needle) {
unsigned int count = 0;
std::string::size_type searchFrom = 0;
while (true) {
std::string::size_type found = haystack.find(needle, searchFrom);
if (found == std::string::npos) {
return count;
}
count++;
searchFrom = found + needle.size();
}
}

void FTacticalBattleScreenElectricalFireTest::testApplyFireDamageDelegatesToTacticalModel() {
// AC: electrical fire damage resolution is delegated to FTacticalGame.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleScreen::applyFireDamage()");

assertContains(body, "m_tacticalGame->applyFireDamage();");
CPPUNIT_ASSERT(body.find("beginTacticalReport") == std::string::npos);
CPPUNIT_ASSERT(body.find("m_tacticalReport") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testDeclareWinnerUsesTacticalUIAndGuardedClosePath() {
// AC: winner declaration notifies through ITacticalUI and exits via guarded close helper.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string body = extractFunctionBody(source, "void FBattleScreen::declareWinner()");

assertContains(body, "ITacticalUI * tacticalUI = m_tacticalGame->getUI();");
assertContains(body, "tacticalUI->notifyWinner(attackerWins);");
assertContains(body, "closeBattleScreen(0);");
CPPUNIT_ASSERT(body.find("wxMessageBox") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testCloseBattleScreenUsesModelCloseGuardWithModalFirstPath() {
// AC: close logic stays behind one guarded helper, ending modal sessions before any non-modal Destroy() path.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string closeBody = extractFunctionBody(source, "void FBattleScreen::closeBattleScreen(int returnCode)");
const std::string onCloseBody = extractFunctionBody(source, "void FBattleScreen::onClose(wxCloseEvent & event)");

std::vector<std::string> closeSequence;
closeSequence.push_back("if (m_tacticalGame->isCloseInProgress()) {");
closeSequence.push_back("return;");
closeSequence.push_back("m_tacticalGame->setCloseInProgress(true);");
closeSequence.push_back("if (IsModal()) {");
closeSequence.push_back("EndModal(returnCode);");
closeSequence.push_back("return;");
closeSequence.push_back("SetReturnCode(returnCode);");
closeSequence.push_back("Destroy();");
assertAppearsInOrder(closeBody, closeSequence);

CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(closeBody, "Destroy();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(closeBody, "EndModal(returnCode);"));

std::vector<std::string> onCloseSequence;
onCloseSequence.push_back("if (m_tacticalGame->isCloseInProgress()) {");
onCloseSequence.push_back("event.Skip();");
onCloseSequence.push_back("return;");
onCloseSequence.push_back("closeBattleScreen(GetReturnCode());");
assertAppearsInOrder(onCloseBody, onCloseSequence);
}

void FTacticalBattleScreenElectricalFireTest::testModalCallerSitesOwnBattleScreenLifetimeAfterShowModal() {
// AC: modal caller sites keep ownership outside FBattleScreen by using stack dialogs instead of self-destruction.
const std::string selectCombatSource = readFile(repoFile("src/gui/SelectCombatGUI.cpp"));
const std::string scenarioDialogSource = readFile(repoFile("src/battleSim/ScenarioDialog.cpp"));
const std::string scenarioEditorSource = readFile(repoFile("src/battleSim/ScenarioEditorGUI.cpp"));

assertAppearsInOrder(selectCombatSource, std::vector<std::string>(1, "FBattleScreen bb;"));
assertAppearsInOrder(selectCombatSource, std::vector<std::string>(1, "bb.ShowModal();"));
CPPUNIT_ASSERT(selectCombatSource.find("new FBattleScreen(") == std::string::npos);
CPPUNIT_ASSERT(selectCombatSource.find("delete bb;") == std::string::npos);

CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), countOccurrences(scenarioDialogSource, "FBattleScreen bb;"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), countOccurrences(scenarioDialogSource, "bb.ShowModal();"));
CPPUNIT_ASSERT(scenarioDialogSource.find("new FBattleScreen(") == std::string::npos);
CPPUNIT_ASSERT(scenarioDialogSource.find("delete bb;") == std::string::npos);

assertContains(scenarioEditorSource, "FBattleScreen bb;");
assertContains(scenarioEditorSource, "bb.ShowModal();");
CPPUNIT_ASSERT(scenarioEditorSource.find("new FBattleScreen(") == std::string::npos);
CPPUNIT_ASSERT(scenarioEditorSource.find("delete bb;") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testBattleSimLaunchPathUsesModalDialogFlowIntoBattleScreen() {
// AC: BattleSim launch path keeps modal dialog flow and reaches tactical launchers that own FBattleScreen on stack.
const std::string battleSimFrameSource = readFile(repoFile("src/battleSim/BattleSimFrame.cpp"));
const std::string localGameSource = readFile(repoFile("src/battleSim/LocalGameDialog.cpp"));
const std::string scenarioDialogSource = readFile(repoFile("src/battleSim/ScenarioDialog.cpp"));
const std::string scenarioEditorSource = readFile(repoFile("src/battleSim/ScenarioEditorGUI.cpp"));

const std::string playLocalBody = extractFunctionBody(battleSimFrameSource, "void BattleSimFrame::onPlayLocal( wxCommandEvent& event )");
assertAppearsInOrder(playLocalBody, std::vector<std::string>(1, "LocalGameDialog d(this);"));
assertAppearsInOrder(playLocalBody, std::vector<std::string>(1, "d.ShowModal();"));
CPPUNIT_ASSERT(playLocalBody.find("new LocalGameDialog(") == std::string::npos);
CPPUNIT_ASSERT(playLocalBody.find("d.Show();") == std::string::npos);

const std::string predefinedBody = extractFunctionBody(localGameSource, "void LocalGameDialog::onPlayPredefined( wxCommandEvent& event )");
assertAppearsInOrder(predefinedBody, std::vector<std::string>(1, "ScenarioDialog d(this);"));
assertAppearsInOrder(predefinedBody, std::vector<std::string>(1, "d.ShowModal();"));
CPPUNIT_ASSERT(predefinedBody.find("new ScenarioDialog(") == std::string::npos);

const std::string customBody = extractFunctionBody(localGameSource, "void LocalGameDialog::onCreateNew( wxCommandEvent& event )");
assertAppearsInOrder(customBody, std::vector<std::string>(1, "ScenarioEditorGUI d(this);"));
assertAppearsInOrder(customBody, std::vector<std::string>(1, "d.ShowModal();"));
CPPUNIT_ASSERT(customBody.find("new ScenarioEditorGUI(") == std::string::npos);

CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), countOccurrences(scenarioDialogSource, "Hide();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), countOccurrences(scenarioDialogSource, "Show();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(4), countOccurrences(scenarioDialogSource, "bb.ShowModal();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(scenarioEditorSource, "bb.ShowModal();"));
CPPUNIT_ASSERT(scenarioEditorSource.find("bb.Show();") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testBattleScreenConstructorOwnsAndInstallsTacticalGameAndUI() {
// AC: FBattleScreen constructor owns FTacticalGame and WXTacticalUI installation path.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string ctorBody = extractFunctionBody(source, "FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style )");

assertContains(source, "#include \"tactical/FTacticalGame.h\"");
assertContains(source, "#include \"gui/WX");
assertContains(ctorBody, "m_tacticalGame = new FTacticalGame();");
assertContains(ctorBody, "m_tacticalUI = new WX");
assertContains(ctorBody, "m_tacticalGame->installUI(m_tacticalUI);");
CPPUNIT_ASSERT(ctorBody.find("m_tacticalGame = NULL") == std::string::npos);
CPPUNIT_ASSERT(ctorBody.find("m_tacticalUI = NULL") == std::string::npos);
}

}
