T23SRC:=$(T23MFW_ROOT)/src

T23MFW_CC_FILES := $(wildcard $(T23SRC)/*.cc) $(wildcard $(T23SRC)/QuantTA/*.cc)
T23MFW_OBJ_FILES_DEVDBG := $(addprefix objs/DbgDev/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))
T23MFW_OBJ_FILES_DEVFAST := $(addprefix objs/DevFast/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))
T23MFW_OBJ_FILES_NOBELT := $(addprefix objs/NoBelt/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))


# # # # # # # # # DBGDEV # # # # # # # # # # # #
T23MFW_COMPILER_DBGDEV=ccache clang++ \
						-W -Wall -std=c++11 \
						-Werror=return-type \
						-g -O0 -pedantic \
						-march=native \
						-DIS_DEBUG -DIS_DEEPBUG \
#						-pthread
T23MFW_LINKER_DBGDEV=ccache clang++ -W -Wall -std=c++11 \
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
						-O3 -pedantic -march=native \
						-fomit-frame-pointer -ffast-math -flto \
						-funsafe-loop-optimizations \
						-fno-default-inline \
						-DIS_DEBUG -DIS_DEVFAST \
#						-fno-strict-overflow \
#						-pthread
T23MFW_LINKER_DEVFAST=ccache g++ -W -Wall -std=c++11 \
						-O3 -pedantic \
						-flto \
						-fno-default-inline \
						-DIS_DEBUG -DIS_DEVFAST
#						-pthread \


# # # # # # # # # NOBELT # # # # # # # # # # # #
T23MFW_COMPILER_NOBELT=ccache g++ \
						-W -Wall -std=c++11 \
						-Werror=return-type \
						-O3 -march=native \
						-fomit-frame-pointer -ffast-math -flto \
						-funsafe-loop-optimizations \
						-fno-default-inline \
						-DNDEBUG -DIS_NOBELT \
#						-fno-strict-overflow \
#						-pthread
T23MFW_LINKER_NOBELT=ccache g++ -W -Wall -std=c++11 \
						-flto \
						-fno-default-inline \
						-O3 -pedantic
#						-pthread


T23MFW_COMPILER_RELEASE=$(T23MFW_COMPILER_NOBELT)
T23MFW_LINKER_RELEASE=$(T23MFW_LINKER_NOBELT)
