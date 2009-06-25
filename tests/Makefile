CXXFLAGS = -I../include  -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage

LIBS = `wx-config --libs` -L./strategic -lstrategicTests -L./ships -lshipsTests \
       -L./weapons -lweaponsTests -L./core -lcoreTests \
       -lcppunit -L../src/strategic -lstrategic -L../src/gui -lgui -L../src/ships -lships \
       -L../src/tactical -ltactical -L../src/weapons -lweapons -L../src/core -lcore 
       

#################### the implicit rules #############################

# NB: we use the old style suffix rules (.c.o) and not the GNU make
#     implicit rules (%.o: %.c) because they are more portable, in
#     particular the BSD make understands the former but not the
#     latter
.SUFFIXES: .o .c .cpp .cxx .rsrc .r

.c.o:
	$(CC) -c $(CFLAGS) $(PICFLAGS) -o $@ $<

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(PICFLAGS) -o $@ $<

.rsrc.r:
	$(DEREZ) $^ Carbon.r -useDF > $@

############################# Paths #################################

OBJS = SSWTests.o #FApp.o Frontier.o FMainFrameTest.o 

OBJ_LIBS = libcoreTests.a libweaponsTests.a libstrategicTests.a  libshipsTests.a #libguiTests.a libtacticalTests.a

TARGET = SSWTests

all: $(OBJ_LIBS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

libcoreTests.a:
	@ cd core; $(MAKE) $@
	
libguiTests.a:
	@ cd gui; $(MAKE) $@
	
libshipsTests.a:
	@ cd ships; $(MAKE) $@
	
libstrategicTests.a:
	@ cd strategic; $(MAKE) $@
	
libtacticalTests.a:
	@ cd tactical; $(MAKE) $@
	
libweaponsTests.a:
	@ cd weapons; $(MAKE) $@
	
objs: $(OBJS)
	rm *.gcda
	rm *.gcov
	rm *.gcno

.PHONY:
all_clean:
	@ cd core; ${MAKE} clean
#	@ cd gui; ${MAKE} clean
	@ cd ships; ${MAKE} clean
	@ cd strategic; ${MAKE} clean
#	@ cd tactical; ${MAKE} clean
	@ cd weapons; ${MAKE} clean
	rm $(OBJS)
	rm $(TARGET)
	
clean:
	rm $(OBJS)
	rm $(TARGET)


# DO NOT DELETE

SSWTests.o: core/FPointTest.h ../include/core/FPoint.h core/FObjectTest.h
SSWTests.o: ../include/core/FObject.h weapons/AllWeaponTest.h
SSWTests.o: weapons/FWeaponTest.h ../include/weapons/FWeapon.h
SSWTests.o: ../include/core/FPObject.h ../include/core/FObject.h
SSWTests.o: weapons/FAssaultRocketTest.h ../include/weapons/FAssaultRocket.h
SSWTests.o: weapons/FDisruptorCannonTest.h
SSWTests.o: ../include/weapons/FDisruptorCannon.h
SSWTests.o: weapons/FElectronBatteryTest.h
SSWTests.o: ../include/weapons/FElectronBattery.h weapons/FLaserBatteryTest.h
SSWTests.o: ../include/weapons/FLaserBattery.h weapons/FLaserCannonTest.h
SSWTests.o: ../include/weapons/FLaserCannon.h weapons/FMineLauncherTest.h
SSWTests.o: ../include/weapons/FMineLauncher.h weapons/FProtonBatteryTest.h
SSWTests.o: ../include/weapons/FProtonBattery.h weapons/FRocketBatteryTest.h
SSWTests.o: ../include/weapons/FRocketBattery.h
SSWTests.o: weapons/FSeekerMissileLauncherTest.h
SSWTests.o: ../include/weapons/FSeekerMissileLauncher.h
SSWTests.o: weapons/FTorpedoTest.h ../include/weapons/FTorpedo.h
SSWTests.o: ships/FVehicleTest.h ../include/ships/FVehicle.h
SSWTests.o: ships/FAssaultScoutTest.h ../include/ships/FAssaultScout.h
SSWTests.o: ../include/ships/FVehicle.h ships/FArmedStationTest.h
SSWTests.o: ../include/ships/FArmedStation.h ships/FAssaultCarrierTest.h
SSWTests.o: ../include/ships/FAssaultCarrier.h ships/FBattleshipTest.h
SSWTests.o: ../include/ships/FBattleship.h ships/FDestroyerTest.h
SSWTests.o: ../include/ships/FDestroyer.h ships/FFighterTest.h
SSWTests.o: ../include/ships/FFighter.h ships/FFortifiedStationTest.h
SSWTests.o: ../include/ships/FFortifiedStation.h ships/FFortressTest.h
SSWTests.o: ../include/ships/FFortress.h ships/FFrigateTest.h
SSWTests.o: ../include/ships/FFrigate.h ships/FHvCruiserTest.h
SSWTests.o: ../include/ships/FHvCruiser.h ships/FLtCruiserTest.h
SSWTests.o: ../include/ships/FLtCruiser.h ships/FMinelayerTest.h
SSWTests.o: ../include/ships/FMinelayer.h strategic/FGameTest.h
SSWTests.o: strategic/FPlanetTest.h ../include/strategic/FPlanet.h
SSWTests.o: ../include/strategic/FAstroBody.h strategic/FFleetTest.h
SSWTests.o: ../include/strategic/FFleet.h
