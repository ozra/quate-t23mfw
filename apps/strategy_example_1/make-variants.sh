
# *TODO* add -DDESIGN_CHOICE__SOME_FLAGGING somehow !?

make devfast -j 3 CPPFLAGS=-DFOO_YA  &&
    mv bin/DevFast/strategy_standalone bin/DevFast/strategy_standalone_SPECBEND

make devfast -j 3 CPPFLAGS=-DDESIGN_CHOICE__N47_BIGENDIAN_DECODERS   &&
    mv bin/DevFast/strategy_standalone bin/DevFast/strategy_standalone_N47BEND

