CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX -DTACTICAL_TEST_REPO_ROOT=\"$(CURDIR)/../..\"

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

OBJS = FTacticalCombatReportTest.o FTacticalAttackResultTest.o FTacticalDamageSummaryGUITest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o TacticalTests.o

TARGET = libtacticalTests.a
RUNNER = TacticalTests

all: $(TARGET) $(RUNNER)

$(TARGET): FTacticalCombatReportTest.o FTacticalAttackResultTest.o FTacticalDamageSummaryGUITest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o
	ar rcsu $(TARGET) FTacticalCombatReportTest.o FTacticalAttackResultTest.o FTacticalDamageSummaryGUITest.o FTacticalBattleScreenReportContextTest.o FTacticalBattleScreenElectricalFireTest.o

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
