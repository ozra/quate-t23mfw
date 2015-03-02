include $(T23MFW_ROOT)/build-utils/Make_T23MFW.mk

VPATH=src/:$(T23SRC)

PRICEHUNTER_LINK_FLAGS=-static

PRICEHUNTER_COMPILE_FLAGS= $(CPPFLAGS) \
	-MMD \
	-Iinclude/ \
    -Iext/T23MFW/ext/dukascopy/lib/easylzma/src/ \
    -Iext/T23MFW/ext/dukascopy/include/ \
    -Iext/T23MFW/ext/boost_1_56_0/boost/ \

PRICEHUNTER_LIBS=-Lext/T23MFW/ext/boost-libs/ \
			-lboost_system \
			-lboost_thread \
			-lboost_filesystem \
			-lboost_date_time \
			-lboost_program_options
#		-lboost_regex
#        ext/T23MFW/ext/easylzma-libs/libeasylzma_s.a \
#        ext/T23MFW/ext/dukascopy-libs/libninety47_dukascopy.a \
#        -lboost_container \


PRICEHUNTER_CC_FILES=	\
			QuantBasic_DESIGN_CHOICES.cc \
			rfx11_types.cc \
			rfx11_lib_debug.cc \
			EtcUtils.cc \
			HardSignal.cc \
			MutatingString.cc \
			MutatingBuffer.cc \
			RequestUrl.cc \
			Profiling.cc \
			QuantBasicTypes.cc \
			QuantBase.cc \
			QuantTime.cc \
			QuantTick.cc \
			QuantBuffersBase.cc \
			QuantBuffersReverseIndexedCircular.cc \
			QuantProfiling.cc \
			QSDF_Utils.cc \
			QSDF_Prefetcher.cc \
			QSDF_BaseAbstract.cc \
			QSDF_Ticks_Abstract.cc \
			QSDF_Sys_T23M_Abstract.cc \
			QSDF_Sys_DukascopyFormat_FsInterface.cc \
			QSDF_Ticks_Dukascopy.cc \
			QSDF_Ticks_T23M.cc \
			QuantSymbolsRegistry.cc \
			QuantInstrument.cc \
			QuantBroker.cc \
			Broker_Huobi.cc \
			Broker_DukascopySwizz.cc \
			price-hunter-hub_main.cc


PRICEHUNTER_OBJ_FILES_DBGDEV= $(addprefix var/DbgDev/objs/,$(notdir $(PRICEHUNTER_CC_FILES:.cc=.o)))
PRICEHUNTER_OBJ_FILES_RELEASE= $(addprefix var/Release/objs/,$(notdir $(PRICEHUNTER_CC_FILES:.cc=.o)))

# ALL_CC_FILES= $(T23MFW_CC_FILES) $(PRICEHUNTER_CC_FILES)
ALL_CC_FILES= $(PRICEHUNTER_CC_FILES)

ALL_HH_FILES_DBGDEV = $(addprefix var/DbgDev/include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))
ALL_HH_FILES_RELEASE = $(addprefix var/Release/include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))

ALL_OBJ_FILES_DBGDEV= $(PRICEHUNTER_OBJ_FILES_DBGDEV)
ALL_OBJ_FILES_RELEASE= $(PRICEHUNTER_OBJ_FILES_RELEASE)

ALL_OBJ_FILES=$(ALL_OBJ_FILES_DBGDEV) $(ALL_OBJ_FILES_RELEASE)


all: dbgdev

clean:
	$(RM) 	bin/DbgDev/*.* bin/Release/*.* \
			var/DbgDev/objs/*.* var/Release/objs/*.* \
			var/DbgDev/include/*.* var/Release/include/*.*

dbgdev: bin/DbgDev/pricehunter_standalone

release: bin/Release/pricehunter_standalone

bin/DbgDev/pricehunter_standalone: $(ALL_HH_FILES_DBGDEV) $(ALL_OBJ_FILES_DBGDEV)
		$(T23MFW_LINKER_DBGDEV) $(PRICEHUNTER_LINK_FLAGS) \
		-o $@ \
		$(ALL_OBJ_FILES_DBGDEV) \
		$(PRICEHUNTER_LIBS)

var/DbgDev/include/%.hh: %.cc
#include/%.hh: $(PRICEHUNTER_CC_FILES)
	noah-cpp --fixed-path -o 'var/DbgDev/include/' $<

var/DbgDev/objs/%.o: %.cc
	$(T23MFW_COMPILER_DBGDEV) $(PRICEHUNTER_COMPILE_FLAGS) -c -o $@ \
		-Ivar/DbgDev/include/ \
		$<


bin/Release/pricehunter_standalone: $(ALL_HH_FILES_RELEASE) $(ALL_OBJ_FILES_RELEASE)
		$(T23MFW_LINKER_RELEASE) $(PRICEHUNTER_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_RELEASE) \
		$(PRICEHUNTER_LIBS)

var/Release/include/%.hh: %.cc
#include/%.hh: $(PRICEHUNTER_CC_FILES)
	noah-cpp --fixed-path -o 'var/Release/include/' $<

var/Release/objs/%.o: %.cc
	$(T23MFW_COMPILER_RELEASE) $(PRICEHUNTER_COMPILE_FLAGS) -c -o $@ \
		-Ivar/Release/include/ \
		$<


-include $(ALL_OBJ_FILES_RELEASE:.o=.d)
-include $(ALL_OBJ_FILES_DBGDEV:.o=.d)
