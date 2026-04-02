.RECIPEPREFIX = >
ifeq ($(COVERAGE),1)
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
else
COVERAGE_FLAGS =
endif

CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX $(COVERAGE_FLAGS) -DTACTICAL_TEST_REPO_ROOT=\"$(CURDIR)/../..\"

LIBS = `wx-config --libs`
MODEL_LIBS = -L../../src/tactical -L../../src/weapons -L../../src/defenses -L../../src/ships -L../../src/strategic -L../../src/core -Wl,--start-group -ltactical -lweapons -ldefenses -lships -lstrategic -lcore -Wl,--end-group

PICFLAGS = -fPIC -DPIC

.SUFFIXES: .o .c .cpp .cxx .rsrc .r

.c.o:
>$(CC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o:
>$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o:
>$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.rsrc.r:
>$(DEREZ) $^ Carbon.r -useDF > $@

OBJS = FTacticalAttackResultTest.o FTacticalBattleScreenDelegationTest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o FTacticalMineDamageReportSummaryTest.o FTacticalMineDamageFlowTest.o FTacticalBattleDisplayFireFlowTest.o FTacticalBatteryRangeClampTest.o FTacticalBattleBoardRendererDelegationTest.o FTacticalStationOrbitalMovementTest.o FTacticalGameHeaderTest.o FTacticalGameMechanicsTest.o FTacticalModelSelectionHexClickSurfaceTest.o ITacticalUIBoundaryTest.o TacticalTests.o

TARGET = libtacticalTests.a
RUNNER = TacticalTests

all: model-deps $(TARGET) $(RUNNER)

model-deps:
>$(MAKE) -C ../../src/core
>$(MAKE) -C ../../src/strategic
>$(MAKE) -C ../../src/ships
>$(MAKE) -C ../../src/defenses
>$(MAKE) -C ../../src/weapons
>$(MAKE) -C ../../src/tactical

$(TARGET): FTacticalAttackResultTest.o FTacticalBattleScreenDelegationTest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o FTacticalMineDamageReportSummaryTest.o FTacticalMineDamageFlowTest.o FTacticalBattleDisplayFireFlowTest.o FTacticalBatteryRangeClampTest.o FTacticalBattleBoardRendererDelegationTest.o FTacticalStationOrbitalMovementTest.o FTacticalGameHeaderTest.o FTacticalGameMechanicsTest.o FTacticalModelSelectionHexClickSurfaceTest.o ITacticalUIBoundaryTest.o
>ar rcs $(TARGET) FTacticalAttackResultTest.o FTacticalBattleScreenDelegationTest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o FTacticalMineDamageReportSummaryTest.o FTacticalMineDamageFlowTest.o FTacticalBattleDisplayFireFlowTest.o FTacticalBatteryRangeClampTest.o FTacticalBattleBoardRendererDelegationTest.o FTacticalStationOrbitalMovementTest.o FTacticalGameHeaderTest.o FTacticalGameMechanicsTest.o FTacticalModelSelectionHexClickSurfaceTest.o ITacticalUIBoundaryTest.o

$(RUNNER): model-deps $(OBJS)
>$(CXX) $(CXXFLAGS) -o $(RUNNER) $(OBJS) -lcppunit $(MODEL_LIBS) $(LIBS)

objs: $(OBJS)
>rm -f *.gcda
>rm -f *.gcov
>rm -f *.gcno

.PHONY:
clean:
>rm -f $(OBJS)
>rm -f $(TARGET)
>rm -f $(RUNNER)
