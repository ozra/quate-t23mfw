include $(T23MFW_ROOT)/Make_T23MFW.mk


# *TODO* WTF. is VPATH? official variable or something wrongly named??  - 2014-09-12/ORC(01:51)
VPATH=src/:$(T23SRC)

STRATEGY_LINK_FLAGS= $(T23MFW_LINK_FLAGS)
STRATEGY_COMPILE_FLAGS= $(T23MFW_CC_COMPILE_FLAGS) \
	-Iinclude/ \
    -Iext/T23MFW/ext/dukascopy/lib/easylzma/src/ \
    -Iext/T23MFW/ext/dukascopy/include/ \
    -Iext/T23MFW/ext/boost_1_56_0/boost/ \


STRATEGY_CC_FILES= $(wildcard src/*.cc)
STRATEGY_OBJ_FILES= $(addprefix objs/,$(notdir $(STRATEGY_CC_FILES:.cc=.o)))

ALL_CC_FILES= $(T23MFW_CC_FILES) $(STRATEGY_CC_FILES)
ALL_OBJ_FILES= $(T23MFW_OBJ_FILES) $(STRATEGY_OBJ_FILES)
ALL_HH_FILES = $(addprefix include/,$(notdir $(ALL_CC_FILES:.cc=.hh)))


all: t23_strategy_standalone

clean:
	$(RM) objs/* include/* t23_strategy_standalone
#	$(RM) $(ALL_OBJ_FILES) $(ALL_HH_FILES) t23_strategy_standalone

# *TODO* make if here for dll/a/so/stand-alone
# *TODO* make if here for debug vs release (external libs: *_dbg.a or *.a )

t23_strategy_standalone: $(ALL_HH_FILES) $(ALL_OBJ_FILES)
	$(CC) $(STRATEGY_LINK_FLAGS) \
		-static \
		-o $@ $(ALL_OBJ_FILES) \
		-Lext/T23MFW/ext/boost-libs/ \
		-lboost_system \
		-lboost_thread \
		-lboost_filesystem \
		-lboost_date_time \
		-lboost_regex

#        ext/T23MFW/ext/easylzma-libs/libeasylzma_s.a \
#        ext/T23MFW/ext/dukascopy-libs/libninety47_dukascopy.a \

#        -lboost_container \

include/%.hh: %.cc
	noah-cpp --fixed-path -o 'include/' $<

objs/%.o: %.cc
	$(CC) $(STRATEGY_COMPILE_FLAGS) -c -o $@ $<




-include $(ALL_OBJ_FILES:.o=.d)
