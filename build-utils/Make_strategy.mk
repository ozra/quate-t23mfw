include $(T23MFW_ROOT)/build-utils/Make_T23MFW.mk

# T23MFW_COMPILER_NOBELT=$(T23MFW_COMPILER_NOBELT_CLANG)
# T23MFW_LINKER_NOBELT=$(T23MFW_LINKER_NOBELT_CLANG)
T23MFW_COMPILER_NOBELT=$(T23MFW_COMPILER_NOBELT_GCC)
T23MFW_LINKER_NOBELT=$(T23MFW_LINKER_NOBELT_GCC)


VPATH=src/:$(T23SRC):$(T23SRC)/QTA/src/

ASTYLING=astyle --style=otbs --indent=spaces=4 --delete-empty-lines --break-closing-brackets --attach-namespaces --indent-modifiers --indent-preproc-block --indent-preproc-define --indent-preproc-cond --min-conditional-indent=0 --max-instatement-indent=40 --pad-oper --pad-header --unpad-paren --align-pointer=middle --align-reference=middle --add-one-line-brackets --convert-tabs --close-templates --max-code-length=79 --lineend=linux
# --break-blocks=all

STRATEGY_LINK_FLAGS=-static

# 	-nostdinc++ \
# 	-H \

STRATEGY_COMPILE_FLAGS= $(CPPFLAGS) \
	-MMD \
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
# STRATEGY_HH_FILES_DEVFAST= $(addprefix objs/DevFast/include/,$(notdir $(STRATEGY_CC_FILES:.cc=.hh)))
STRATEGY_OBJ_FILES_DBGDEV= $(addprefix objs/DbgDev/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))
STRATEGY_OBJ_FILES_DEVFAST= $(addprefix objs/DevFast/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))
STRATEGY_OBJ_FILES_NOBELT= $(addprefix objs/NoBelt/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))

ALL_CC_FILES= $(T23MFW_CC_FILES) $(STRATEGY_CC_FILES)
ALL_HH_FILES_DBGDEV = $(addprefix objs/DbgDev/include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))
ALL_HH_FILES_DEVFAST = $(addprefix objs/DevFast/include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))
ALL_HH_FILES_NOBELT = $(addprefix objs/NoBelt/include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))
ALL_OBJ_FILES_DBGDEV= $(T23MFW_OBJ_FILES_DBGDEV) $(STRATEGY_OBJ_FILES_DBGDEV)
ALL_OBJ_FILES_DEVFAST= $(T23MFW_OBJ_FILES_DEVFAST) $(STRATEGY_OBJ_FILES_DEVFAST)
ALL_OBJ_FILES_NOBELT= $(T23MFW_OBJ_FILES_NOBELT) $(STRATEGY_OBJ_FILES_NOBELT)

ALL_OBJ_FILES=$(ALL_OBJ_FILES_DBGDEV) $(ALL_OBJ_FILES_DEVFAST) $(ALL_OBJ_FILES_NOBELT)


all: dbgdev
devfast: bin/DevFast/strategy_standalone
#devfast_lib: objs/DevFast/libt23m.a
#syntax: bin/DbgDev/strategy_standalone_syntax
dbgdev: bin/DbgDev/strategy_standalone
nobelt: bin/NoBelt/strategy_standalone
clean:
	$(RM)   bin/DbgDev/* bin/DevFast/* bin/NoBelt/* \
			objs/DbgDev/*.* objs/DevFast/*.* objs/NoBelt/*.*
			objs/DbgDev/include/*.* objs/DevFast/include/*.* objs/NoBelt/include/*.*


# syntax: bin/DbgDev/strategy_standalone_syntax
# bin/DbgDev/strategy_standalone_syntax: $(ALL_HH_FILES) $(ALL_OBJ_FILES_DBGDEV)
# 	$(T23MFW_LINKER_DBGDEV) $(STRATEGY_LINK_FLAGS) \
# 		-o $@ \
# 		$(ALL_OBJ_FILES_DBGDEV) \
# 		$(STRATEGY_LIBS)

syntax: $(ALL_HH_FILES_DBGDEV) $(ALL_OBJ_FILES_DBGDEV)
objs/DbgDev/include/%.hh: %.cc
	$(ASTYLING) $< \
	&& noah-cpp --fixed-path -o 'objs/DbgDev/include/' $<
objs/DbgDev/%.o: %.cc
	$(T23MFW_COMPILER_SYNTAX) $(STRATEGY_COMPILE_FLAGS) -Iobjs/DbgDev/include/ -c -o $@ \
		$<


bin/DbgDev/strategy_standalone: $(ALL_HH_FILES_DBGDEV) $(ALL_OBJ_FILES_DBGDEV)
	$(T23MFW_LINKER_DBGDEV) $(STRATEGY_LINK_FLAGS) \
		-o $@ \
		$(ALL_OBJ_FILES_DBGDEV) \
		$(STRATEGY_LIBS) \
	&& rm -rf src/*.orig ext/T23MFW/src/*.orig

objs/DbgDev/include/%.hh: %.cc
	$(ASTYLING) $< \
	&& noah-cpp --fixed-path -o 'objs/DbgDev/include/' $<

objs/DbgDev/%.o: %.cc
	$(T23MFW_COMPILER_DBGDEV) $(STRATEGY_COMPILE_FLAGS) -Iobjs/DbgDev/include/ -c -o $@ \
		$<


bin/DevFast/strategy_standalone: $(ALL_HH_FILES_DEVFAST) $(ALL_OBJ_FILES_DEVFAST)
		$(T23MFW_LINKER_DEVFAST) $(STRATEGY_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_DEVFAST) \
		$(STRATEGY_LIBS) \
	&& rm -rf src/*.orig ext/T23MFW/src/*.orig

objs/DevFast/include/%.hh: %.cc
	$(ASTYLING) $< \
	&& noah-cpp --fixed-path -o 'objs/DevFast/include/' $<
objs/DevFast/%.o: %.cc
	$(T23MFW_COMPILER_DEVFAST) $(STRATEGY_COMPILE_FLAGS) -Iobjs/DevFast/include/ -c -o $@ \
		$<

# bin/DevFast/strategy_standalone: objs/DevFast/libt23m.a $(STRATEGY_HH_FILES_DEVFAST) $(STRATEGY_OBJ_FILES_DEVFAST)
# 	$(T23MFW_LINKER_DEVFAST) $(STRATEGY_LINK_FLAGS) \
# 		-o $@ $(ALL_OBJ_FILES_DEVFAST) \
# 		$(STRATEGY_LIBS)
# objs/DevFast/include/%.hh: %.cc
# 	noah-cpp --fixed-path -o 'objs/DevFast/include/' $<
# objs/DevFast/%.o: %.cc
# 	$(T23MFW_COMPILER_DEVFAST) $(STRATEGY_COMPILE_FLAGS) -Iobjs/DevFast/include/ -c -o $@ \
# 		$<
#
# objs/DevFast/libt23m.a: $(ALL_HH_FILES_DEVFAST) $(ALL_OBJ_FILES_DEVFAST)
# 	ar x $(STRATEGY_LIBS) && ar rcs $@ \
# 		$(ALL_OBJ_FILES_DEVFAST) \
# 		$(the fuckin o's from the fuckin STRATEGY_LIBS)



bin/NoBelt/strategy_standalone: $(ALL_HH_FILES_NOBELT) $(ALL_OBJ_FILES_NOBELT)
	$(T23MFW_LINKER_NOBELT) $(STRATEGY_LINK_FLAGS) \
		-o $@ $(ALL_OBJ_FILES_NOBELT) \
		$(STRATEGY_LIBS) \
	&& rm -rf src/*.orig ext/T23MFW/src/*.orig

objs/NoBelt/include/%.hh: %.cc
	$(ASTYLING) $< \
	&& noah-cpp --fixed-path -o 'objs/NoBelt/include/' $<
objs/NoBelt/%.o: %.cc
	$(T23MFW_COMPILER_NOBELT) $(STRATEGY_COMPILE_FLAGS) -Iobjs/NoBelt/include/ -c -o $@ \
		$<


-include $(ALL_OBJ_FILES:.o=.d)
