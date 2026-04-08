/**
 * @file FTacticalDamageSummaryGUITest.h
 * @brief Header file for the FTacticalDamageSummaryGUITest class
 */

#ifndef FTacticalDamageSummaryGUITest_H_
#define FTacticalDamageSummaryGUITest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

class FTacticalDamageSummaryGUITest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalDamageSummaryGUITest );
	CPPUNIT_TEST( testReportTypeLabelsAndDialogTitleMapToReportContext );
	CPPUNIT_TEST( testDamageSummaryDialogBuildsShipRollupAndOptionalHitDetailSections );
	CPPUNIT_TEST( testBattleScreenEntryPointAndGuiBuildWiringArePresent );
	CPPUNIT_TEST_SUITE_END();

private:
	std::string readFile(const std::string & path);

public:
	void setUp();
	void tearDown();

	void testReportTypeLabelsAndDialogTitleMapToReportContext();
	void testDamageSummaryDialogBuildsShipRollupAndOptionalHitDetailSections();
	void testBattleScreenEntryPointAndGuiBuildWiringArePresent();
};

}

#endif
