
T23SRC:=$(T23MFW_ROOT)/src
T23MFW_CC_FILES := $(wildcard $(T23SRC)/*.cc)
#T23MFW_CC_FILES := $(wildcard $(T23SRC)/*.cc) $(wildcard $(T23SRC)/QuantTA/*.cc)

# generate deps:
# clang++ -std=c++11 -MM -I/windzone/T5-test-strategy/include  SOME_FILE.cc

# T23MFW_CC_FILES := \
# 	Broker_DukascopySwizz.cc \
# 	EtcUtils.cc \
# 	HardSignal.cc \
# 	MutatingBuffer.cc \
# 	MutatingString.cc \
# 	QTABase.cc \
# 	QTA.cc \
# 	QTA_Ema.cc \
# 	QTA_Highest.cc \
# 	QTA_Lowest.cc \
# 	QTA_ObjectAbstract.cc \
# 	QTA_PeriodTickMean.cc \
# 	QuantBase.cc \
# 	QuantBasic_DESIGN_CHOICES.cc \
# 	QuantBasicTypes.cc \
# 	QuantBuffersBase.cc \
# 	QuantBuffersReverseIndexedCircular.cc \
# 	QuantBuffersSynchronizedBufferAbstract.cc \
# 	QuantBuffersSynchronizedBuffer.cc \
# 	QuantBuffersSynchronizedHeap.cc \
# 	QuantExecutionContext.cc \
# 	QuantExecutionRetroactive.cc \
# 	QuantExecution_standalone_main.cc \
# 	QuantFeedAbstract.cc \
# 	QuantFeed.cc \
# 	QuantInstrument.cc \
# 	QuantObjectManagers.cc \
# 	QuantPeriodizationAbstract.cc \
# 	QuantPeriodization.cc \
# 	QuantProfiling.cc \
# 	SQDI_DukascopyInterfaceCode.cc \
# 	SQDI_Ticks_Dukascopy.cc \
# 	QuantSequentialDataPrefetcher.cc \
# 	SQDI_QuledAbstract.cc \
# 	SQDI_SyntheticTriangleWaveDebug_Ticks.cc \
# 	SQDI_Ticks_T23M.cc \
# 	SQDI_Ticks_Abstract.cc \
# 	QuantSequentialDataUtils.cc \
# 	QuantStudyContextAbstract.cc \
# 	QuantStudyContextBase.cc \
# 	QuantSuit.cc \
# 	QuantSymbolsRegistry.cc \
# 	QuantTick.cc \
# 	QuantTime.cc \
# 	QuantTradeDesk.cc \
# 	rfx11_lib_debug.cc \
# 	rfx11_types.cc \
# 	Seqdacon_standalone_main.cc \
# 	SwiftSignal.cc


T23MFW_OBJ_FILES_DBGDEV := $(addprefix objs/DbgDev/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))
T23MFW_OBJ_FILES_DEVFAST := $(addprefix objs/DevFast/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))
T23MFW_OBJ_FILES_NOBELT := $(addprefix objs/NoBelt/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))


# # # # # # # # # DBGDEV # # # # # # # # # # # #
# T23MFW_COMPILER_DBGDEV=ccache clang++
T23MFW_COMPILER_DBGDEV=clang++ \
						-ferror-limit=1 \
						-fno-crash-diagnostics \
						-W -Wall -std=c++11 \
						-Werror=return-type \
						-g -O0 -pedantic \
						-march=native \
						-DIS_DEBUG -DIS_DEEPBUG \
#						-pthread
T23MFW_LINKER_DBGDEV=clang++ -W -Wall -std=c++11 \
						-g -O0 -pedantic \
						-DIS_DEBUG -DIS_DEEPBUG
#						-pthread \


# # # # # # # # # SYNTAX CHECK  # # # # # # # # # # # #
T23MFW_COMPILER_SYNTAX=clang++ \
						-fsyntax-only \
						-W -Wall -std=c++11 -Werror=return-type -g -O0 -pedantic -march=native \
						-DIS_DEBUG -DIS_DEEPBUG
T23MFW_LINKER_SYNTAX=foo


# # # # # # # # # DEVFAST # # # # # # # # # # # #
T23MFW_COMPILER_DEVFAST=ccache g++ \
						-W -Wall -std=c++11 \
						-Werror=return-type \
						-O2 -pedantic -march=native \
						-fomit-frame-pointer -ffast-math \
						-funsafe-loop-optimizations \
						-fno-default-inline \
						-DIS_DEBUG -DIS_DEVFAST \
#						-flto \
#						-fno-strict-overflow \
#						-pthread
T23MFW_LINKER_DEVFAST=g++ -W -Wall -std=c++11 \
						-O2 -pedantic \
						-fno-default-inline \

#						-DIS_DEBUG -DIS_DEVFAST
# 						-flto \
#						-pthread \


# # # # # # # # # NOBELT # # # # # # # # # # # #
T23MFW_COMPILER_NOBELT_GCC=ccache g++ \
						-W -Wall -std=c++11 \
						-Werror=return-type \
						-Ofast -march=native \
						-fomit-frame-pointer -ffast-math -flto \
						-funsafe-loop-optimizations \
						-fno-default-inline \
						-DNDEBUG -DIS_NOBELT \
#						-fno-strict-overflow \
#						-pthread
#						-O3 -march=native \

T23MFW_LINKER_NOBELT_GCC=g++ -W -Wall -std=c++11 \
						-flto \
						-fno-default-inline \
						-Ofast -pedantic
#						-O3 -pedantic
#						-pthread

# # # # # # # # # NOBELT # # # # # # # # # # # #
T23MFW_COMPILER_NOBELT_CLANG=ccache clang++ \
						-W -Wall -std=c++11 \
						-Werror=return-type \
						-Ofast -march=native \
						-fomit-frame-pointer -ffast-math -flto \
						-DNDEBUG -DIS_NOBELT \
#						-fno-strict-overflow \
#						-pthread
T23MFW_LINKER_NOBELT_CLANG=clang++ -v -W -Wall -std=c++11 \
						-flto \
						-Ofast -pedantic
#						-pthread


T23MFW_COMPILER_RELEASE=$(T23MFW_COMPILER_NOBELT)
T23MFW_LINKER_RELEASE=$(T23MFW_LINKER_NOBELT)
