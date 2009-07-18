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

OBJS = FGameTest.o FPlanetTest.o FFleetTest.o FJumpRouteTest.o \
       FSystemTest.o FMapTest.o #FPlayer.o \
       #FGame.o FAstroBodyTest.o 

TARGET = libstrategicTests.a

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

FFleetTest.o: FFleetTest.h ../../include/strategic/FFleet.h
FFleetTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FFleetTest.o: ../../include/ships/FVehicle.h ../../include/weapons/FWeapon.h
FFleetTest.o: ../../include/strategic/FSystem.h
FFleetTest.o: ../../include/strategic/FPlanet.h
FFleetTest.o: ../../include/strategic/FAstroBody.h
FFleetTest.o: ../../include/strategic/FFleet.h
FGameTest.o: FGameTest.h ../../include/strategic/FGame.h
FGameTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FGameTest.o: ../../include/strategic/FPlayer.h
FGameTest.o: ../../include/strategic/FFleet.h ../../include/strategic/FMap.h
FGameTest.o: ../../include/strategic/FSystem.h
FGameTest.o: ../../include/strategic/FPlanet.h
FGameTest.o: ../../include/strategic/FAstroBody.h
FGameTest.o: ../../include/ships/FVehicle.h ../../include/weapons/FWeapon.h
FGameTest.o: ../../include/strategic/FJumpRoute.h
FJumpRouteTest.o: FJumpRouteTest.h ../../include/strategic/FJumpRoute.h
FJumpRouteTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FJumpRouteTest.o: ../../include/strategic/FSystem.h
FJumpRouteTest.o: ../../include/strategic/FPlanet.h
FJumpRouteTest.o: ../../include/strategic/FAstroBody.h
FJumpRouteTest.o: ../../include/ships/FVehicle.h
FJumpRouteTest.o: ../../include/weapons/FWeapon.h
FJumpRouteTest.o: ../../include/strategic/FFleet.h
FJumpRouteTest.o: ../../include/strategic/FSystem.h
FMapTest.o: FMapTest.h ../../include/strategic/FMap.h
FMapTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FMapTest.o: ../../include/strategic/FSystem.h
FMapTest.o: ../../include/strategic/FPlanet.h
FMapTest.o: ../../include/strategic/FAstroBody.h
FMapTest.o: ../../include/ships/FVehicle.h ../../include/weapons/FWeapon.h
FMapTest.o: ../../include/strategic/FFleet.h
FMapTest.o: ../../include/strategic/FJumpRoute.h
FPlanetTest.o: FPlanetTest.h ../../include/strategic/FPlanet.h
FPlanetTest.o: ../../include/strategic/FAstroBody.h
FPlanetTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FPlanetTest.o: ../../include/ships/FVehicle.h ../../include/weapons/FWeapon.h
FSystemTest.o: FSystemTest.h ../../include/strategic/FSystem.h
FSystemTest.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FSystemTest.o: ../../include/strategic/FPlanet.h
FSystemTest.o: ../../include/strategic/FAstroBody.h
FSystemTest.o: ../../include/ships/FVehicle.h ../../include/weapons/FWeapon.h
FSystemTest.o: ../../include/strategic/FFleet.h
