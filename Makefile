# This makefile is way more complicated than it really needs to be, mostly because I wanted to actually figure out a lot of this stuff
# I've worked on things with Makefiles before, and some of the patterns that are shown here can be seen in them.
# The most relevant Makefile I've worked with is https://github.com/FlagBrew/PKSM/blob/master/3ds/Makefile, so that would be what I consider my source
# The features that I have here are:
#   Automatic discovery of source files (recursively) from directories listed
#   Inclusion of every directory in the INCLUDES variable being valid
#   Allows for duplicate-name files (base64.cpp and testDir/base64.cpp) to be compiled and used in the same project
#   Allows for duplicate-name, different-language files (base64.cpp and base64.c) to be compiled and used in the same project
#   Allows usage of .. in path names without completely screwing up the build directory's containment
# Over time I may end up fixing bugs in this, but I believe it's good for now

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

EXEC_NAME := opengem

# CC := gcc
# CXX := g++

DEBUG_FLAGS := -g -Og -DDEBUG
RELEASE_FLAGS := -O3 -flto

SOURCES := src
# For loose files not in the folders listed above
EXTRA_SOURCE_FILES :=
# Not recursive to avoid filename clashes
INCLUDES := inc

# Equivalent to the above, but files found through here will not be formatted
NOFORMAT_SOURCES := lib/fbg/src lib/fbgx
NOFORMAT_EXTRA_SOURCE_FILES := lib/fbg/custom_backend/glfw/fbg_glfw.c lib/fbg/custom_backend/glfw/glew/glew.c
NOFORMAT_INCLUDES := lib/fbg/custom_backend lib/fbg/src lib

BUILD_RELEASE := build_release
BUILD_DEBUG := build

OUT_RELEASE := out_release
OUT_DEBUG := out

# Uncomment this to make SOURCES recursive
SOURCES_RECURSIVE := 1
# Note that this is not a recursive folder blacklist (no matter the above setting). It blacklists exactly the folders you specify
SOURCES_BLACKLIST :=
SOURCES_FILE_BLACKLIST :=

CFLAGS := $(CFLAGS) -DGLEW_STATIC
CXXFLAGS := $(CXXFLAGS) -DGLEW_STATIC -std=c++17
ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lglfw3 -lgdi32 -lopengl32 -Lm -Lc -static
else
LDFLAGS := $(LDFLAGS) -lGL -lglfw -Lm -Lc
endif

# Past here there shouldn't be much that needs editing unless extra file types are necessary
#------------------------------------------------------------------------------------------------

ifeq ($(strip $(SOURCES_RECURSIVE)),1)
# Kinda hacky, but it works
SOURCES := $(filter-out $(addsuffix /,$(SOURCES_BLACKLIST)),$(sort $(dir $(call rwildcard,$(SOURCES),*.c *.cpp))))
NOFORMAT_SOURCES := $(filter-out $(addsuffix /,$(SOURCES_BLACKLIST)),$(sort $(dir $(call rwildcard,$(NOFORMAT_SOURCES),*.c *.cpp))))
CFILES := $(patsubst ./%,%,$(foreach dir,$(SOURCES),$(wildcard $(dir:=*.c)))) $(filter %.c,$(EXTRA_SOURCE_FILES))
NOFORMAT_CFILES := $(patsubst ./%,%,$(foreach dir,$(NOFORMAT_SOURCES),$(wildcard $(dir:=*.c)))) $(filter %.c,$(NOFORMAT_EXTRA_SOURCE_FILES))
CPPFILES := $(patsubst ./%,%,$(foreach dir,$(SOURCES),$(wildcard $(dir:=*.cpp)))) $(filter %.cpp,$(EXTRA_SOURCE_FILES))
NOFORMAT_CPPFILES := $(patsubst ./%,%,$(foreach dir,$(NOFORMAT_SOURCES),$(wildcard $(dir:=*.cpp)))) $(filter %.cpp,$(NOFORMAT_EXTRA_SOURCE_FILES))
else
SOURCES := $(filter-out $(SOURCES_BLACKLIST),$(SOURCES))
NOFORMAT_SOURCES := $(filter-out $(SOURCES_BLACKLIST),$(NOFORMAT_SOURCES))
CFILES := $(patsubst ./%,%,$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))) $(filter %.c,$(EXTRA_SOURCE_FILES))
NOFORMAT_CFILES := $(patsubst ./%,%,$(foreach dir,$(NOFORMAT_SOURCES),$(wildcard $(dir)/*.c))) $(filter %.c,$(NOFORMAT_EXTRA_SOURCE_FILES))
CPPFILES := $(patsubst ./%,%,$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))) $(filter %.cpp,$(EXTRA_SOURCE_FILES))
NOFORMAT_CPPFILES := $(patsubst ./%,%,$(foreach dir,$(NOFORMAT_SOURCES),$(wildcard $(dir)/*.cpp))) $(filter %.cpp,$(NOFORMAT_EXTRA_SOURCE_FILES))
endif

CFILES := $(filter-out $(SOURCES_FILE_BLACKLIST),$(CFILES))
NOFORMAT_CFILES := $(filter-out $(SOURCES_FILE_BLACKLIST),$(NOFORMAT_CFILES))
CPPFILES := $(filter-out $(SOURCES_FILE_BLACKLIST),$(CPPFILES))
NOFORMAT_CPPFILES := $(filter-out $(SOURCES_FILE_BLACKLIST),$(NOFORMAT_CPPFILES))

ifeq ($(strip $(RELEASE)),)
OPTIMIZE := $(DEBUG_FLAGS)
BUILD := $(BUILD_DEBUG)
OUT := $(OUT_DEBUG)
else
OPTIMIZE := $(RELEASE_FLAGS)
BUILD := $(BUILD_RELEASE)
OUT := $(OUT_RELEASE)
endif

CFLAGS := $(CFLAGS) $(foreach dir, $(INCLUDES) $(NOFORMAT_INCLUDES), -I$(CURDIR)/$(dir)) $(OPTIMIZE)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS) $(OPTIMIZE)
LDFLAGS := $(LDFLAGS) $(OPTIMIZE)

OFILES := $(CFILES:.c=.c.o) $(CPPFILES:.cpp=.cpp.o) $(NOFORMAT_CFILES:.c=.c.o) $(NOFORMAT_CPPFILES:.cpp=.cpp.o)
BUILD_OFILES := $(subst //,/,$(subst /../,/__PrEvDiR/,$(subst /,//, $(OFILES))))
BUILD_OFILES := $(patsubst ../%,__PrEvDiR/%,$(BUILD_OFILES))
BUILD_OFILES := $(addprefix $(BUILD)/, $(BUILD_OFILES))
DEPSFILES := $(BUILD_OFILES:.o=.d)

LD := $(if $(strip $(CPPFILES) $(NOFORMAT_CPPFILES)),$(CXX),$(CC))

ifeq ($(OS),Windows_NT)
WHICH = where
else
WHICH = which
endif

.PHONY: all clean format release debug spotless

all: format $(OUT)/$(EXEC_NAME)
	@cp $(OUT)/$(EXEC_NAME) .

release:
	$(MAKE) RELEASE=1 all

debug: all

clean:
	@rm -rf $(BUILD_DEBUG)
	@rm -rf $(OUT_DEBUG)

spotless: clean
	@rm -rf $(BUILD_RELEASE)
	@rm -rf $(OUT_RELEASE)

format:
ifneq ($(strip $(shell $(WHICH) clang-format)),)
	clang-format -i $(CFILES) $(CPPFILES) $(foreach dir, $(INCLUDES), $(call rwildcard,$(dir),*.h *.hpp))
endif

# Add extra file type rules below. Note how to place files in the build and out directories
#------------------------------------------------------------------------------------------------

$(OUT)/$(EXEC_NAME): $(BUILD_OFILES)
	@mkdir -p $(dir $@)
	$(LD) $(BUILD_OFILES) $(LDFLAGS) -o $@

$(BUILD)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -MMD -MP -MF $(@:.o=.d) $(CXXFLAGS) -c -o $@ $<

include $(wildcard $(DEPSFILES))
