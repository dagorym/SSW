/**
 * @file FTacticalMineDamageReportSummaryTest.h
 * @brief Header file for the FTacticalMineDamageReportSummaryTest class
 */

#ifndef FTacticalMineDamageReportSummaryTest_H_
#define FTacticalMineDamageReportSummaryTest_H_

#include <cppunit/extensions/HelperMacros.h>

namespace FrontierTests {

/**
 * @brief CppUnit fixture for FTacticalMineDamageReportSummaryTest tactical regression coverage.
 *
 * Exercises the tactical regression behavior covered by this fixture case.
 *
 * @author gpt-5.3-codex (medium)
 * @date Created: Mar 22, 2026
 * @date Last Modified: Mar 22, 2026
 */
class FTacticalMineDamageReportSummaryTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE( FTacticalMineDamageReportSummaryTest );
	CPPUNIT_TEST( testMineDamageReportSummaryPreservesImmediateEventAndVisibleShipRollup );
	CPPUNIT_TEST( testMineDamageSummaryKeepsNestedRawDetailWithoutDoubleCountingTargetHullDamage );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	/**
	 * @brief Verifies mine Damage Report Summary Preserves Immediate Event And Visible Ship Rollup.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testMineDamageReportSummaryPreservesImmediateEventAndVisibleShipRollup();
	/**
	 * @brief Verifies mine Damage Summary Keeps Nested Raw Detail Without Double Counting Target Hull Damage.
	 *
	 * Exercises the tactical regression behavior covered by this fixture case.
	 *
	 * @author gpt-5.3-codex (medium)
	 * @date Created: Mar 22, 2026
	 * @date Last Modified: Mar 22, 2026
	 */
	void testMineDamageSummaryKeepsNestedRawDetailWithoutDoubleCountingTargetHullDamage();
};

}

#endif
