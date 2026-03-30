/**
 * @file WXTacticalUIAdapterTest.cpp
 * @brief Implementation file for WXTacticalUI adapter acceptance tests
 */

#include "WXTacticalUIAdapterTest.h"

#include <cstdlib>
#include <fstream>
#include <iterator>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

bool contains(const std::string & source, const std::string & token) {
return source.find(token) != std::string::npos;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( WXTacticalUIAdapterTest );

void WXTacticalUIAdapterTest::setUp() {
}

void WXTacticalUIAdapterTest::tearDown() {
}

std::string WXTacticalUIAdapterTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_EQUAL(true, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

void WXTacticalUIAdapterTest::testHeaderDeclaresAdapterImplementingInterface() {
// AC: WXTacticalUI publicly implements ITacticalUI and constructor accepts wxWindow*.
const std::string header = readFile(repoFile("include/gui/WXTacticalUI.h"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "class WXTacticalUI : public ITacticalUI"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "explicit WXTacticalUI(wxWindow* parent);"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "wxWindow* m_parent;"));
}

void WXTacticalUIAdapterTest::testConstructorStoresParentAndRedrawUsesRefreshSafely() {
// AC: constructor stores parent and requestRedraw() only refreshes when parent exists.
const std::string source = readFile(repoFile("src/gui/WXTacticalUI.cpp"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "WXTacticalUI::WXTacticalUI(wxWindow* parent) : m_parent(parent) {}"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "void WXTacticalUI::requestRedraw() {"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "if (m_parent != NULL) {"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "m_parent->Refresh();"));
}

void WXTacticalUIAdapterTest::testShowMessageMatchesRepositoryInformationalPattern() {
// AC: showMessage follows repository informational dialog pattern with null-parent fallback.
const std::string source = readFile(repoFile("src/gui/WXTacticalUI.cpp"));
const std::string strategic = readFile(repoFile("src/gui/WXStrategicUI.cpp"));

CPPUNIT_ASSERT_EQUAL(true, contains(source, "wxMessageDialog dialog(m_parent, body, title, wxOK | wxICON_INFORMATION);"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "dialog.ShowModal();"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "wxMessageOutputBest().Printf(\"%s\\n%s\", title.c_str(), body.c_str());"));

// Confirm consistent fallback pattern with existing WXStrategicUI behavior.
CPPUNIT_ASSERT_EQUAL(true, contains(strategic, "wxMessageOutputBest().Printf(\"%s\\n%s\", title.c_str(), body.c_str());"));
}

void WXTacticalUIAdapterTest::testDamageSummaryAndICMSelectionWrapDialogsWithGuards() {
// AC: dialog wrappers call modal dialogs and return guarded non-zero on unusable context/data.
const std::string source = readFile(repoFile("src/gui/WXTacticalUI.cpp"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "int WXTacticalUI::showDamageSummary(const FTacticalCombatReportSummary& summary) {"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "if (m_parent == NULL) {"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "return 1;"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "TacticalDamageSummaryGUI dialog(m_parent, summary);"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "return dialog.ShowModal();"));

CPPUNIT_ASSERT_EQUAL(true, contains(source, "int WXTacticalUI::runICMSelection(std::vector<ICMData*>& icmData,"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "if (m_parent == NULL || defenders == NULL || defenders->size() == 0 ||"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "icmData.size() == 0) {"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "ICMSelectionGUI dialog(m_parent, &icmData);"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "return dialog.ShowModal();"));
}

void WXTacticalUIAdapterTest::testNotifyWinnerPreservesBattleScreenWinnerMessageSemantics() {
// AC: winner title/message semantics match FBattleScreen::declareWinner logic.
const std::string source = readFile(repoFile("src/gui/WXTacticalUI.cpp"));
const std::string battleScreen = readFile(repoFile("src/tactical/FBattleScreen.cpp"));

CPPUNIT_ASSERT_EQUAL(true, contains(source, "std::string msg = \"The winner of the battle is \\nPlayer \";"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "msg += attackerWins ? \"Sathar\" : \"UPF\";"));
CPPUNIT_ASSERT_EQUAL(true, contains(source, "showMessage(\"Enemy Defeated!\", msg);"));

CPPUNIT_ASSERT_EQUAL(true, contains(battleScreen, "std::string msg = \"The winner of the battle is \\nPlayer \";"));
CPPUNIT_ASSERT_EQUAL(true, contains(battleScreen, "msg+= (getActivePlayer())?\"Sathar\":\"UPF\";"));
CPPUNIT_ASSERT_EQUAL(true, contains(battleScreen, "wxMessageBox( msg, \"Enemy Defeated!\", wxOK | wxICON_INFORMATION );"));
}

void WXTacticalUIAdapterTest::testRuntimeTacticalCallersNotRewiredToAdapter() {
// AC: active runtime tactical callers have not been rewired to WXTacticalUI in this subtask.
const std::string battleScreenHeader = readFile(repoFile("include/tactical/FBattleScreen.h"));
const std::string battleScreenSource = readFile(repoFile("src/tactical/FBattleScreen.cpp"));
const std::string battleBoardHeader = readFile(repoFile("include/tactical/FBattleBoard.h"));
const std::string battleDisplayHeader = readFile(repoFile("include/tactical/FBattleDisplay.h"));

CPPUNIT_ASSERT_EQUAL(false, contains(battleScreenHeader, "WXTacticalUI"));
CPPUNIT_ASSERT_EQUAL(false, contains(battleScreenSource, "WXTacticalUI"));
CPPUNIT_ASSERT_EQUAL(false, contains(battleBoardHeader, "WXTacticalUI"));
CPPUNIT_ASSERT_EQUAL(false, contains(battleDisplayHeader, "WXTacticalUI"));
}

void WXTacticalUIAdapterTest::testAdapterSourceCompilesStandalone() {
// AC: additive adapter source compiles directly even before GUI Makefile wiring subtask.
const std::string includePath = repoFile("include");
const std::string cxxflags = "`wx-config --cxxflags`";
const std::string cmd =
"g++ -c " + repoFile("src/gui/WXTacticalUI.cpp") +
" -I" + includePath +
" -g " + cxxflags +
" -Wall -Woverloaded-virtual -DLINUX -o /dev/null";
const int rc = std::system(cmd.c_str());
CPPUNIT_ASSERT_EQUAL(0, rc);
}

} // namespace FrontierTests
