./bin/Linux/main: src/main.cpp src/glad.c src/Camera.cpp src/Game.cpp src/Input.cpp src/Model.cpp src/Shader.cpp src/Window.cpp include/matrices.h include/utils.h include/Camera.h include/Game.h include/Input.h include/Model.h include/Shader.h include/Window.h
	mkdir -p bin/Linux
	g++ -std=c++11 -Wall -Wno-unused-function -g -I ./include/ -o ./bin/Linux/main src/main.cpp src/glad.c src/Camera.cpp src/Game.cpp src/Input.cpp src/Model.cpp src/Shader.cpp src/Window.cpp ./lib-linux/libglfw3.a -lrt -lm -ldl -lX11 -lpthread -lXrandr -lXinerama -lXxf86vm -lXcursor

.PHONY: clean run
clean:
	rm -f bin/Linux/main

run: ./bin/Linux/main
	cd bin/Linux && ./main
