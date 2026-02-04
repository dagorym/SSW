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

OBJS = FDefense.o FNone.o FReflectiveHull.o FMaskingScreen.o FICM.o \
       FStasisScreen.o FProtonScreen.o FElectronScreen.o

TARGET = libdefenses.a

all $(TARGET): $(OBJS)
	ar rcsu $(TARGET) $(OBJS)

objs: $(OBJS)
	rm -f *.gcda
	rm  -f*.gcov
	rm -f *.gcno

.PHONY:
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

# DO NOT DELETE

FDefense.o: ../../include/defenses/FDefense.h ../../include/core/FPObject.h
FDefense.o: ../../include/core/FObject.h ../../include/weapons/FWeapon.h
FDefense.o: ../../include/defenses/defenses.h ../../include/defenses/FNone.h
FDefense.o: ../../include/defenses/FReflectiveHull.h
FDefense.o: ../../include/defenses/FMaskingScreen.h
FDefense.o: ../../include/defenses/FICM.h
FDefense.o: ../../include/defenses/FStasisScreen.h
FDefense.o: ../../include/defenses/FProtonScreen.h
FDefense.o: ../../include/defenses/FElectronScreen.h
FElectronScreen.o: ../../include/defenses/FElectronScreen.h
FElectronScreen.o: ../../include/defenses/FDefense.h
FElectronScreen.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FElectronScreen.o: ../../include/weapons/FWeapon.h
FICM.o: ../../include/defenses/FICM.h ../../include/defenses/FDefense.h
FICM.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FICM.o: ../../include/weapons/FWeapon.h
FMaskingScreen.o: ../../include/defenses/FMaskingScreen.h
FMaskingScreen.o: ../../include/defenses/FDefense.h
FMaskingScreen.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FMaskingScreen.o: ../../include/weapons/FWeapon.h
FNone.o: ../../include/defenses/FNone.h ../../include/defenses/FDefense.h
FNone.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FNone.o: ../../include/weapons/FWeapon.h
FProtonScreen.o: ../../include/defenses/FProtonScreen.h
FProtonScreen.o: ../../include/defenses/FDefense.h
FProtonScreen.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FProtonScreen.o: ../../include/weapons/FWeapon.h
FReflectiveHull.o: ../../include/defenses/FReflectiveHull.h
FReflectiveHull.o: ../../include/defenses/FDefense.h
FReflectiveHull.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FReflectiveHull.o: ../../include/weapons/FWeapon.h
FStasisScreen.o: ../../include/defenses/FStasisScreen.h
FStasisScreen.o: ../../include/defenses/FDefense.h
FStasisScreen.o: ../../include/core/FPObject.h ../../include/core/FObject.h
FStasisScreen.o: ../../include/weapons/FWeapon.h
