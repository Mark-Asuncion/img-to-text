CC = g++
CXXFLAGS = $(shell /opt/ImageMagick-7.1.1/bin/Magick++-config --cppflags)
LDFLAGS = $(shell /opt/ImageMagick-7.1.1/bin/Magick++-config --ldflags)
OPTFLAGS = -Wall -O2 -g

SRC = ./src/main.cpp
TARGET = ./build/demo

$(TARGET): $(SRC)
	$(CC) $(CXXFLAGS) $(OPTFLAGS) -o $@ $^ $(LDFLAGS)
