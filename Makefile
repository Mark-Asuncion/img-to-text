CC = g++
CCFLAGS = $(shell Magick++-config --cppflags --cxxflags --ldflags --libs)
OPTFLAGS = -Wextra -Wall -Wpedantic -std=c++17
DEBUG = -DDEBUG

SOURCES = ./src/main.cpp
BUILDDIR = ./build
TARGET = $(BUILDDIR)/toascii
ASSET = ./assets/sample.bmp

$(TARGET): $(SOURCES)
	$(CC) $(OPTFLAGS) $(DEBUG) -o $(TARGET) $(SOURCES) $(CCFLAGS)

.PHONY: compile_flags.txt

compile_flags.txt:
	@echo "$(OPTFLAGS) $(CCFLAGS) $(DEBUG)" | tr ' ' '\n' > compile_flags.txt
