#include "FGameHeaderDependencyTest.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace FrontierTests {

CPPUNIT_TEST_SUITE_REGISTRATION(FGameHeaderDependencyTest);

static std::string readFirstAvailableFile(const std::vector<std::string> &paths) {
	for (std::vector<std::string>::const_iterator it = paths.begin(); it != paths.end(); ++it) {
		std::ifstream in(it->c_str());
		if (in.good()) {
			std::ostringstream buffer;
			buffer << in.rdbuf();
			return buffer.str();
		}
	}
	CPPUNIT_FAIL("Expected at least one candidate file path to be readable");
	return "";
}

void FGameHeaderDependencyTest::testHeaderDoesNotIncludeWxHeaders() {
	const std::string header = readFirstAvailableFile(std::vector<std::string>{
		"../../include/strategic/FGame.h",
		"../include/strategic/FGame.h"
	});
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("#include <wx/"));
}

void FGameHeaderDependencyTest::testHeaderDoesNotExposeWxRenderOrMouseTypes() {
	const std::string header = readFirstAvailableFile(std::vector<std::string>{
		"../../include/strategic/FGame.h",
		"../include/strategic/FGame.h"
	});

	CPPUNIT_ASSERT(header.find("class wxWindow;") != std::string::npos);
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("class wxDC;"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("class wxMouseEvent;"));
	CPPUNIT_ASSERT(header.find("static FGame & create();") != std::string::npos);
	CPPUNIT_ASSERT(header.find("static FGame & create(IStrategicUI * ui);") != std::string::npos);
	CPPUNIT_ASSERT(header.find("static FGame & create(wxWindow * win);") != std::string::npos);
	CPPUNIT_ASSERT(header.find("int init(wxWindow *w);") != std::string::npos);
}

void FGameHeaderDependencyTest::testHeaderUsesStrategicUIAndRemovesParentMember() {
	const std::string header = readFirstAvailableFile(std::vector<std::string>{
		"../../include/strategic/FGame.h",
		"../include/strategic/FGame.h"
	});

	CPPUNIT_ASSERT(header.find("#include \"strategic/IStrategicUI.h\"") != std::string::npos);
	CPPUNIT_ASSERT(header.find("IStrategicUI * m_ui;") != std::string::npos);
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("wxWindow * m_parent;"));
}

void FGameHeaderDependencyTest::testHeaderDoesNotDeclareStrategicDrawOrMouseHandlers() {
	const std::string header = readFirstAvailableFile(std::vector<std::string>{
		"../../include/strategic/FGame.h",
		"../include/strategic/FGame.h"
	});

	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("draw("));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("drawTurnCounter("));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("onLeftDClick("));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("onLeftUp("));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("m_tenday"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, header.find("m_day"));
}

void FGameHeaderDependencyTest::testSourceDoesNotUseDisplayPaintingHelpers() {
	const std::string source = readFirstAvailableFile(std::vector<std::string>{
		"../../src/strategic/FGame.cpp",
		"../src/strategic/FGame.cpp"
	});

	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("WXMapDisplay"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("WXPlayerDisplay"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("WXGameDisplay"));
}

void FGameHeaderDependencyTest::testSourceDoesNotImplementStrategicDrawOrMouseHandlers() {
	const std::string source = readFirstAvailableFile(std::vector<std::string>{
		"../../src/strategic/FGame.cpp",
		"../src/strategic/FGame.cpp"
	});

	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("FGame::draw("));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("FGame::drawTurnCounter("));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("FGame::onLeftDClick("));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("FGame::onLeftUp("));
}

void FGameHeaderDependencyTest::testWXGameDisplayOwnsStrategicDrawEntryPoint() {
	const std::string displayHeader = readFirstAvailableFile(std::vector<std::string>{
		"../../include/gui/WXGameDisplay.h",
		"../include/gui/WXGameDisplay.h"
	});
	const std::string displaySource = readFirstAvailableFile(std::vector<std::string>{
		"../../src/gui/WXGameDisplay.cpp",
		"../src/gui/WXGameDisplay.cpp"
	});

	CPPUNIT_ASSERT(displayHeader.find("void draw(wxDC &dc, FGame &game);") != std::string::npos);
	CPPUNIT_ASSERT(displaySource.find("void WXGameDisplay::draw(wxDC &dc, FGame &game)") != std::string::npos);
}

void FGameHeaderDependencyTest::testSourceRemovesDirectStrategicDialogIncludes() {
	const std::string source = readFirstAvailableFile(std::vector<std::string>{
		"../../src/strategic/FGame.cpp",
		"../src/strategic/FGame.cpp"
	});

	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("SystemDialogGUI.h"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("UPFUnattachedGUI.h"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("SatharFleetsGUI.h"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("ViewFleetGUI.h"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("SatharRetreatGUI.h"));
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("SelectCombatGUI.h"));
}

void FGameHeaderDependencyTest::testSourceRoutesStrategicInteractionsThroughInterface() {
	const std::string source = readFirstAvailableFile(std::vector<std::string>{
		"../../src/strategic/FGame.cpp",
		"../src/strategic/FGame.cpp"
	});

	CPPUNIT_ASSERT(source.find("m_ui->showMessage(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->runUPFUnattachedSetup(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->runSatharFleetSetup(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->showSystemDialog(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->showFleetDialog(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->showRetreatConditions(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->selectCombat(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->notifyVictory(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->notifyFailedJump(") != std::string::npos);
	CPPUNIT_ASSERT(source.find("m_ui->requestRedraw(") != std::string::npos);
	CPPUNIT_ASSERT_EQUAL(std::string::npos, source.find("wxMessageBox("));
}

}
