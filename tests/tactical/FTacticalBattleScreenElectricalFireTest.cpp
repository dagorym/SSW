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
// AC: close logic stays behind one guarded helper with FBattleScreen-owned close state and shared vector routing.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string closeBody = extractFunctionBody(source, "void FBattleScreen::closeBattleScreen(int returnCode)");
const std::string onCloseBody = extractFunctionBody(source, "void FBattleScreen::onClose(wxCloseEvent & event)");
const std::string menuQuitBody = extractFunctionBody(source, "void FBattleScreen::onMenuQuit(wxCommandEvent & WXUNUSED(event))");

assertContains(closeBody, "if (m_closeInProgress) {");
assertContains(closeBody, "m_closeInProgress = true;");
assertContains(closeBody, "if (IsModal()) {");
assertContains(closeBody, "EndModal(returnCode);");
assertContains(closeBody, "SetReturnCode(returnCode);");
assertContains(closeBody, "Hide();");
assertContains(closeBody, "Destroy();");
assertContains(closeBody, "if (!IsBeingDeleted()) {");
assertContains(closeBody, "m_closeInProgress = false;");

CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(closeBody, "Destroy();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(closeBody, "EndModal(returnCode);"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(closeBody, "if (m_closeInProgress) {"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(closeBody, "m_closeInProgress = true;"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(closeBody, "m_closeInProgress = false;"));
CPPUNIT_ASSERT(closeBody.find("m_tacticalGame->isCloseInProgress()") == std::string::npos);
CPPUNIT_ASSERT(closeBody.find("m_tacticalGame->setCloseInProgress(") == std::string::npos);

std::vector<std::string> onCloseSequence;
onCloseSequence.push_back("if (m_closeInProgress) {");
onCloseSequence.push_back("closeBattleScreen(GetReturnCode());");
assertAppearsInOrder(onCloseBody, onCloseSequence);
CPPUNIT_ASSERT(onCloseBody.find("event.Skip();") == std::string::npos);
CPPUNIT_ASSERT(onCloseBody.find("m_tacticalGame->isCloseInProgress()") == std::string::npos);
assertContains(menuQuitBody, "Close(true);");
CPPUNIT_ASSERT(menuQuitBody.find("closeBattleScreen(") == std::string::npos);

CPPUNIT_ASSERT(source.find("exit(") == std::string::npos);
CPPUNIT_ASSERT(source.find("ExitMainLoop") == std::string::npos);
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

const char * scenarioSignatures[] = {
"void ScenarioDialog::onScenario1( wxCommandEvent& event )",
"void ScenarioDialog::onScenario2( wxCommandEvent& event )",
"void ScenarioDialog::onScenario3( wxCommandEvent& event )",
"void ScenarioDialog::onScenario4( wxCommandEvent& event )"
};
for (size_t i = 0; i < sizeof(scenarioSignatures) / sizeof(scenarioSignatures[0]); ++i) {
const std::string scenarioBody = extractFunctionBody(scenarioDialogSource, scenarioSignatures[i]);
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(scenarioBody, "Hide();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(scenarioBody, "bb.ShowModal();"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(scenarioBody, "Show();"));
std::vector<std::string> scenarioSequence;
scenarioSequence.push_back("Hide();");
scenarioSequence.push_back("bb.ShowModal();");
scenarioSequence.push_back("Show();");
assertAppearsInOrder(scenarioBody, scenarioSequence);
CPPUNIT_ASSERT(scenarioBody.find("bb.Show();") == std::string::npos);
}

CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(scenarioEditorSource, "bb.ShowModal();"));
CPPUNIT_ASSERT(scenarioEditorSource.find("bb.Show();") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testBattleScreenConstructorOwnsAndInstallsTacticalGameAndUI() {
// AC: FBattleScreen constructor owns FTacticalGame and WXTacticalUI installation path.
// AC: FBattleBoard and FBattleDisplay stay parented and layout-managed by FBattleScreen.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string ctorBody = extractFunctionBody(source, "FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style )");

assertContains(source, "#include \"tactical/FTacticalGame.h\"");
assertContains(source, "#include \"gui/WX");
assertContains(ctorBody, "m_tacticalGame = new FTacticalGame();");
assertContains(ctorBody, "m_tacticalUI = new WX");
assertContains(ctorBody, "m_tacticalGame->installUI(m_tacticalUI);");
assertContains(ctorBody, "m_map = new FBattleBoard( this );");
assertContains(ctorBody, "m_display = new FBattleDisplay( this );");
assertContains(ctorBody, "this->SetSizer( fgSizer1 );");
assertContains(ctorBody, "applyLayoutPolicy();");
CPPUNIT_ASSERT(ctorBody.find("m_tacticalGame = NULL") == std::string::npos);
CPPUNIT_ASSERT(ctorBody.find("m_tacticalUI = NULL") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testBattleScreenConstructorBuildsMenuBarAndQuitBinding() {
// AC: FBattleScreen constructor installs File/Settings/Help menu bar with disabled non-Quit entries.
// AC: Quit command is the only active wxEVT_MENU binding and routes to the shared close event path.
const std::string source = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string ctorBody = extractFunctionBody(source, "FBattleScreen::FBattleScreen(const wxString& title, const wxPoint& pos, const wxSize& size, long style )");
const std::string menuQuitBody = extractFunctionBody(source, "void FBattleScreen::onMenuQuit(wxCommandEvent & WXUNUSED(event))");

assertContains(ctorBody, "wxMenu *menuFile = new wxMenu;");
assertContains(ctorBody, "wxMenu *menuSettings = new wxMenu;");
assertContains(ctorBody, "wxMenu *menuHelp = new wxMenu;");
assertContains(ctorBody, "menuFile->Append(ID_TacticalLoadGame, \"&Load Game\");");
assertContains(ctorBody, "menuFile->Append(ID_TacticalSaveGame, \"&Save Game\");");
assertContains(ctorBody, "menuFile->Append(ID_TacticalQuit, \"&Quit\");");
assertContains(ctorBody, "menuSettings->Append(ID_TacticalDamageDetails, \"&Damage Details\");");
assertContains(ctorBody, "menuHelp->Append(ID_TacticalUsersGuide, \"&User's Guide\");");
assertContains(ctorBody, "menuHelp->Append(ID_TacticalAbout, \"&About\");");
assertContains(ctorBody, "menuFile->Enable(ID_TacticalLoadGame, false);");
assertContains(ctorBody, "menuFile->Enable(ID_TacticalSaveGame, false);");
assertContains(ctorBody, "menuSettings->Enable(ID_TacticalDamageDetails, false);");
assertContains(ctorBody, "menuHelp->Enable(ID_TacticalUsersGuide, false);");
assertContains(ctorBody, "menuHelp->Enable(ID_TacticalAbout, false);");
assertContains(ctorBody, "wxMenuBar *menuBar = new wxMenuBar;");
assertAppearsInOrder(ctorBody, std::vector<std::string>{
	"menuBar->Append(menuFile, \"&File\");",
	"menuBar->Append(menuSettings, \"&Settings\");",
	"menuBar->Append(menuHelp, \"&Help\");",
	"SetMenuBar(menuBar);"
});
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(ctorBody, "Bind(wxEVT_MENU, &FBattleScreen::onMenuQuit, this, ID_TacticalQuit);"));
CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(1), countOccurrences(ctorBody, "Bind(wxEVT_MENU,"));
assertContains(menuQuitBody, "Close(true);");
CPPUNIT_ASSERT(menuQuitBody.find("closeBattleScreen(") == std::string::npos);
CPPUNIT_ASSERT(menuQuitBody.find("exit(") == std::string::npos);
CPPUNIT_ASSERT(source.find("exit(") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testTacticalMenuCommandIDsDeclaredInSharedWxHeader() {
// AC: tactical menu IDs are declared in shared wxWidgets command enum.
const std::string header = readFile(repoFile("include/wxWidgets.h"));
assertContains(header, "ID_TacticalLoadGame");
assertContains(header, "ID_TacticalSaveGame");
assertContains(header, "ID_TacticalQuit");
assertContains(header, "ID_TacticalDamageDetails");
assertContains(header, "ID_TacticalUsersGuide");
assertContains(header, "ID_TacticalAbout");
}

void FTacticalBattleScreenElectricalFireTest::testBattleScreenHeaderDeclaresFrameModalAndLifecycleContracts() {
// AC: FBattleScreen is a wxFrame-backed top-level with modal compatibility and lifecycle APIs.
const std::string header = readFile(repoFile("include/tactical/FBattleScreen.h"));

assertContains(header, "class FBattleScreen : public wxFrame");
assertContains(header, "int ShowModal();");
assertContains(header, "void EndModal(int returnCode);");
assertContains(header, "bool IsModal() const;");
assertContains(header, "void SetReturnCode(int returnCode);");
assertContains(header, "int GetReturnCode() const;");
assertContains(header, "static void resetLifecycleCounters();");
assertContains(header, "static int getConstructedCount();");
assertContains(header, "static int getDestroyedCount();");
assertContains(header, "static int getLiveInstanceCount();");
CPPUNIT_ASSERT(header.find("class FBattleScreen : public wxDialog") == std::string::npos);
}

void FTacticalBattleScreenElectricalFireTest::testMainFrameLaunchPathKeepsNonModalBattleScreenUsage() {
// AC: Existing non-modal launch paths stay source-compatible and still use Show(true).
const std::string mainFrameSource = readFile(repoFile("src/FMainFrame.cpp"));

assertContains(mainFrameSource, "FBattleScreen *bb = new FBattleScreen();");
assertContains(mainFrameSource, "bb->Show(true);");
CPPUNIT_ASSERT(mainFrameSource.find("bb->ShowModal();") == std::string::npos);
}

}
