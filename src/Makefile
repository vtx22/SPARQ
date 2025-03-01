NAME := SPARQ

CCX = g++ 
CFLAGS = -Wall -std=c++17 -DIMGUI_SFML_VIEWPORTS_ENABLE -DSFML_STATIC -DSFML_USE_STATIC_STD_LIBS --static
LIBS = -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -static-libstdc++ -static-libgcc -lwinmm -lopengl32 -lgdi32 -lfreetype -ldwmapi --static
# Directories
INCLUDE_DIRS = -I. -I./imgui-sfml/ -I./imgui/ -I./implot/ -I./sfml/include/ -I./ImStyles/ -I./cpp-serial-win/src
SFML_LIB_DIR = -Lsfml/lib -Limgui-sfml/ -Limgui/ -Limplot/ -LImStyles/ -Lcpp-serial-win/ SPARQ.res

# Source and object files
OWN_SRC_FILES = $(wildcard ./*.cpp)
EXT_SRC_FILES = $(wildcard imgui/*.cpp imgui-sfml/*.cpp implot/*.cpp ImStyles/*.cpp cpp-serial-win/src/*.cpp)

OWN_OBJ_FILES = $(OWN_SRC_FILES:.cpp=.o)
EXT_OBJ_FILES = $(EXT_SRC_FILES:.cpp=.o)
OBJ_FILES = $(OWN_OBJ_FILES) $(EXT_OBJ_FILES)

ifdef release
    LIBS += -mwindows
endif

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
	del /Q $(subst /,\,$(OWN_OBJ_FILES))

clean_all:
	del /Q $(subst /,\,$(OBJ_FILES))
	@echo Cleaned all object files.