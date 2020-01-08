# sc-prot Metamod Plugin sc-prot_mm.dll and sc-prot_mm_i386.so Makefile for win32 and linux


ifeq ($(OS),Windows_NT)

	COMPILER = gcc -mno-cygwin
	LINKER   = gcc -mno-cygwin 

	DELCMD   = del
	
	CFLAGS   = 
	LFLAGS   = -mdll -mwindows -lstdc++ -Xlinker --add-stdcall-alias
	VERSION  = version.o
	RC       = sc-prot_mm.rc
	DLLNAME  = sc-prot_mm.dll
	
else

	COMPILER = gcc
	LINKER   = gcc
	DELCMD   = -rm -f
	
	CFLAGS   = -fPIC -Dstricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -Dstrcmpi=strcasecmp
	LFLAGS   = -shared -ldl -lm
	VERSION  = 
	RC       = 
	DLLNAME  = sc-prot_mm_i386.so

endif


OFLAGS  = -m32 -O3 -ffast-math -fomit-frame-pointer
CFLAGS += $(OFLAGS) -Wall -g0 -fno-exceptions -fno-rtti 
LFLAGS += -s


METADIR = ./metamod


INCLUDEDIRS = -I. -I$(METADIR) -Iengine -Icommon -Ipm_shared -Idlls


OBJ =	meta_api.o		\
        plugin.o		\
				\
        dllapi.o		\
        engine_api.o		\
				\
        cplayer.o		\
        sc-prot_util.o		\
        cvar.o			\
				\
        $(VERSION)


DOCC = $(COMPILER) $(CFLAGS) $(INCLUDEDIRS) -o $@ -c $<
DOO = $(LINKER) -o $@ $(OBJ) $(LFLAGS)


$(DLLNAME) : $(OBJ) 
	$(DOO)


clean:
	$(DELCMD) $(OBJ) $(DLLNAME) *.depend


./$(VERSION): ./$(RC)
	windres -o $@ -i $<
	
./$(VERSION): ./$(RC) vers_plugin.h


# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.depend)


./%.o: ./%.cpp
	$(DOCC)
	$(COMPILER) -MM $(INCLUDEDIRS) $< > $*.depend


