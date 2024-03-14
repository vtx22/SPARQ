CCX = g++
CFLAGS = -Wall

NAME := SPARQ


all: compile link

compile:
	$(CCX) $(CFLAGS) -Isfml/include -I./imgui-sfml/ -I./imgui-docking/ -I./implot/ -c ./*.cpp imgui-docking/*.cpp imgui-sfml/*cpp implot/*.cpp

link: 
	$(CCX) *.o -o $(NAME) -Lsfml/lib -Limgui-sfml/ -Limgui-docking/ -Limplot/ -lsfml-graphics -lsfml-window -lsfml-system -lOpengl32

