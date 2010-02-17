#CXXFLAGS = -I../include -I/usr/local/lib/wx/include/gtk2-ansi-release-2.8 \
#            -I/usr/local/include/wx-2.8 \
#            -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -g 

#LIBS = -L/usr/local/lib -pthread -lwx_gtk2_richtext-2.8 -lwx_gtk2_aui-2.8 -lwx_gtk2_xrc-2.8 \
#       -lwx_gtk2_qa-2.8 -lwx_gtk2_html-2.8 -lwx_gtk2_adv-2.8 -lwx_gtk2_core-2.8 \
#       -lwx_base_xml-2.8 -lwx_base_net-2.8 -lwx_base-2.8

CXXFLAGS = -I../../include -g `wx-config --cxxflags` -Wall -Woverloaded-virtual -DLINUX -fprofile-arcs -ftest-coverage

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

OBJS = FWeapon.o \
       FLaserBattery.o FLaserCannon.o FProtonBattery.o FElectronBattery.o \
       FDisruptorCannon.o FRocketBattery.o FAssaultRocket.o FTorpedo.o \
       FSeekerMissileLauncher.o FMineLauncher.o

TARGET = libweapons.a

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

FAssaultRocket.o: ../../include/weapons/FAssaultRocket.h
FAssaultRocket.o: ../../include/weapons/FWeapon.h
FAssaultRocket.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FDisruptorCannon.o: ../../include/weapons/FDisruptorCannon.h
FDisruptorCannon.o: ../../include/weapons/FWeapon.h
FDisruptorCannon.o: ../../include/core/FPObject.h
FDisruptorCannon.o: ../../include/core/FObject.h
FElectronBattery.o: ../../include/weapons/FElectronBattery.h
FElectronBattery.o: ../../include/weapons/FWeapon.h
FElectronBattery.o: ../../include/core/FPObject.h
FElectronBattery.o: ../../include/core/FObject.h
FLaserBattery.o: ../../include/weapons/FLaserBattery.h
FLaserBattery.o: ../../include/weapons/FWeapon.h
FLaserBattery.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FLaserCannon.o: ../../include/weapons/FLaserCannon.h
FLaserCannon.o: ../../include/weapons/FWeapon.h ../../include/core/FPObject.h
FLaserCannon.o: ../../include/core/FObject.h
FMineLauncher.o: ../../include/weapons/FMineLauncher.h
FMineLauncher.o: ../../include/weapons/FWeapon.h
FMineLauncher.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FProtonBattery.o: ../../include/weapons/FProtonBattery.h
FProtonBattery.o: ../../include/weapons/FWeapon.h
FProtonBattery.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FRocketBattery.o: ../../include/weapons/FRocketBattery.h
FRocketBattery.o: ../../include/weapons/FWeapon.h
FRocketBattery.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FSeekerMissileLauncher.o: ../../include/weapons/FSeekerMissileLauncher.h
FSeekerMissileLauncher.o: ../../include/weapons/FWeapon.h
FSeekerMissileLauncher.o: ../../include/core/FPObject.h
FSeekerMissileLauncher.o: ../../include/core/FObject.h
FTorpedo.o: ../../include/weapons/FTorpedo.h ../../include/weapons/FWeapon.h
FTorpedo.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FWeapon.o: ../../include/weapons/FWeapon.h ../../include/core/FPObject.h
FWeapon.o: ../../include/core/FObject.h ../../include/weapons/weapons.h
FWeapon.o: ../../include/weapons/FLaserBattery.h
FWeapon.o: ../../include/weapons/FLaserCannon.h
FWeapon.o: ../../include/weapons/FElectronBattery.h
FWeapon.o: ../../include/weapons/FProtonBattery.h
FWeapon.o: ../../include/weapons/FRocketBattery.h
FWeapon.o: ../../include/weapons/FDisruptorCannon.h
FWeapon.o: ../../include/weapons/FTorpedo.h
FWeapon.o: ../../include/weapons/FAssaultRocket.h
FWeapon.o: ../../include/weapons/FSeekerMissileLauncher.h
FWeapon.o: ../../include/weapons/FMineLauncher.h
FWeapon.o: ../../include/ships/FVehicle.h ../../include/core/FGameConfig.h
FWeapon.o: ../../include/defenses/FDefense.h ../../include/Frontier.h
FWeapon.o: ../../include/wxWidgets.h
