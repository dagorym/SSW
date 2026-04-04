/**
 * @file GuiTests.cpp
 * @brief Main program for GUI unit tests
 */

#include <cppunit/ui/text/TestRunner.h>
#include "GuiHarnessTest.h"
#include "StrategicGuiLiveTest.h"
#include "TacticalGuiLiveTest.h"

using namespace FrontierTests;

int main(int argc, char **argv) {
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(GuiHarnessTest::suite());
	runner.addTest(StrategicGuiLiveTest::suite());
	runner.addTest(TacticalGuiLiveTest::suite());
	const bool wasSuccessful = runner.run("", false);
	return wasSuccessful ? 0 : 1;
}
