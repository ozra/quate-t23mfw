include $(T23MFW_ROOT)/Make_T23MFW.mk

VPATH=src/:$(T23SRC):$(T23SRC)/QTA/src/

STRATEGY_LINK_FLAGS=-static

STRATEGY_COMPILE_FLAGS= $(T23MFW_CC_COMPILE_FLAGS) \
	-Iinclude/ \
    -Iext/T23MFW/ext/dukascopy/lib/easylzma/src/ \
    -Iext/T23MFW/ext/dukascopy/include/ \
    -Iext/T23MFW/ext/boost_1_56_0/boost/ \

STRATEGY_LIBS=-Lext/T23MFW/ext/boost-libs/ \
			-lboost_system \
			-lboost_thread \
			-lboost_filesystem \
			-lboost_date_time \
			-lboost_program_options
#		-lboost_regex
#        ext/T23MFW/ext/easylzma-libs/libeasylzma_s.a \
#        ext/T23MFW/ext/dukascopy-libs/libninety47_dukascopy.a \
#        -lboost_container \


STRATEGY_CC_FILES= $(wildcard src/*.cc)
STRATEGY_OBJ_FILES_DEVDBG= $(addprefix objs/DbgDev/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))
STRATEGY_OBJ_FILES_DEVFAST= $(addprefix objs/DevFast/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))
STRATEGY_OBJ_FILES_NOBELT= $(addprefix objs/NoBelt/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))

ALL_CC_FILES= $(T23MFW_CC_FILES) $(STRATEGY_CC_FILES)
ALL_HH_FILES = $(addprefix include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))
ALL_OBJ_FILES_DEVDBG= $(T23MFW_OBJ_FILES_DEVDBG) $(STRATEGY_OBJ_FILES_DEVDBG)
ALL_OBJ_FILES_DEVFAST= $(T23MFW_OBJ_FILES_DEVFAST) $(STRATEGY_OBJ_FILES_DEVFAST)
ALL_OBJ_FILES_NOBELT= $(T23MFW_OBJ_FILES_NOBELT) $(STRATEGY_OBJ_FILES_NOBELT)

ALL_OBJ_FILES=$(ALL_OBJ_FILES_DEVDBG) $(ALL_OBJ_FILES_DEVFAST) $(ALL_OBJ_FILES_NOBELT)


all: devfast

clean:
	$(RM) include/* \
			bin/DbgDev/* bin/DevFast/* bin/NoBelt/* \
			objs/DbgDev/* objs/DevFast/* objs/NoBelt/*

devdbg: bin/DbgDev/strategy_standalone

devfast: bin/DevFast/strategy_standalone

nobelt: bin/NoBelt/strategy_standalone

bin/DbgDev/strategy_standalone: $(ALL_HH_FILES) $(ALL_OBJ_FILES_DEVDBG)
		$(T23MFW_LINKER_DBGDEV) $(STRATEGY_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_DEVDBG) \
		$(STRATEGY_LIBS)

include/%.hh: %.cc
	noah-cpp --fixed-path -o 'include/' $<

objs/DbgDev/%.o: %.cc
	$(T23MFW_COMPILER_DBGDEV) $(STRATEGY_COMPILE_FLAGS) -c -o $@ $<


bin/DevFast/strategy_standalone: $(ALL_HH_FILES) $(ALL_OBJ_FILES_DEVFAST)
		$(T23MFW_LINKER_DEVFAST) $(STRATEGY_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_DEVFAST) \
		$(STRATEGY_LIBS)

include/%.hh: %.cc
	noah-cpp --fixed-path -o 'include/' $<

objs/DevFast/%.o: %.cc
	$(T23MFW_COMPILER_DEVFAST) $(STRATEGY_COMPILE_FLAGS) -c -o $@ $<


bin/NoBelt/strategy_standalone: $(ALL_HH_FILES) $(ALL_OBJ_FILES_NOBELT)
		$(T23MFW_LINKER_NOBELT) $(STRATEGY_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_NOBELT) \
		$(STRATEGY_LIBS)

include/%.hh: %.cc
	noah-cpp --fixed-path -o 'include/' $<

objs/NoBelt/%.o: %.cc
	$(T23MFW_COMPILER_NOBELT) $(STRATEGY_COMPILE_FLAGS) -c -o $@ $<


-include $(ALL_OBJ_FILES:.o=.d)
