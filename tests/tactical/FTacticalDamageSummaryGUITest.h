/**
 * @file FTacticalDamageSummaryGUITest.h
 * @brief Header file for the FTacticalDamageSummaryGUITest class
 */

#ifndef FTacticalDamageSummaryGUITest_H_
#define FTacticalDamageSummaryGUITest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalDamageSummaryGUITest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 22, 2026
 * @date Last Modified: Apr 08, 2026
 */
class FTacticalDamageSummaryGUITest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalDamageSummaryGUITest );
	CPPUNIT_TEST( testReportTypeLabelsAndDialogTitleMapToReportContext );
	CPPUNIT_TEST( testDamageSummaryDialogBuildsShipRollupAndOptionalHitDetailSections );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	/**
	 * @brief Verifies report Type Labels And Dialog Title Map To Report Context.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testReportTypeLabelsAndDialogTitleMapToReportContext();
	/**
	 * @brief Verifies damage Summary Dialog Builds Ship Rollup And Optional Hit Detail Sections.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Apr 08, 2026
	 * @date Last Modified: Apr 08, 2026
	 */
	void testDamageSummaryDialogBuildsShipRollupAndOptionalHitDetailSections();
};

}

#endif
