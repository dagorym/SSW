/**
 * @file FTacticalBatteryRangeClampTest.cpp
 * @brief Implementation file for FBattleBoard battery-range clamp source-inspection tests
 * @author Tom Stephens
 * @date Created: Mar 24, 2026
 */

#include "FTacticalBatteryRangeClampTest.h"

#include <fstream>

namespace FrontierTests {

namespace {

std::string repoFile(const std::string & relativePath) {
	return std::string(TACTICAL_TEST_REPO_ROOT) + "/" + relativePath;
}

}

CPPUNIT_TEST_SUITE_REGISTRATION( FTacticalBatteryRangeClampTest );

void FTacticalBatteryRangeClampTest::setUp() {
}

void FTacticalBatteryRangeClampTest::tearDown() {
}

std::string FTacticalBatteryRangeClampTest::readFile(const std::string & path) {
	std::ifstream file(path.c_str());
	CPPUNIT_ASSERT_MESSAGE(path, file.is_open());
	return std::string((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}

std::string FTacticalBatteryRangeClampTest::extractFunctionBody(const std::string & source, const std::string & signature) {
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

std::string::size_type FTacticalBatteryRangeClampTest::assertContainsAndGetIndex(
	const std::string & haystack,
	const std::string & needle) {
	std::string::size_type index = haystack.find(needle);
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Expected to find '") + needle + "' in inspected source",
		index != std::string::npos);
	return index;
}

void FTacticalBatteryRangeClampTest::assertInOrder(
	const std::string & haystack,
	const std::string & first,
	const std::string & second) {
	std::string::size_type firstIndex = assertContainsAndGetIndex(haystack, first);
	std::string::size_type secondIndex = assertContainsAndGetIndex(haystack, second);
	CPPUNIT_ASSERT_MESSAGE(
		std::string("Expected '") + first + "' to appear before '" + second + "'",
		firstIndex < secondIndex);
}

void FTacticalBatteryRangeClampTest::testCenterBoardUsesSymmetricWindowAndDistanceFilter() {
	// AC1: center-board battery scan creates symmetric min/max window in x and y, then filters by hex distance.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleBoard::computeBatteryRange(FPoint &pos, PointSet &tList)");

	assertInOrder(body, "int xMin = pos.getX() - range;", "int xMax = pos.getX() + range;");
	assertInOrder(body, "int yMin = pos.getY() - range;", "int yMax = pos.getY() + range;");
	assertContainsAndGetIndex(body, "if (FHexMap::computeHexDistance(pos.getX(),pos.getY(),i,j)<=(int)range){");
	assertContainsAndGetIndex(body, "tList.insert(FPoint(i,j));");
}

void FTacticalBatteryRangeClampTest::testUpperBoundClampingUsesCorrectGuardAndAssignments() {
	// AC2: xMax/yMax clamp to last valid index and reject prior buggy upper-bound logic.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleBoard::computeBatteryRange(FPoint &pos, PointSet &tList)");

	assertContainsAndGetIndex(body, "if (xMax > m_nCol - 1) { xMax = m_nCol - 1; }");
	assertContainsAndGetIndex(body, "if (yMax > m_nRow - 1) { yMax = m_nRow - 1; }");
	CPPUNIT_ASSERT(body.find("xMax = m_nCol;") == std::string::npos);
	CPPUNIT_ASSERT(body.find("yMax = m_nRow;") == std::string::npos);
	CPPUNIT_ASSERT(body.find("if (xMax < m_nCol)") == std::string::npos);
	assertContainsAndGetIndex(body, "if (xMax > m_nCol - 1)");
}

void FTacticalBatteryRangeClampTest::testLowerBoundClampingAppearsBeforeLoopTraversal() {
	// AC3: xMin/yMin lower clamps exist and run before coordinate traversal begins.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleBoard::computeBatteryRange(FPoint &pos, PointSet &tList)");

	std::string::size_type xLowerIndex = assertContainsAndGetIndex(body, "if (xMin < 0) { xMin = 0; }");
	std::string::size_type yLowerIndex = assertContainsAndGetIndex(body, "if (yMin < 0) { yMin = 0; }");
	std::string::size_type loopIndex = assertContainsAndGetIndex(body, "for (int i=xMin;i<=xMax;i++){");

	CPPUNIT_ASSERT(xLowerIndex < loopIndex);
	CPPUNIT_ASSERT(yLowerIndex < loopIndex);
}

void FTacticalBatteryRangeClampTest::testAllClampStatementsExecuteBeforeLoopHeader() {
	// AC4: all four clamps are established before the nested loops to prevent any off-map visitation path.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleBoard::computeBatteryRange(FPoint &pos, PointSet &tList)");

	std::string::size_type loopIndex = assertContainsAndGetIndex(body, "for (int i=xMin;i<=xMax;i++){");

	CPPUNIT_ASSERT(assertContainsAndGetIndex(body, "if (xMin < 0) { xMin = 0; }") < loopIndex);
	CPPUNIT_ASSERT(assertContainsAndGetIndex(body, "if (xMax > m_nCol - 1) { xMax = m_nCol - 1; }") < loopIndex);
	CPPUNIT_ASSERT(assertContainsAndGetIndex(body, "if (yMin < 0) { yMin = 0; }") < loopIndex);
	CPPUNIT_ASSERT(assertContainsAndGetIndex(body, "if (yMax > m_nRow - 1) { yMax = m_nRow - 1; }") < loopIndex);
}

void FTacticalBatteryRangeClampTest::testLoopBoundariesUseInclusiveClampedInvariants() {
	// AC5: inclusive loops rely on previously clamped xMin/xMax/yMin/yMax boundaries.
	const std::string source = readFile(repoFile("src/tactical/FBattleBoard.cpp"));
	const std::string body = extractFunctionBody(source, "void FBattleBoard::computeBatteryRange(FPoint &pos, PointSet &tList)");

	std::string::size_type xLowerClampIndex = assertContainsAndGetIndex(body, "if (xMin < 0) { xMin = 0; }");
	std::string::size_type xUpperClampIndex = assertContainsAndGetIndex(body, "if (xMax > m_nCol - 1) { xMax = m_nCol - 1; }");
	std::string::size_type yUpperClampIndex = assertContainsAndGetIndex(body, "if (yMax > m_nRow - 1) { yMax = m_nRow - 1; }");
	std::string::size_type iLoopIndex = assertContainsAndGetIndex(body, "for (int i=xMin;i<=xMax;i++){");
	std::string::size_type jLoopIndex = assertContainsAndGetIndex(body, "for (int j=yMin;j<=yMax;j++){");

	CPPUNIT_ASSERT(xLowerClampIndex < iLoopIndex);
	CPPUNIT_ASSERT(xUpperClampIndex < iLoopIndex);
	CPPUNIT_ASSERT(yUpperClampIndex < jLoopIndex);
	CPPUNIT_ASSERT(iLoopIndex < jLoopIndex);
}

}
