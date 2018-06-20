RACK_DIR ?= ../..
SLUG = lufu
VERSION = 0.6.0

SOURCES += $(wildcard src/*.cpp) third_party/AudioFile/AudioFile.cpp
DISTRIBUTABLES += $(wildcard LICENSE*) res
FLAGS += -Ithird_party
include $(RACK_DIR)/plugin.mk
