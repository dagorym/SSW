/**
 * @file ITacticalUIBoundaryTest.cpp
 * @brief Implementation file for ITacticalUI boundary acceptance tests
 */

#include "ITacticalUIBoundaryTest.h"

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

CPPUNIT_TEST_SUITE_REGISTRATION( ITacticalUIBoundaryTest );

void ITacticalUIBoundaryTest::setUp() {
}

void ITacticalUIBoundaryTest::tearDown() {
}

std::string ITacticalUIBoundaryTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_EQUAL(true, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

int ITacticalUIBoundaryTest::countToken(const std::string & source, const std::string & token) {
int count = 0;
std::string::size_type pos = source.find(token);
while (pos != std::string::npos) {
count++;
pos = source.find(token, pos + token.size());
}
return count;
}

void ITacticalUIBoundaryTest::testHeaderDeclaresFrontierITacticalUI() {
// AC: include/tactical/ITacticalUI.h exists and declares Frontier::ITacticalUI.
const std::string header = readFile(repoFile("include/tactical/ITacticalUI.h"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "namespace Frontier"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "class ITacticalUI"));
}

void ITacticalUIBoundaryTest::testInterfaceDeclaresExactlyRequiredCallbacks() {
// AC: interface contains exactly the required callback methods/signatures and no additional callbacks.
const std::string header = readFile(repoFile("include/tactical/ITacticalUI.h"));

CPPUNIT_ASSERT_EQUAL(true, contains(header, "virtual void requestRedraw() = 0;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "virtual void showMessage(const std::string& title, const std::string& body) = 0;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "virtual int showDamageSummary(const FTacticalCombatReportSummary& summary) = 0;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "virtual int runICMSelection(std::vector<ICMData*>& icmData, VehicleList* defenders) = 0;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "virtual void notifyWinner(bool attackerWins) = 0;"));
CPPUNIT_ASSERT_EQUAL(5, countToken(header, "= 0;"));
}

void ITacticalUIBoundaryTest::testHeaderHasNoWxIncludesOrTypes() {
// AC: header contains no wx includes and no wx type references.
const std::string header = readFile(repoFile("include/tactical/ITacticalUI.h"));
CPPUNIT_ASSERT_EQUAL(false, contains(header, "#include <wx"));
CPPUNIT_ASSERT_EQUAL(false, contains(header, "#include \"wx/"));
CPPUNIT_ASSERT_EQUAL(false, contains(header, "wxWindow"));
CPPUNIT_ASSERT_EQUAL(false, contains(header, "wxString"));
}

void ITacticalUIBoundaryTest::testBoundaryCompilesForTacticalModelUse() {
// AC: ITacticalUI is usable by FTacticalGame without GUI includes and with required callback signatures.
const std::string includePath = repoFile("include");
const std::string cmd =
"printf '#include <type_traits>\\n"
"#include \"tactical/ITacticalUI.h\"\\n"
"#include \"tactical/FTacticalGame.h\"\\n"
"static_assert(std::is_same<decltype(&Frontier::ITacticalUI::requestRedraw), void (Frontier::ITacticalUI::*)()>::value, \"requestRedraw signature mismatch\");\\n"
"static_assert(std::is_same<decltype(&Frontier::ITacticalUI::showMessage), void (Frontier::ITacticalUI::*)(const std::string&, const std::string&)>::value, \"showMessage signature mismatch\");\\n"
"static_assert(std::is_same<decltype(&Frontier::ITacticalUI::showDamageSummary), int (Frontier::ITacticalUI::*)(const Frontier::FTacticalCombatReportSummary&)>::value, \"showDamageSummary signature mismatch\");\\n"
"static_assert(std::is_same<decltype(&Frontier::ITacticalUI::runICMSelection), int (Frontier::ITacticalUI::*)(std::vector<Frontier::ICMData*>&, Frontier::VehicleList*)>::value, \"runICMSelection signature mismatch\");\\n"
"static_assert(std::is_same<decltype(&Frontier::ITacticalUI::notifyWinner), void (Frontier::ITacticalUI::*)(bool)>::value, \"notifyWinner signature mismatch\");\\n"
"int main(){Frontier::FTacticalGame *g = 0; (void)g; return 0;}\\n'"
" | g++ -x c++ - -I" + includePath + " -c -o /dev/null";
const int rc = std::system(cmd.c_str());
CPPUNIT_ASSERT_EQUAL(0, rc);
}

} // namespace FrontierTests
