/**
 * @file WXTacticalUIAdapterTest.cpp
 * @brief Implementation file for WXTacticalUI adapter acceptance tests
 */

#include "WXTacticalUIAdapterTest.h"

#include <vector>

#include "gui/WXTacticalUI.h"
#include "tactical/ITacticalUI.h"

namespace FrontierTests {
using namespace Frontier;


CPPUNIT_TEST_SUITE_REGISTRATION( WXTacticalUIAdapterTest );

void WXTacticalUIAdapterTest::setUp() {
}

void WXTacticalUIAdapterTest::tearDown() {
}

void WXTacticalUIAdapterTest::testAdapterConstructsAsITacticalUI() {
	WXTacticalUI ui(NULL);
	ITacticalUI * interfaceView = &ui;
	CPPUNIT_ASSERT(interfaceView != NULL);
}

void WXTacticalUIAdapterTest::testRequestRedrawIsSafeWithAndWithoutParent() {
	WXTacticalUI nullParentUi(NULL);
	nullParentUi.requestRedraw();
}

void WXTacticalUIAdapterTest::testRunICMSelectionValidatesInputs() {
	WXTacticalUI ui(NULL);

	std::vector<ICMData *> noRows;
	CPPUNIT_ASSERT_EQUAL(1, ui.runICMSelection(noRows, NULL));

	VehicleList emptyDefenders;
	CPPUNIT_ASSERT_EQUAL(1, ui.runICMSelection(noRows, &emptyDefenders));
}

void WXTacticalUIAdapterTest::testNotifyWinnerUsesNoParentFallbackSafely() {
	WXTacticalUI ui(NULL);
	ui.showMessage("Tactical Adapter", "No parent fallback path");
	ui.notifyWinner(true);
	ui.notifyWinner(false);
}

} // namespace FrontierTests
