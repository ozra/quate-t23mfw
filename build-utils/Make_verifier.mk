include $(T23MFW_ROOT)/build-utils/Make_T23MFW.mk

VPATH=src/:$(T23SRC)

VERIFIER_LINK_FLAGS=-static

VERIFIER_COMPILE_FLAGS= $(CPPFLAGS) \
	-MMD \
	-Iinclude/ \
    -Iext/T23MFW/ext/dukascopy/lib/easylzma/src/ \
    -Iext/T23MFW/ext/dukascopy/include/ \
    -Iext/T23MFW/ext/boost_1_56_0/boost/ \

VERIFIER_LIBS=-Lext/T23MFW/ext/boost-libs/ \
			-lboost_system \
			-lboost_thread \
			-lboost_filesystem \
			-lboost_date_time \
			-lboost_program_options
#		-lboost_regex
#        ext/T23MFW/ext/easylzma-libs/libeasylzma_s.a \
#        ext/T23MFW/ext/dukascopy-libs/libninety47_dukascopy.a \
#        -lboost_container \


VERIFIER_CC_FILES=	\
			QuantBasic_DESIGN_CHOICES.cc \
			rfx11_types.cc \
			rfx11_lib_debug.cc \
			EtcUtils.cc \
			HardSignal.cc \
			MutatingString.cc \
			MutatingBuffer.cc \
			Profiling.cc \
			QuantBasicTypes.cc \
			QuantBase.cc \
			QuantTime.cc \
			QuantTick.cc \
			QuantBuffersBase.cc \
			QuantBuffersReverseIndexedCircular.cc \
			QuantProfiling.cc \
			QuantSequentialDataUtils.cc \
			QuantSequentialDataPrefetcher.cc \
			SQDI_BaseAbstract.cc \
			SQDI_Ticks_Abstract.cc \
			SQDI_Sys_T23M_Abstract.cc \
			SQDI_Sys_DukascopyFormat_FsInterface.cc \
			SQDI_Ticks_Dukascopy.cc \
			SQDI_Ticks_T23M.cc \
			verifier_main.cc


VERIFIER_OBJ_FILES_DBGDEV= $(addprefix objs/DbgDev/,$(notdir $(VERIFIER_CC_FILES:.cc=.o)))
VERIFIER_OBJ_FILES_RELEASE= $(addprefix objs/Release/,$(notdir $(VERIFIER_CC_FILES:.cc=.o)))

# ALL_CC_FILES= $(T23MFW_CC_FILES) $(VERIFIER_CC_FILES)
ALL_CC_FILES= $(VERIFIER_CC_FILES)

ALL_HH_FILES = $(addprefix include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))
#ALL_PCH_FILES = $(addprefix include/,$(notdir $(ALL_CC_FILES:.cc=.hh.pch)))

ALL_OBJ_FILES_DBGDEV= $(VERIFIER_OBJ_FILES_DBGDEV)
ALL_OBJ_FILES_RELEASE= $(VERIFIER_OBJ_FILES_RELEASE)

ALL_OBJ_FILES=$(ALL_OBJ_FILES_DBGDEV) $(ALL_OBJ_FILES_RELEASE)


all: dbgdev

clean:
	$(RM) include/* \
			bin/DbgDev/* bin/Release/* \
			objs/DbgDev/* objs/Release/*

dbgdev: bin/DbgDev/verifier_standalone

release: bin/Release/verifier_standalone

bin/DbgDev/verifier_standalone: $(ALL_PCH_FILES) $(ALL_HH_FILES) $(ALL_OBJ_FILES_DBGDEV)
		$(T23MFW_LINKER_DBGDEV) $(VERIFIER_LINK_FLAGS) \
		-o $@ \
		$(ALL_OBJ_FILES_DBGDEV) \
		$(VERIFIER_LIBS)

include/%.hh: %.cc
#include/%.hh: $(VERIFIER_CC_FILES)
	noah-cpp --fixed-path -o 'include/' $< \

#		&& clang -x c++-header $< -o $<.pth

#include/%.hh.pch: include/%.hh
#	clang++ -std=c++11 -relocatable-pch $< -o $<.pth


objs/DbgDev/%.o: %.cc
	$(T23MFW_COMPILER_DBGDEV) $(VERIFIER_COMPILE_FLAGS) -c -o $@ \
		$<


bin/Release/verifier_standalone: $(ALL_HH_FILES) $(ALL_OBJ_FILES_RELEASE)
		$(T23MFW_LINKER_RELEASE) $(VERIFIER_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_RELEASE) \
		$(VERIFIER_LIBS)

include/%.hh: %.cc
#include/%.hh: $(VERIFIER_CC_FILES)
	noah-cpp --fixed-path -o 'include/' $<

objs/Release/%.o: %.cc
	$(T23MFW_COMPILER_RELEASE) $(VERIFIER_COMPILE_FLAGS) -c -o $@ \
		$<


-include $(ALL_OBJ_FILES:.o=.d)
