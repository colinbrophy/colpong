ifdef VERBOSE
        Q =
        E = @true 
else
        Q = @
        E = @echo 
endif

CFILES := $(shell find . -mindepth 1 -maxdepth 4 -name "*.c")
CXXFILES := $(shell find . -mindepth 1 -maxdepth 4 -name "*.cpp")

INFILES := $(CFILES) $(CXXFILES)

OBJFILES := $(CXXFILES:%.cpp=%) $(CFILES:%.c=%)
DEPFILES := $(CXXFILES:%.cpp=%) $(CFILES:%.c=%)
OFILES := $(OBJFILES:%=obj/%.o)

BINFILE = colpong

COMMONFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

ifdef DEBUG
        COMMONFLAGS := $(COMMONFLAGS) -g
endif
CFLAGS = $(COMMONFLAGS) --std=c99
CXXFLAGS = $(COMMONFLAGS) --std=c++14
DEPDIR = deps
all: $(BINFILE)
ifeq ($(MAKECMDGOALS),)
-include Makefile.dep
endif
ifneq ($(filter-out clean, $(MAKECMDGOALS)),)
-include Makefile.dep
endif

CC = gcc
CXX = g++


-include Makefile.local

.PHONY: clean all depend
.SUFFIXES:
obj/%.o: %.c
	$(E)C-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CC) -o $@ -c $< $(CFLAGS)
obj/%.o: %.cpp
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(CXXFLAGS)
Makefile.dep: $(CFILES) $(CXXFILES)
	$(E)Depend
	$(Q)for i in $(^); do $(CXX) $(CXXFLAGS) -MM "$${i}" -MT obj/`basename $${i%.*}`.o; done > $@

        
$(BINFILE): $(OFILES)
	$(E)Linking $@
	$(Q)$(CXX) -o $@ $(OFILES) $(LDFLAGS)
clean:
	$(E)Removing files
	$(Q)rm -f $(BINFILE) obj/* Makefile.dep
