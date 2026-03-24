/**
 * @file FTacticalBatteryRangeClampTest.h
 * @brief Header file for FBattleBoard battery-range clamp source-inspection tests
 * @author Tom Stephens
 * @date Created: Mar 24, 2026
 */

#ifndef FTacticalBatteryRangeClampTest_H_
#define FTacticalBatteryRangeClampTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalBatteryRangeClampTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalBatteryRangeClampTest );
	CPPUNIT_TEST( testCenterBoardUsesSymmetricWindowAndDistanceFilter );
	CPPUNIT_TEST( testUpperBoundClampingUsesCorrectGuardAndAssignments );
	CPPUNIT_TEST( testLowerBoundClampingAppearsBeforeLoopTraversal );
	CPPUNIT_TEST( testAllClampStatementsExecuteBeforeLoopHeader );
	CPPUNIT_TEST( testLoopBoundariesUseInclusiveClampedInvariants );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);
	std::string extractFunctionBody(const std::string & source, const std::string & signature);
	std::string::size_type assertContainsAndGetIndex(const std::string & haystack, const std::string & needle);
	void assertInOrder(const std::string & haystack, const std::string & first, const std::string & second);

public:
	void setUp();
	void tearDown();

	void testCenterBoardUsesSymmetricWindowAndDistanceFilter();
	void testUpperBoundClampingUsesCorrectGuardAndAssignments();
	void testLowerBoundClampingAppearsBeforeLoopTraversal();
	void testAllClampStatementsExecuteBeforeLoopHeader();
	void testLoopBoundariesUseInclusiveClampedInvariants();
};

}

#endif
