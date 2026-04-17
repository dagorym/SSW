/**
 * @file GuiTests.cpp
 * @brief Main program for GUI unit tests
 */

#include <cppunit/ui/text/TestRunner.h>
#include "BattleSimGuiLiveTest.h"
#include "GuiHarnessTest.h"
#include "StrategicGuiLiveTest.h"
#include "TacticalGuiLiveTest.h"
#include "WXGuiTestHarness.h"

using namespace FrontierTests;

int main(int argc, char **argv) {
	WXGuiTestHarness harness;
	if (!harness.bootstrap()) {
		return 1;
	}

	CppUnit::TextUi::TestRunner runner;
	runner.addTest(GuiHarnessTest::suite());
	runner.addTest(StrategicGuiLiveTest::suite());
	runner.addTest(TacticalGuiLiveTest::suite());
	runner.addTest(BattleSimGuiLiveTest::suite());
	const bool wasSuccessful = runner.run("", false);
	harness.cleanupOrphanTopLevels(10);
	harness.shutdown();
	return wasSuccessful ? 0 : 1;
}
