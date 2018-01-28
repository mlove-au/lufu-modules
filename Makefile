
# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS += -I./third_party/

# Careful about linking to libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine.
LDFLAGS += 

# Add .cpp and .c files to the build
SOURCES = $(wildcard src/*.cpp)

SOURCES += third_party/AudioFile/AudioFile.cpp

# Must include the VCV plugin Makefile framework
include ../../plugin.mk

run:
	make -C ../../ run
# Convenience target for including files in the distributable release
DIST_NAME = Multiclock
.PHONY: dist
dist: all
ifndef VERSION
	$(error VERSION must be defined when making distributables)
endif
	mkdir -p dist/$(DIST_NAME)
	cp LICENSE* dist/$(DIST_NAME)/
	cp $(TARGET) dist/$(DIST_NAME)/
	cp -R res dist/$(DIST_NAME)/
	cd dist && zip -5 -r $(DIST_NAME)-$(VERSION)-$(ARCH).zip $(DIST_NAME)



tests:
	$(CXX) $(CXXFLAGS) $(FLAGS) src/tests.cxx -o unittests.exe 

