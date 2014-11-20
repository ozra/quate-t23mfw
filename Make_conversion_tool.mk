include $(T23MFW_ROOT)/Make_T23MFW.mk

VPATH=src/:$(T23SRC)

SEQDACON_LINK_FLAGS=-static

SEQDACON_COMPILE_FLAGS= $(CPPFLAGS) \
	-Iinclude/ \
    -Iext/T23MFW/ext/dukascopy/lib/easylzma/src/ \
    -Iext/T23MFW/ext/dukascopy/include/ \
    -Iext/T23MFW/ext/boost_1_56_0/boost/ \

SEQDACON_LIBS=-Lext/T23MFW/ext/boost-libs/ \
			-lboost_system \
			-lboost_thread \
			-lboost_filesystem \
			-lboost_date_time \
			-lboost_program_options
#		-lboost_regex
#        ext/T23MFW/ext/easylzma-libs/libeasylzma_s.a \
#        ext/T23MFW/ext/dukascopy-libs/libninety47_dukascopy.a \
#        -lboost_container \


SEQDACON_CC_FILES=	\
			rfx11_types.cc \
			rfx11_lib_debug.cc \
			seqdacon_main.cc \
			QuantBasic_DESIGN_CHOICES.cc \
			QuantBasicTypes.cc \
			QuantBase.cc \
			QuantTime.cc \
			HardSignal.cc \
			mutatable_buffer.cc \
			QuantTick.cc \
			QuantBuffersBase.cc \
			QuantBuffersReverseIndexedCircular.cc \
			QuantProfiling.cc \
			MutatingString.cc \
			QuantSequentialDataUtils.cc \
			QuantSequentialDataPrefetcher.cc \
			QuantSequentialData_TradeTicksAbstract.cc \
			QuantSequentialData_DukascopyInterfaceCode.cc \
			QuantSequentialData_DukascopyTicks.cc \
			QuantSequentialData_QuledAbstract.cc \
			QuantSequentialData_T23MFWTicks.cc


SEQDACON_OBJ_FILES_DEVDBG= $(addprefix objs/DbgDev/,$(notdir $(SEQDACON_CC_FILES:.cc=.o)))
SEQDACON_OBJ_FILES_RELEASE= $(addprefix objs/Release/,$(notdir $(SEQDACON_CC_FILES:.cc=.o)))

# ALL_CC_FILES= $(T23MFW_CC_FILES) $(SEQDACON_CC_FILES)
ALL_CC_FILES= $(SEQDACON_CC_FILES)
ALL_HH_FILES = $(addprefix include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))
ALL_OBJ_FILES_DEVDBG= $(SEQDACON_OBJ_FILES_DEVDBG)
ALL_OBJ_FILES_RELEASE= $(SEQDACON_OBJ_FILES_RELEASE)

ALL_OBJ_FILES=$(ALL_OBJ_FILES_DEVDBG) $(ALL_OBJ_FILES_RELEASE)


all: dbgdev

clean:
	$(RM) include/* \
			bin/DbgDev/* bin/Release/* \
			objs/DbgDev/* objs/Release/*

dbgdev: bin/DbgDev/seqdacon_standalone

release: bin/Release/seqdacon_standalone

bin/DbgDev/seqdacon_standalone: $(ALL_HH_FILES) $(ALL_OBJ_FILES_DEVDBG)
		$(T23MFW_LINKER_DBGDEV) $(SEQDACON_LINK_FLAGS) \
		-o $@ \
		$(ALL_OBJ_FILES_DEVDBG) \
		$(SEQDACON_LIBS)

include/%.hh: %.cc
#include/%.hh: $(SEQDACON_CC_FILES)
	noah-cpp --fixed-path -o 'include/' $<

objs/DbgDev/%.o: %.cc
	$(T23MFW_COMPILER_DBGDEV) $(SEQDACON_COMPILE_FLAGS) -c -o $@ \
		$<


bin/Release/seqdacon_standalone: $(ALL_HH_FILES) $(ALL_OBJ_FILES_RELEASE)
		$(T23MFW_LINKER_RELEASE) $(SEQDACON_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_RELEASE) \
		$(SEQDACON_LIBS)

include/%.hh: %.cc
#include/%.hh: $(SEQDACON_CC_FILES)
	noah-cpp --fixed-path -o 'include/' $<

objs/Release/%.o: %.cc
	$(T23MFW_COMPILER_RELEASE) $(SEQDACON_COMPILE_FLAGS) -c -o $@ \
		$<


-include $(ALL_OBJ_FILES:.o=.d)
