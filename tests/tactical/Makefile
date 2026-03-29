ifeq ($(COVERAGE),1)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
else
COVERAGE_FLAGS =
endif

CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX $(COVERAGE_FLAGS) -DTACTICAL_TEST_REPO_ROOT=\"$(CURDIR)/../..\"

LIBS = `wx-config --libs`

PICFLAGS = -fPIC -DPIC

.SUFFIXES: .o .c .cpp .cxx .rsrc .r

.c.o:
	$(CC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.rsrc.r:
	$(DEREZ) $^ Carbon.r -useDF > $@

OBJS = FTacticalCombatReportTest.o FTacticalAttackResultTest.o FTacticalDamageSummaryGUITest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o FTacticalMineDamageReportSummaryTest.o FTacticalMineDamageFlowTest.o FTacticalBattleDisplayFireFlowTest.o FTacticalBatteryRangeClampTest.o FTacticalStationOrbitalMovementTest.o FTacticalGameHeaderTest.o TacticalTests.o

TARGET = libtacticalTests.a
RUNNER = TacticalTests

all: $(TARGET) $(RUNNER)

$(TARGET): FTacticalCombatReportTest.o FTacticalAttackResultTest.o FTacticalDamageSummaryGUITest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o FTacticalMineDamageReportSummaryTest.o FTacticalMineDamageFlowTest.o FTacticalBattleDisplayFireFlowTest.o FTacticalBatteryRangeClampTest.o FTacticalStationOrbitalMovementTest.o FTacticalGameHeaderTest.o
	ar rcs $(TARGET) FTacticalCombatReportTest.o FTacticalAttackResultTest.o FTacticalDamageSummaryGUITest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o FTacticalMineDamageReportSummaryTest.o FTacticalMineDamageFlowTest.o FTacticalBattleDisplayFireFlowTest.o FTacticalBatteryRangeClampTest.o FTacticalStationOrbitalMovementTest.o FTacticalGameHeaderTest.o

$(RUNNER): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(RUNNER) $(OBJS) -lcppunit $(LIBS)

objs: $(OBJS)
	rm *.gcda
	rm *.gcov
	rm *.gcno

.PHONY:
clean:
	rm $(OBJS)
	rm $(TARGET)
	rm $(RUNNER)
