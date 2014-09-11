#CC=gcc
#CXX=g++
#RM=rm -f
#CPPFLAGS=-g -std=c++11 -pthread
#LDFLAGS=-g $(shell root-config --ldflags)
#LDLIBS=$(shell root-config --libs)
#
## SRCS=tool.cc support.cc
#SRC_DIR=src
#SRCS=$(sort $(notdir $(wildcard ${SRC_DIR}/*.cc)) )
#OBJS=$(subst .cc,.o,$(SRCS))
#
#all: t23m-specific
#
#t23m-specific: $(OBJS)
#	g++ $(LDFLAGS) -o tool $(OBJS) $(LDLIBS)
#
#depend: .depend
#
#.depend: $(SRCS)
#	rm -f ./.depend
#	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;
#
#clean:
#	$(RM) $(OBJS)
#
#dist-clean: clean
#	$(RM) *~ .dependtool
#
#include .depend

include $(T23MFW_ROOT)/Make_T23MFW.mk

VPATH=src/:$(T23SRC)/

STRATEGY_LINK_FLAGS= $(T23MFW_LINK_FLAGS)
STRATEGY_COMPILE_FLAGS= $(T23MFW_CC_COMPILE_FLAGS) -I$(T23SRC)/ -Isrc/

STRATEGY_CC_FILES= $(wildcard src/*.cc)
STRATEGY_OBJ_FILES= $(addprefix objs/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))

ALL_CC_FILES= $(T23MFW_CC_FILES) $(STRATEGY_CC_FILES)
ALL_OBJ_FILES= $(T23MFW_OBJ_FILES) $(STRATEGY_OBJ_FILES)

t23_strategy.dll: $(ALL_OBJ_FILES)
	$(CC) $(STRATEGY_LINK_FLAGS) -o $@ $(ALL_OBJ_FILES)

# $(T23SRC)/%.cc
# objs-t23m/%.o: $(T23SRC)/%.cc
# 	g++ $(STRATEGY_COMPILE_FLAGS) -c -o $@ $<

objs/%.o: %.cc
	$(CC) $(STRATEGY_COMPILE_FLAGS) -c -o $@ $<


-include $(ALL_OBJ_FILES:.o=.d)
