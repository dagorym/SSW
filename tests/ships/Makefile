#CXXFLAGS = -I../include -I/usr/local/lib/wx/include/gtk2-ansi-release-2.8 \
#            -I/usr/local/include/wx-2.8 \
#            -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -g 

#LIBS = -L/usr/local/lib -pthread -lwx_gtk2_richtext-2.8 -lwx_gtk2_aui-2.8 -lwx_gtk2_xrc-2.8 \
#       -lwx_gtk2_qa-2.8 -lwx_gtk2_html-2.8 -lwx_gtk2_adv-2.8 -lwx_gtk2_core-2.8 \
#       -lwx_base_xml-2.8 -lwx_base_net-2.8 -lwx_base-2.8

CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX

LIBS = `wx-config --libs`

PICFLAGS = -fPIC -DPIC

#CXXFLAGS = -I../include -I/home/tstephen/src/wxGTK-2.8.7/buildgtkm/lib/wx/include/gtk2-ansi-release-static-2.8 -I/home/tstephen/src/wxGTK-2.8.7/include -I/home/tstephen/src/wxGTK-2.8.7/contrib/include -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -g
#LIBS = -L/home/tstephen/src/wxGTK-2.8.7/buildgtkm/lib -pthread   /home/tstephen/src/wxGTK-2.8.7/buildgtkm/lib/libwx_gtk2-2.8.a -pthread -L/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lgthread-2.0 -lglib-2.0 -lXinerama -lXxf86vm -lSM -lwxregex-2.8 -lwxexpat-2.8 -lwxtiff-2.8 -lwxjpeg-2.8 -lwxpng-2.8 -lwxzlib-2.8 -ldl -lm

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

OBJS = FVehicleTest.o FArmedStationTest.o FAssaultCarrierTest.o \
       FAssaultScoutTest.o FBattleshipTest.o FDestroyerTest.o \
       FFighterTest.o FFortifiedStationTest.o FFortressTest.o \
       FFrigateTest.o FHvCruiserTest.o FLtCruiserTest.o \
       FMinelayerTest.o
       
TARGET = libshipsTests.a

all $(TARGET): $(OBJS)
	ar rcsu $(TARGET) $(OBJS)

#	$(CXX) $(CXXFLAGS) -shared -o $(TARGET) $(OBJS)# $(LIBS)
	
objs: $(OBJS)
	rm *.gcda
	rm *.gcov
	rm *.gcno

.PHONY:
clean:
	rm $(OBJS)
	rm $(TARGET)


# DO NOT DELETE

FArmedStationTest.o: FArmedStationTest.h ../../include/ships/FArmedStation.h
FArmedStationTest.o: ../../include/ships/FVehicle.h
FArmedStationTest.o: ../../include/core/FPObject.h
FArmedStationTest.o: ../../include/core/FObject.h
FArmedStationTest.o: ../../include/weapons/FWeapon.h
FAssaultCarrierTest.o: FAssaultCarrierTest.h
FAssaultCarrierTest.o: ../../include/ships/FAssaultCarrier.h
FAssaultCarrierTest.o: ../../include/ships/FVehicle.h
FAssaultCarrierTest.o: ../../include/core/FPObject.h
FAssaultCarrierTest.o: ../../include/core/FObject.h
FAssaultCarrierTest.o: ../../include/weapons/FWeapon.h
FAssaultScoutTest.o: FAssaultScoutTest.h ../../include/ships/FAssaultScout.h
FAssaultScoutTest.o: ../../include/ships/FVehicle.h
FAssaultScoutTest.o: ../../include/core/FPObject.h
FAssaultScoutTest.o: ../../include/core/FObject.h
FAssaultScoutTest.o: ../../include/weapons/FWeapon.h
FBattleshipTest.o: FBattleshipTest.h ../../include/ships/FBattleship.h
FBattleshipTest.o: ../../include/ships/FVehicle.h
FBattleshipTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FBattleshipTest.o: ../../include/weapons/FWeapon.h
FDestroyerTest.o: FDestroyerTest.h ../../include/ships/FDestroyer.h
FDestroyerTest.o: ../../include/ships/FVehicle.h
FDestroyerTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FDestroyerTest.o: ../../include/weapons/FWeapon.h
FFighterTest.o: FFighterTest.h ../../include/ships/FFighter.h
FFighterTest.o: ../../include/ships/FVehicle.h ../../include/core/FPObject.h
FFighterTest.o: ../../include/core/FObject.h ../../include/weapons/FWeapon.h
FFortifiedStationTest.o: FFortifiedStationTest.h
FFortifiedStationTest.o: ../../include/ships/FFortifiedStation.h
FFortifiedStationTest.o: ../../include/ships/FVehicle.h
FFortifiedStationTest.o: ../../include/core/FPObject.h
FFortifiedStationTest.o: ../../include/core/FObject.h
FFortifiedStationTest.o: ../../include/weapons/FWeapon.h
FFortressTest.o: FFortressTest.h ../../include/ships/FFortress.h
FFortressTest.o: ../../include/ships/FVehicle.h ../../include/core/FPObject.h
FFortressTest.o: ../../include/core/FObject.h ../../include/weapons/FWeapon.h
FFrigateTest.o: FFrigateTest.h ../../include/ships/FFrigate.h
FFrigateTest.o: ../../include/ships/FVehicle.h ../../include/core/FPObject.h
FFrigateTest.o: ../../include/core/FObject.h ../../include/weapons/FWeapon.h
FHvCruiserTest.o: FHvCruiserTest.h ../../include/ships/FHvCruiser.h
FHvCruiserTest.o: ../../include/ships/FVehicle.h
FHvCruiserTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FHvCruiserTest.o: ../../include/weapons/FWeapon.h
FLtCruiserTest.o: FLtCruiserTest.h ../../include/ships/FLtCruiser.h
FLtCruiserTest.o: ../../include/ships/FVehicle.h
FLtCruiserTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FLtCruiserTest.o: ../../include/weapons/FWeapon.h
FMinelayerTest.o: FMinelayerTest.h ../../include/ships/FMinelayer.h
FMinelayerTest.o: ../../include/ships/FVehicle.h
FMinelayerTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FMinelayerTest.o: ../../include/weapons/FWeapon.h
FVehicleTest.o: FVehicleTest.h ../../include/ships/FVehicle.h
FVehicleTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FVehicleTest.o: ../../include/weapons/FWeapon.h
