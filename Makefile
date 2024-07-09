CC = g++
CCFLAGS = $(shell Magick++-config --cppflags --cxxflags --ldflags --libs)
OPTFLAGS = -Wextra -Wall -Wpedantic -std=c++17

SOURCES = $(wildcard ./src/*.cpp)
BUILDDIR = ./build
TARGET = $(BUILDDIR)/toascii
ASSET = ./assets/sample.bmp

$(TARGET): $(SOURCES)
	$(CC) $(OPTFLAGS) -o $(TARGET) $(SOURCES) $(CCFLAGS)
	@cp $(ASSET) $(BUILDDIR)

.PHONY: compile_flags.txt

compile_flags.txt:
	@echo "$(OPTFLAGS) $(CCFLAGS)" | tr ' ' '\n' > compile_flags.txt
