/**
 * @file FTacticalMineDamageReportSummaryTest.h
 * @brief Header file for the FTacticalMineDamageReportSummaryTest class
 */

#ifndef FTacticalMineDamageReportSummaryTest_H_
#define FTacticalMineDamageReportSummaryTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FTacticalMineDamageReportSummaryTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalMineDamageReportSummaryTest );
	CPPUNIT_TEST( testMineDamageReportSummaryPreservesImmediateEventAndVisibleShipRollup );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testMineDamageReportSummaryPreservesImmediateEventAndVisibleShipRollup();
};

}

#endif
