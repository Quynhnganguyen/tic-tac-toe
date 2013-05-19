CXX = g++
CXXFLAGS = -g3 -gdwarf-2 -Wall -pedantic -O0 -std=c++11
# linker flags for allegro library
LDFLAGS = `pkg-config --cflags --libs allegro-5.0 allegro_acodec-5.0 allegro_audio-5.0  allegro_color-5.0 allegro_dialog-5.0 allegro_font-5.0 allegro_image-5.0 allegro_main-5.0 allegro_memfile-5.0 allegro_physfs-5.0 allegro_primitives-5.0 allegro_ttf-5.0`


SOURCES = main.cpp 
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = tic-tac-toe

$(TARGET) : $(OBJECTS) Makefile.dependencies
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)


.PHONY: clean
clean:
	@rm -f $(TARGET) $(OBJECTS) core Makefile.dependencies

