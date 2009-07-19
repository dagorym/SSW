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

OBJS = FWeaponTest.o FAssaultRocketTest.o FDisruptorCannonTest.o \
       FElectronBatteryTest.o FLaserBatteryTest.o FLaserCannonTest.o \
       FMineLauncherTest.o FProtonBatteryTest.o FRocketBatteryTest.o \
       FSeekerMissileLauncherTest.o FTorpedoTest.o #AllWeaponTest.o 

TARGET = libweaponsTests.a

all $(TARGET): $(OBJS)
	ar rcsu $(TARGET) $(OBJS)

objs: $(OBJS)
	rm *.gcda
	rm *.gcov
	rm *.gcno

.PHONY:
clean:
	rm $(OBJS)
	rm $(TARGET)


# DO NOT DELETE

FAssaultRocketTest.o: FAssaultRocketTest.h
FAssaultRocketTest.o: ../../include/weapons/FAssaultRocket.h
FAssaultRocketTest.o: ../../include/weapons/FWeapon.h
FAssaultRocketTest.o: ../../include/core/FPObject.h
FAssaultRocketTest.o: ../../include/core/FObject.h
FDisruptorCannonTest.o: FDisruptorCannonTest.h
FDisruptorCannonTest.o: ../../include/weapons/FDisruptorCannon.h
FDisruptorCannonTest.o: ../../include/weapons/FWeapon.h
FDisruptorCannonTest.o: ../../include/core/FPObject.h
FDisruptorCannonTest.o: ../../include/core/FObject.h
FDisruptorCannonTest.o: ../../include/ships/FVehicle.h
FElectronBatteryTest.o: FElectronBatteryTest.h
FElectronBatteryTest.o: ../../include/weapons/FElectronBattery.h
FElectronBatteryTest.o: ../../include/weapons/FWeapon.h
FElectronBatteryTest.o: ../../include/core/FPObject.h
FElectronBatteryTest.o: ../../include/core/FObject.h
FLaserBatteryTest.o: FLaserBatteryTest.h
FLaserBatteryTest.o: ../../include/weapons/FLaserBattery.h
FLaserBatteryTest.o: ../../include/weapons/FWeapon.h
FLaserBatteryTest.o: ../../include/core/FPObject.h
FLaserBatteryTest.o: ../../include/core/FObject.h
FLaserCannonTest.o: FLaserCannonTest.h ../../include/weapons/FLaserCannon.h
FLaserCannonTest.o: ../../include/weapons/FWeapon.h
FLaserCannonTest.o: ../../include/core/FPObject.h
FLaserCannonTest.o: ../../include/core/FObject.h
FMineLauncherTest.o: FMineLauncherTest.h
FMineLauncherTest.o: ../../include/weapons/FMineLauncher.h
FMineLauncherTest.o: ../../include/weapons/FWeapon.h
FMineLauncherTest.o: ../../include/core/FPObject.h
FMineLauncherTest.o: ../../include/core/FObject.h
FProtonBatteryTest.o: FProtonBatteryTest.h
FProtonBatteryTest.o: ../../include/weapons/FProtonBattery.h
FProtonBatteryTest.o: ../../include/weapons/FWeapon.h
FProtonBatteryTest.o: ../../include/core/FPObject.h
FProtonBatteryTest.o: ../../include/core/FObject.h
FRocketBatteryTest.o: FRocketBatteryTest.h
FRocketBatteryTest.o: ../../include/weapons/FRocketBattery.h
FRocketBatteryTest.o: ../../include/weapons/FWeapon.h
FRocketBatteryTest.o: ../../include/core/FPObject.h
FRocketBatteryTest.o: ../../include/core/FObject.h
FSeekerMissileLauncherTest.o: FSeekerMissileLauncherTest.h
FSeekerMissileLauncherTest.o: ../../include/weapons/FSeekerMissileLauncher.h
FSeekerMissileLauncherTest.o: ../../include/weapons/FWeapon.h
FSeekerMissileLauncherTest.o: ../../include/core/FPObject.h
FSeekerMissileLauncherTest.o: ../../include/core/FObject.h
FTorpedoTest.o: FTorpedoTest.h ../../include/weapons/FTorpedo.h
FTorpedoTest.o: ../../include/weapons/FWeapon.h ../../include/core/FPObject.h
FTorpedoTest.o: ../../include/core/FObject.h
FWeaponTest.o: FWeaponTest.h ../../include/weapons/FWeapon.h
FWeaponTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FWeaponTest.o: ../../include/ships/FVehicle.h
