/**
 * @file FTacticalBattleDisplayFireFlowTest.h
 * @brief Header file for the FTacticalBattleDisplayFireFlowTest class
 */

#ifndef FTacticalBattleDisplayFireFlowTest_H_
#define FTacticalBattleDisplayFireFlowTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalBattleDisplayFireFlowTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalBattleDisplayFireFlowTest );
	CPPUNIT_TEST( testBuildTacticalAttackReportPreservesStructuredAttackAndEffectDetailData );
	CPPUNIT_TEST( testFireAllWeaponsShowsSharedSummaryBeforeDestroyedShipCleanup );
	CPPUNIT_TEST( testDefensiveFireDoneWaitsForSummaryAcknowledgementBeforeAdvancingToOffensiveFire );
	CPPUNIT_TEST( testOffensiveFireDoneWaitsForSummaryAcknowledgementBeforeReturningToMovement );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);
	std::string extractFunctionBody(const std::string & source, const std::string & signature);
	void assertContains(const std::string & haystack, const std::string & needle);
	void assertBefore(const std::string & haystack, const std::string & first, const std::string & second);
	unsigned int countOccurrences(const std::string & haystack, const std::string & needle);

public:
	void setUp();
	void tearDown();

	void testBuildTacticalAttackReportPreservesStructuredAttackAndEffectDetailData();
	void testFireAllWeaponsShowsSharedSummaryBeforeDestroyedShipCleanup();
	void testDefensiveFireDoneWaitsForSummaryAcknowledgementBeforeAdvancingToOffensiveFire();
	void testOffensiveFireDoneWaitsForSummaryAcknowledgementBeforeReturningToMovement();
};

}

#endif
