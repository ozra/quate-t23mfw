T23SRC:=$(T23MFW_ROOT)/src

T23MFW_CC_FILES := $(wildcard $(T23SRC)/*.cc) $(wildcard $(T23SRC)/QuantTA/*.cc)
T23MFW_OBJ_FILES_DEVDBG := $(addprefix objs/DbgDev/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))
T23MFW_OBJ_FILES_DEVFAST := $(addprefix objs/DevFast/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))
T23MFW_OBJ_FILES_NOBELT := $(addprefix objs/NoBelt/,$(notdir $(T23MFW_CC_FILES:.cc=.o)))


# # # # # # # # # DBGDEV # # # # # # # # # # # #
T23MFW_COMPILER_DBGDEV=clang++ \
						-W -Wall -std=c++11 \
						-g -O0 -pedantic \
						-march=native \
						-DIS_DEBUG -DIS_DEEPBUG \
						-MMD
#						-pthread \

T23MFW_LINKER_DBGDEV=clang++ -W -Wall -std=c++11 \
						-g -O0 -pedantic \
						-DIS_DEBUG -DIS_DEEPBUG
#						-pthread \


# # # # # # # # # DEVFAST # # # # # # # # # # # #
T23MFW_COMPILER_DEVFAST=g++ \
						-W -Wall -std=c++11 \
						-O2 -pedantic -march=native \
						-fomit-frame-pointer -ffast-math -flto \
						-fno-default-inline \
						-DIS_DEBUG -DIS_DEVFAST \
						-MMD
#						-pthread \

T23MFW_LINKER_DEVFAST=g++ -W -Wall -std=c++11 \
						-O2 -pedantic \
						-fomit-frame-pointer -ffast-math -flto \
						-fno-default-inline \
						-DIS_DEBUG -DIS_DEVFAST
#						-pthread \


# # # # # # # # # NOBELT # # # # # # # # # # # #
T23MFW_COMPILER_NOBELT=g++ \
						-W -Wall -std=c++11 \
						-O3 -march=native \
						-fomit-frame-pointer -ffast-math -flto \
						-DNDEBUG -DIS_NOBELT \
						-MMD
#						-pthread \

T23MFW_LINKER_NOBELT=g++ -W -Wall -std=c++11 \
						-fomit-frame-pointer -ffast-math -flto \
						-O3 -pedantic
#						-pthread \

