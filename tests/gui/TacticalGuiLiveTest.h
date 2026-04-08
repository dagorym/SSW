/**
 * @file TacticalGuiLiveTest.h
 * @brief Header file for live tactical GUI regression coverage
 */

#ifndef _TACTICALGUILIVETEST_H_
#define _TACTICALGUILIVETEST_H_

#include <cppunit/extensions/HelperMacros.h>

#include "WXGuiTestHarness.h"

namespace FrontierTests {

class TacticalGuiLiveTest : public CppUnit::TestFixture {
CPPUNIT_TEST_SUITE( TacticalGuiLiveTest );
CPPUNIT_TEST( testWXTacticalUIParentBackedModalAndRedrawPaths );
CPPUNIT_TEST( testTacticalActionButtonsRemainSizerPositionedWhenShown );
CPPUNIT_TEST( testTacticalActionButtonsStayBelowPromptReservationAcrossPhases );
CPPUNIT_TEST( testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior );
CPPUNIT_TEST( testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo );
CPPUNIT_TEST_SUITE_END();

private:
WXGuiTestHarness m_harness;

public:
void setUp();
void tearDown();

void testWXTacticalUIParentBackedModalAndRedrawPaths();
void testTacticalActionButtonsRemainSizerPositionedWhenShown();
void testTacticalActionButtonsStayBelowPromptReservationAcrossPhases();
void testTacticalDamageSummaryDialogDisplaysContextAndCloseBehavior();
void testICMSelectionDialogInteractionFinalizesAssignedCountsAndAmmo();
};

}

#endif
