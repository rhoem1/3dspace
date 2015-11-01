# simplistic makefile

GCC=gcc
GPP=g++
LD=g++

INCLUDES=-I. 
DEFINES=-std=c++11 -mfpmath=sse -O3
EXTRA_DEFINES=-g
CFLAGS=$(INCLUDES) $(DEFINES) $(EXTRA_DEFINES)
CPPFLAGS=$(INCLUDES) $(DEFINES) $(EXTRA_DEFINES)

LDFLAGS= 
POST_LIBS= -lSDL2 -lm -lGL

APP=3dtest

OBJS= \
	  main.o \
	  vec3.o \
	  mesh.o \
	  unit_shapes.o \
	  sdlViewport.o

all: $(APP)

$(APP): $(OBJS)
	$(LD) $(LDFLAGS) -o $(APP) $(OBJS) $(SOUND_OBJS) $(POST_LIBS)


# depends

main.o: main.cpp sdlViewport.h vec3.h unit_shapes.h tank.h mesh.h
	$(GPP) $(CPPFLAGS) -o $@ -c $<

vec3.o: vec3.cpp vec3.h
	$(GPP) $(CPPFLAGS) -o $@ -c $<

mesh.o: mesh.cpp mesh.h vec3.h
	$(GPP) $(CPPFLAGS) -o $@ -c $<

unit_shapes.o: unit_shapes.cpp unit_shapes.h mesh.h vec3.h
	$(GPP) $(CPPFLAGS) -o $@ -c $<

tank.o: tank.cpp tank.h mesh.h vec3.h
	$(GPP) $(CPPFLAGS) -o $@ -c $<

sdlViewport.o: sdlViewport.cpp sdlViewport.h
	$(GPP) $(CPPFLAGS) -o $@ -c $<



clean: 
	rm -f *.o $(APP)

clean-all: clean 
