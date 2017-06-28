# Makefile for unix systems
# this requires GNU make

# Select build type exe,lib,so to build an executable, a library or a shared 
# object
BUILD_TYPE=exe
# project name
PROJECT_NAME=vd


# set defines -DASD is eqal to a #define ASD in every file 
DEFINES = 
# Include settings -I... etc.
INCLUDES = -I./include 
# Library setting -L... -l... etc.
LIBRARIES =

#
# INPUT
#
# Add c files one by one
CFILES   = 
# Add cpp files one by one 
CPPFILES = 
# All *.c and *.cpp files in all SRCDIRs and beneath will be built
SRCDIR = src

#
# OUTPUT
#
# The binary directory. Where to write the finished files
BINDIR = bin
# Object directory for all intermediate data.
OBJDIR = obj
# The dependency files generated to track changes in headers
DEPDIR = $(OBJDIR)/dep

#
# EXECUTION
#
# The execution directory is where make run will call the binary file from
EXDIR= $(BINDIR)
ARGV=


# 
#  WARNINGS
#
WARNFLAGS+= -Wall -Wextra 
#WARNFLAGS+= -Wno-unused-variable
#WARNFLAGS+= -Wno-unused-but-set-variable
WARNFLAGS+= -Wno-unused-function 
WARNFLAGS+= -Wno-unused-parameter
WARNFLAGS+= -Wno-unknown-pragmas


#
# OPTIMIZATION
#
OPTIMIZEFLAGS = -ffast-math -mtune=native -march=native -O4 -DNDEBUG


ifeq ($(RELEASE), 1)
	CFLAGS =   $(WARNFLAGS) $(OPTIMIZEFLAGS) -s
	CXXFLAGS = $(WARNFLAGS) $(OPTIMIZEFLAGS) -s -std=c++11
else
	CFLAGS =   $(WARNFLAGS) -g
	CXXFLAGS = $(WARNFLAGS) -g -std=c++11
	DEFINES += -DNDEBUG
endif


###############################################################################
###############################################################################

# ADD libraries to LINK
LINK += $(LIBRARIES) 

# Set binary name
BIN_NAME= $(PROJECT_NAME)
ifeq ($(BUILD_TYPE),so)
	CXXFLAGS += -fPIC 
	CFLAGS += -fPIC 
	BIN_NAME = lib$(PROJECT_NAME).so.1 
	LINK += -shared -Wl,-soname,lib$(PROJECT_NAME).so.1 
	DEFINES+= -DBUILDING_DLL
endif
ifeq ($(BUILD_TYPE),lib)
	CXXFLAGS += -fPIC 
	CFLAGS += -fPIC 
	BIN_NAME = lib$(PROJECT_NAME).a 
endif

# Add includes to FLAGS
CPPFLAGS += $(INCLUDES) $(DEFINES)



# Find files
CFILES   += $(shell find $(SRCDIR) -name '*.c')
CPPFILES += $(shell find $(SRCDIR) -name '*.cpp')

OBJECTS =$(patsubst %.cpp,$(OBJDIR)/%_cpp.o,$(CPPFILES)) $(patsubst %.c,$(OBJDIR)%_c.o,$(CFILES))
SRCFILES=$(CPPFILES) $(CFILES) 


# build rules
.PHONY: all
all:	$(PROJECT_NAME)

# build and start with "make run"
.PHONY: run
run:	$(PROJECT_NAME)
	sh -c 'cd $(EXDIR) && $(abspath $(BINDIR)/$(BIN_NAME)) $(ARGV)' 

# automatic dependency generation 'borrowed' from Hass
# create $(DEPDIR) (and an empty file dir)
# create a .d file for every .c/.cpp source file which contains
# all dependencies for that file
.PHONY: depend
depend:	$(DEPDIR)/dependencies
DEPFILES = $(patsubst %.c,$(DEPDIR)/%.d,$(CFILES)) $(patsubst %.cpp,$(DEPDIR)/%.d,$(CPPFILES))
$(DEPDIR)/dependencies: $(DEPDIR)/dir $(DEPFILES)
	@cat $(DEPFILES) > $(DEPDIR)/dependencies
$(DEPDIR)/dir:
	@mkdir -p $(DEPDIR)
	@touch $(DEPDIR)/dir
$(DEPDIR)/%.d: %.c $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	mkdir -p $(dir $@)
	@set -e; $(CC) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
$(DEPDIR)/%.d: %.cpp $(DEPDIR)/dir
	mkdir -p $(dir $@)
	@echo rebuilding dependencies for $*
	@set -e; $(CXX) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
-include $(DEPDIR)/dependencies

$(PROJECT_NAME): Makefile setup_dir $(OBJECTS)  $(DEPDIR)/dependencies
ifeq ($(BUILD_TYPE),lib)
	$(AR) $(OBJECTS) -o $(BINDIR)/$(BIN_NAME)
else
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LINK) -o $(BINDIR)/$(BIN_NAME)
endif

$(OBJDIR)/%_c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -c $< -o $@

$(OBJDIR)/%_cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEFINES) -c $< -o $@

setup_dir:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	echo '$(CPPFLAGS) -std=c++11' > .clang_complete

.PHONY: clean
clean:
	@echo removing binaries: $(BINDIR)/$(BIN_NAME)
	@rm -f $(BINDIR)/$(PROJECT_NAME)
	@echo removing object files
	@rm -rf $(OBJDIR)
	@echo removing dependency files
	@rm -rf $(DEPDIR)

