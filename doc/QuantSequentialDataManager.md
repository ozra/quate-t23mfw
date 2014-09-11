/**
* Created:  2014-08-13
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

/*

A special sequential data wrapper should be used for loading the data. This can
handle the data that is still in another format, only in RAM and in TEMP /
CRASH-PROOF-files, until it is actually chunkified to a day-collection.qdbe on
disk.

Anything that can be rebuilt is NOT crash proof saved - just keep in RAM and
assume exception less running.

In the event of an exception an OUTER PROCESS : sync-sequential-quant-data - is
called. It syncs file in RAM-disk to HDD/SSD/NSD/Whatever.

The aim is for sequence-collections (files) to be about 64kB - seems like a
reasonable middleground of debris when small selection, and good-bit-of-chunk
in bigger selections.

*/

class QuantSeqData {
    virtual
}

class QuantSequence_Ticks : public QuantSeqData {
    columns_spec: bla bla
    keys_spec: bla bla
    locator_from_keys: bla bla
    virtual decode_block()
    virtual encode_block()
}



class QuantSeqDataTick_Dukascopy : public QuantSeqDataTick {
    columns_spec: bla bla
    keys_spec: bla bla
    locator_from_keys: bla bla
}



class QuantSeqDataTick_Oanda : public QuantSeqDataTick {
    columns_spec: bla bla
    keys_spec: bla bla
    locator_from_keys: bla bla
}
