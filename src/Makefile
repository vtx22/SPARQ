NAME := SPARQ

CCX = g++ 
CFLAGS = -Wall -std=c++17 

OWN_SRC_FILES = $(wildcard ./*.cpp)
EXT_SRC_FILES = $(wildcard imgui/*.cpp imgui-sfml/*.cpp implot/*.cpp ImStyles/*.cpp)

INCLUDE_DIRS = -I. -I./imgui-sfml/ -I./imgui/ -I./implot/ -I./ImStyles/
SFML_LIB_DIR = -Limgui-sfml/ -Limgui/ -Limplot/ -LImStyles/ 

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	EXT_SRC_FILES += $(wildcard cpp-serial-lin/src/*.cpp)

	CFLGAS += -DBUILD_LINUX
	LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lGL
	INCLUDE_DIRS += -I./sfml-linux/include/ -I./cpp-serial-lin/src
	SFML_LIB_DIR += -Lsfml-linux/lib/ -Lcpp-serial-lin/
endif

ifeq ($(OS), Windows_NT)
	EXT_SRC_FILES += $(wildcard cpp-serial-win/src/*.cpp)

	CFLAGS += -DBUILD_WINDOWS -DIMGUI_SFML_VIEWPORTS_ENABLE -DSFML_STATIC -DSFML_USE_STATIC_STD_LIBS
	LIBS = -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -static-libstdc++ -static-libgcc -lwinmm -lopengl32 -lgdi32 -lfreetype -ldwmapi --static
	INCLUDE_DIRS += -I./sfml-windows/include/ -I./cpp-serial-win/src
	SFML_LIB_DIR += -Lsfml-windows/lib/ -Lcpp-serial-win/ SPARQ.res
ifdef release
    LIBS += -mwindows
endif
endif

OWN_OBJ_FILES = $(OWN_SRC_FILES:.cpp=.o)
EXT_OBJ_FILES = $(EXT_SRC_FILES:.cpp=.o)
OBJ_FILES = $(OWN_OBJ_FILES) $(EXT_OBJ_FILES)


# Build targets
all: $(NAME)

# Link step
$(NAME): $(OBJ_FILES)
	$(CCX) $(OBJ_FILES) -o $(NAME) $(SFML_LIB_DIR) $(LIBS)
	@echo Build complete!

# Compile step
%.o: %.cpp
	$(CCX) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Clean up object files and binary

clean:
ifeq ($(OS), Windows_NT)
	del /Q $(subst /,\,$(OBJ_FILES))
else
	rm -f $(OBJ_FILES) $(NAME)
endif

clean_all:
	del /Q $(subst /,\,$(OBJ_FILES))
	@echo Cleaned all object files.