/**
 * @file FTacticalGameHeaderTest.cpp
 * @brief Implementation file for FTacticalGame header coverage tests
 */

#include "FTacticalGameHeaderTest.h"

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

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalGameHeaderTest );

void FTacticalGameHeaderTest::setUp() {
}

void FTacticalGameHeaderTest::tearDown() {
}

std::string FTacticalGameHeaderTest::readFile(const std::string & path) {
std::ifstream file(path.c_str());
CPPUNIT_ASSERT_EQUAL(true, file.is_open());
return std::string((std::istreambuf_iterator<char>(file)),
std::istreambuf_iterator<char>());
}

void FTacticalGameHeaderTest::testHeaderDeclaresTacticalModelState() {
// AC: FTacticalGame.h declares a compilable, non-wx tactical model class
// and covers tactical battle/movement/selection/report/ICM/hex-map mechanics state.
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));

CPPUNIT_ASSERT_EQUAL(true, contains(header, "class ITacticalUI;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "class FTacticalGame"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "ITacticalUI * m_ui;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "int m_state;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "int m_phase;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "FVehicle * m_curShip;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "FWeapon * m_curWeapon;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "std::vector<ICMData *> m_ICMData;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "FTacticalCombatReport m_tacticalReport;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "FTacticalHexData m_hexData[100][100];"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "std::map<unsigned int, FTacticalTurnData> m_turnInfo;"));
}

void FTacticalGameHeaderTest::testHeaderProvidesHexAndTurnOwnershipTypes() {
// AC: new tactical model ownership bookkeeping types are available from FTacticalGame.h
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));

CPPUNIT_ASSERT_EQUAL(true, contains(header, "typedef struct {"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "} FTacticalHexData;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "VehicleList ships;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "} FTacticalTurnData;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "std::vector<int> turns;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "std::map<FPoint, int> gravityTurns;"));
CPPUNIT_ASSERT_EQUAL(true, contains(header, "FHexPath path;"));
}

void FTacticalGameHeaderTest::testHeaderCompilesWithoutWxIncludes() {
// AC: header include surface compiles without any wx headers.
const std::string header = readFile(repoFile("include/tactical/FTacticalGame.h"));
CPPUNIT_ASSERT_EQUAL(false, contains(header, "#include <wx"));
CPPUNIT_ASSERT_EQUAL(false, contains(header, "#include \"wx/"));
CPPUNIT_ASSERT_EQUAL(false, contains(header, "wxWindow"));

const std::string includePath = std::string(TACTICAL_TEST_REPO_ROOT) + "/include";
const std::string cmd =
"printf '#include \"tactical/FTacticalGame.h\"\\n"
"int main(){Frontier::FTacticalHexData h; Frontier::FTacticalTurnData t; Frontier::FTacticalGame *g = 0; (void)h; (void)t; (void)g; return 0;}\\n'"
" | g++ -x c++ - -I" + includePath + " -c -o /dev/null";
const int rc = std::system(cmd.c_str());
CPPUNIT_ASSERT_EQUAL(0, rc);
}

} // namespace FrontierTests
