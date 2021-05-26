CXX = g++
CXXFLAGS = -std=c++11 $(DEBUGFLAGS)
DEBUGFLAGS := -DDEBUG -g -O0
LIBS := -lGL -lglfw -Lm -Lc
LDFLAGS = $(DEBUGFLAGS)

SOURCE_FILES :=\
	src/main.cpp\
	src/entity/building.cpp\
	src/entity/monster.cpp\
	src/entity/monster_node.cpp\
	src/entity/orb.cpp\
	src/entity/tower.cpp\
	src/entity/tower_shot.cpp\
	src/entity/trap.cpp\
	src/ingame/ingame_building_controller.cpp\
	src/ingame/ingame_core.cpp\
	src/ingame/ingame_enemy_controller.cpp\
	src/ingame/ingame_input_handler.cpp\
	src/ingame/ingame_inventory.cpp\
	src/ingame/ingame_map.cpp\
	src/ingame/ingame_pathfinder.cpp\
	src/ingame/ingame_projectile_controller.cpp\
	src/ingame/ingame_renderer.cpp\
	src/interface/window.cpp\
	src/interface/window_inventory.cpp\
	src/interface/window_map.cpp\

SOURCE_FILES_LIB := \
	lib/fbg/custom_backend/glfw/fbg_glfw.c\
	lib/fbg/custom_backend/glfw/glew/glew.c\
	lib/fbg/src/fbgraphics.c\
   	lib/fbg/src/lodepng/lodepng.c\
	lib/fbg/src/nanojpeg/nanojpeg.c 

OBJECT_FILES = $(SOURCE_FILES:%.cpp=$(OBJDIR)/%.o)
OBJECT_FILES_LIB = $(SOURCE_FILES_LIB:%.c=$(OBJDIR)/%.o)
INCLUDES := -Iinc/ -Ilib/fbg/custom_backend/ -Ilib/fbg/src/
OBJDIR = _out

all : opengem

clean:
	rm -rf $(OBJDIR)
	rm -f opengem

.PHONY: all clean

opengem : $(OBJECT_FILES) $(OBJECT_FILES_LIB)
	$(CXX) $(LDFLAGS) -o "$@" $^ $(LIBS)

$(OBJECT_FILES) : $(OBJDIR)/%.o : %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -Werror $(INCLUDES) -c -o $@ $<

$(OBJECT_FILES_LIB) : $(OBJDIR)/%.o : %.c
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -fpermissive $(INCLUDES) -c -o $@ $<
