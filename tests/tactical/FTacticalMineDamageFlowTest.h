/**
 * @file FTacticalMineDamageFlowTest.h
 * @brief Header file for the FTacticalMineDamageFlowTest class
 */

#ifndef FTacticalMineDamageFlowTest_H_
#define FTacticalMineDamageFlowTest_H_

#include <cppunit/extensions/HelperMacros.h>
#include <string>

namespace FrontierTests {

class FTacticalMineDamageFlowTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalMineDamageFlowTest );
	CPPUNIT_TEST( testApplyMineDamageDefersDestroyedShipCleanupUntilAfterImmediateSummaryDialog );
	CPPUNIT_TEST( testFinalizeMoveAppliesMineDamageExactlyOncePerMovementResolution );
	CPPUNIT_TEST( testMineDamageAttackReportBuilderLeavesNestedEventsUnattachedUntilBattleScreenStoresThem );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);
	std::string extractFunctionBody(const std::string & source, const std::string & signature);
	unsigned int countOccurrences(const std::string & haystack, const std::string & needle);

public:
	void setUp();
	void tearDown();

	void testApplyMineDamageDefersDestroyedShipCleanupUntilAfterImmediateSummaryDialog();
	void testFinalizeMoveAppliesMineDamageExactlyOncePerMovementResolution();
	void testMineDamageAttackReportBuilderLeavesNestedEventsUnattachedUntilBattleScreenStoresThem();
};

}

#endif
