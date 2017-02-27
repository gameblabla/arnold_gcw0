# Makefile for Arnold

CC = gcc
BIND = $(CC)
#   CFLAGS    flags for C compile
#   LFLAGS1   flags after output file spec, before obj file list
#   LFLAGS2   flags after obj file list (libraries, etc)

#use this for RedHat/SuSE Linux
X11INC = -I
X11LIB = -L

# Use this for Solaris 2.5.1 with OpenWin
#X11INC = -I/usr/X11R6/include -I/usr/openwin/share/include
#X11LIB = -L/usr/X11R6/lib -L/usr/openwin/lib

X11LIBS = $(X11LIB) -lX11 -lXext

# Comment to following two lines to disable GTK+
GTKINC = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  
GTKLIB = -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  

# Comment to following two lines to disable SDL
SDLINC = -I/usr/include/SDL -D_REENTRANT
SDLLIB = -L/usr/lib -lSDL -lpthread

# Comment to following two lines to disable ALSA
ALSAINC = 
ALSALIB =  -lasound -lm -ldl -lpthread

# Comment to following two lines to disable PULSE
PULSEINC = 
PULSELIB = 

#-Wall for max warnings!
CFLAGS =  -pg -O2  $(SDLINC) $(ALSAINC) $(PULSEINC) -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SDL=1 -DHAVE_LIBASOUND=1 -DHAVE_ALSA=1 -DHAVE_SYS_SOUNDCARD_H=1 $(X11INC) -I. -DUNIX -DCPC_LSB_FIRST -Wall -Wno-unused
#-g3 -pg -dm -dp -a
#LFLAGS1 =  -lasound -lm -ldl -lpthread $(GTKLIB) $(SDLLIB) $(ALSALIB) $(PULSELIB) $(X11LIBS)
LFLAGS1 = 
#LFLAGS1 =  -static $(SDLLIB) $(ALSALIB) $(PULSELIB) -dynamic $(GTKLIB) -lasound -lm -ldl -lpthread $(X11LIBS)
#debug
LFLAGS2 = -lasound -lm -ldl -lpthread $(SDLLIB) $(ALSALIB) $(PULSELIB) $(X11LIBS) -pg
#LFLAGS2= -g3 -pg -dm -dp -a
#normal
#LFLAGS2 = -s

TARGET_CPU = i686

CPC_O=	cpc/arnold.o cpc/asic.o cpc/audioevent.o cpc/bmp.o cpc/cpc.o \
	cpc/crtc.o cpc/dumpym.o cpc/fdc.o cpc/fdd.o cpc/fdi.o \
	cpc/garray.o cpc/multface.o cpc/printer.o cpc/psgplay.o \
	cpc/psg.o cpc/render.o cpc/render5.o \
	cpc/snapshot.o cpc/sampload.o cpc/spo256.o cpc/pal.o \
	cpc/voc.o cpc/tzxold.o cpc/wav.o cpc/westpha.o cpc/yiq.o \
	cpc/z8536.o cpc/csw.o cpc/cassette.o cpc/amsdos.o \
	cpc/diskimage/diskimg.o cpc/ramrom.o \
	cpc/diskimage/dsk.o cpc/diskimage/extdsk.o \
	cpc/diskimage/iextdsk.o \
	cpc/z80/z80.o  \
	cpc/riff.o cpc/snapv3.o \
	cpc/messages.o

UNIX_O= unix/main.o unix/host.o unix/global.o unix/display.o \
	unix/display_sdl.o unix/sdlsound.o unix/configfile.o \
	unix/roms.o unix/ifacegen.o unix/alsasound.o unix/alsasound-mmap.o \
	unix/alsasound-common.o unix/osssound.o unix/sound.o \
	unix/pulseaudiosound.o

ROMS_BIN=	roms/amsdose/amsdos.rom,roms/cpc464e/os.rom,roms/cpc464e/basic.rom,roms/cpc664e/os.rom,roms/cpc664e/basic.rom,roms/cpc6128e/os.rom,roms/cpc6128e/basic.rom,roms/cpc6128s/os.rom,roms/cpc6128s/basic.rom,roms/cpcplus/system.cpr,roms/kcc/kccos.rom,roms/kcc/kccbas.rom

arnold:  conditionals roms $(CPC_O) $(UNIX_O)
	$(BIND) -Wl,-b,binary,$(ROMS_BIN),-b,$(TARGET_FMT) -o arnold $(LFLAGS1) $(CPC_O) \
	$(UNIX_O) $(LFLAGS2)

conditionals:
ifeq ($(TARGET_CPU),ppc)
TARGET_FMT=elf32-ppc
else
TARGET_FMT=elf32-i386
endif

roms:
	ln -s ../roms .

ctags:
	ctags -R
clean:
	rm -rf cpc/*.o
	rm -rf ifacegen/*.o
	rm -rf unix/*.o
	rm -rf cpc/debugger/*.o
	rm -rf cpc/diskimage/*.o
	rm -rf cpc/z80/*.o
	rm -rf tags
distclean: clean
	rm -rf *~ */*~
	rm -f Makefile
	rm -f config.cache
	rm -f config.log
	rm -f config.status
realclean: distclean
	rm -f aclocal.m4
	rm -f configure
archive:
	$(clean)
	cp makefile unix/makefile.unx
	zip -r cpccore.zip cpc
	zip -r ifacegen.zip ifacegen
	zip -r unix.zip unix
	zip -r roms.zip roms

