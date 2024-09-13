CCX = g++
CFLAGS = -Wall

NAME := SPARQ


all: compile link

compile:
	$(CCX) $(CFLAGS) -Isfml/include -I./imgui-sfml/ -I./imgui/ -I./implot/ -I./ImStyles/ -I./cpp-serial-win/src -c ./*.cpp imgui/*.cpp imgui-sfml/*.cpp implot/*.cpp cpp-serial-win/src/*.cpp ImStyles/*.cpp

link: 
	$(CCX) *.o -o $(NAME) -Lsfml/lib -Limgui-sfml/ -Limgui/ -Limplot/ -Lcpp-serial-win/ -LImStyles/ -lsfml-graphics -lsfml-window -lsfml-system -lOpengl32 -lDwmapi

