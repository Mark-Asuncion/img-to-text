CC = g++
CXXFLAGS = $(shell /opt/ImageMagick-7.1.1/bin/Magick++-config --cppflags)
LDFLAGS = $(shell /opt/ImageMagick-7.1.1/bin/Magick++-config --ldflags)
OPTFLAGS = -Wall -O3

SRC = ./src/main.cpp
TARGET = ./build/imgtotext

$(TARGET): $(SRC)
	$(CC) $(CXXFLAGS) $(OPTFLAGS) -o $@ $^ $(LDFLAGS)
