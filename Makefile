build:
	g++ tema1.cpp Mapper.cpp Reducer.cpp -Wall -o tema1 -lpthread
clean:
	rm tema1