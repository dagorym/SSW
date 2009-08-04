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

OBJS = FFleet.o \
       FPlayer.o FAstroBody.o FPlanet.o FSystem.o FJumpRoute.o FMap.o \
       FGame.o 

TARGET = libstrategic.a

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

FAstroBody.o: ../../include/strategic/FAstroBody.h
FAstroBody.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FFleet.o: ../../include/strategic/FFleet.h ../../include/core/FPObject.h
FFleet.o: ../../include/core/FObject.h ../../include/ships/FVehicle.h
FFleet.o: ../../include/core/FGameConfig.h ../../include/weapons/FWeapon.h
FFleet.o: ../../include/Frontier.h ../../include/wxWidgets.h
FFleet.o: ../../include/strategic/FJumpRoute.h
FFleet.o: ../../include/strategic/FSystem.h ../../include/strategic/FPlanet.h
FFleet.o: ../../include/strategic/FAstroBody.h
FFleet.o: ../../include/strategic/FFleet.h ../../include/strategic/FSystem.h
FGame.o: ../../include/strategic/FGame.h ../../include/core/FPObject.h
FGame.o: ../../include/core/FObject.h ../../include/strategic/FPlayer.h
FGame.o: ../../include/strategic/FFleet.h ../../include/strategic/FMap.h
FGame.o: ../../include/strategic/FSystem.h ../../include/strategic/FPlanet.h
FGame.o: ../../include/strategic/FAstroBody.h ../../include/ships/FVehicle.h
FGame.o: ../../include/core/FGameConfig.h ../../include/weapons/FWeapon.h
FGame.o: ../../include/strategic/FJumpRoute.h
FGame.o: ../../include/strategic/FFleet.h ../../include/gui/SystemDialogGUI.h
FGame.o: ../../include/strategic/FMap.h ../../include/strategic/FPlayer.h
FGame.o: ../../include/Frontier.h ../../include/wxWidgets.h
FGame.o: ../../include/gui/UPFUnattachedGUI.h
FGame.o: ../../include/gui/SatharFleetsGUI.h ../../include/gui/ViewFleetGUI.h
FGame.o: ../../include/strategic/FSystem.h
FGame.o: ../../include/gui/SatharRetreatGUI.h
FGame.o: ../../include/gui/SelectCombatGUI.h ../../include/gui/WXMapDisplay.h
FGame.o: ../../include/gui/WXPlayerDisplay.h
FJumpRoute.o: ../../include/strategic/FJumpRoute.h
FJumpRoute.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FJumpRoute.o: ../../include/strategic/FSystem.h
FJumpRoute.o: ../../include/strategic/FPlanet.h
FJumpRoute.o: ../../include/strategic/FAstroBody.h
FJumpRoute.o: ../../include/ships/FVehicle.h ../../include/core/FGameConfig.h
FJumpRoute.o: ../../include/weapons/FWeapon.h
FJumpRoute.o: ../../include/strategic/FFleet.h
FMap.o: ../../include/strategic/FMap.h
FPlanet.o: ../../include/strategic/FPlanet.h
FPlanet.o: ../../include/strategic/FAstroBody.h ../../include/core/FPObject.h
FPlanet.o: ../../include/core/FObject.h ../../include/ships/FVehicle.h
FPlanet.o: ../../include/core/FGameConfig.h ../../include/weapons/FWeapon.h
FPlayer.o: ../../include/strategic/FPlayer.h
FPlayer.o: ../../include/strategic/FJumpRoute.h ../../include/core/FPObject.h
FPlayer.o: ../../include/core/FObject.h ../../include/strategic/FSystem.h
FPlayer.o: ../../include/strategic/FPlanet.h
FPlayer.o: ../../include/strategic/FAstroBody.h
FPlayer.o: ../../include/ships/FVehicle.h ../../include/core/FGameConfig.h
FPlayer.o: ../../include/weapons/FWeapon.h ../../include/strategic/FFleet.h
FPlayer.o: ../../include/gui/WXMapDisplay.h ../../include/strategic/FMap.h
FSystem.o: ../../include/strategic/FSystem.h
