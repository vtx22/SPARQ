CCX = g++
CFLAGS = -Wall

NAME := SPARQ

SRC_FOLDER := src

all: compile link

compile:
	g++ $(CFLAGS) -Isfml/include -I./imgui-sfml/ -I./imgui-docking/ -I./implot/ -c ./*.cpp imgui-docking/*.cpp imgui-sfml/*cpp implot/*.cpp

link: 
	g++ *.o -o $(NAME) -Lsfml/lib -Limgui-sfml/ -Limgui-docking/ -Limplot/ -lsfml-graphics -lsfml-window -lsfml-system -lOpengl32