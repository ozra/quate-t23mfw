#include "QuantSequentialDataUtils.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <cstring>
#include <stdint.h>

#include "QuantBase.hh"
//#include "QuantTime.hh"
#include "QuantTick.hh"

typedef uint8_t u8;
typedef int8_t s8;
typedef const u8 c_u8;
typedef const s8 c_s8;


void cerrbug_a_buffer ( u8* byte_buffer, size_t byte_buffer_size );


template <typename T>
inline T readBigEndian32 ( c_u8 *buffer ) {
    T value;
    u8 *val_ptr = reinterpret_cast<u8 *>(&value);
    val_ptr[0] = buffer[3];
    val_ptr[1] = buffer[2];
    val_ptr[2] = buffer[1];
    val_ptr[3] = buffer[0];

    return value;
}

inline auto read_string ( u8*& data ) -> const char *  {
    const char * out_ptr = (const char *) data;
    data += std::strlen( (char *) data ) + 1;
    return out_ptr;
}

inline auto write_string ( u8*& data, const char* str ) -> u8* {
    std::strcpy( (char *) data, str );
    data += std::strlen( (char *) data ) + 1;
    return data;
}


/*
 * The following version of encoders/decoders modify the pointer naturally
 **/
template <typename T>
inline auto read_unsigned_only_varint( u8*& data ) -> T {
    T decoded_value = 0;
    int shift_amount = 0;

    do {
        decoded_value |= (T)(*data & 0x7F) << shift_amount;
        shift_amount += 7;
    } while ( (*data++ & 0x80) != 0 );

    return decoded_value;
}

// Encode an unsigned integer.  Returns number of encoded bytes.
// 'buffer' must have room for up to 10 bytes.
template <typename T>
inline auto write_unsigned_only_varint( u8*& buffer, T value ) -> u8* {
    // cerr << "write_unsigned_only_varint " << value << " in to " << (void*)buffer << "\n";
    do {
        u8 next_byte = value & 0x7F;
        value >>= 7;
        if (value)  // remainder non zero? Flag for one more byte
            next_byte |= 0x80;
        // cerr << "write part " << (int)next_byte << " in to " << (void*)buffer << " remainding val is " << value << "\n";
        *buffer++ = next_byte;

    } while (value);

    return buffer;
}


template <typename T>
inline auto read_varint( u8*& data ) -> T {
    T unsigned_value = read_unsigned_only_varint<T>( data );

    /*
    _DBG( T value = ((unsigned_value & 1 ) ?
            ~(unsigned_value >> 1) :
            (unsigned_value >> 1)
    ));
    _Dn("signed value read as unsigned" << unsigned_value << " to " << value);
    */

    /*
    return (T)(
        (unsigned_value & 1 ) ?
            ~(unsigned_value >> 1) :
            (unsigned_value >> 1)
    );
    */

    return ((unsigned_value & 1 ) ?
            ~(unsigned_value >> 1) :
            (unsigned_value >> 1)
    );
}

// Encode a signed 64-bit varint.  Works by first zig-zag transforming
// signed value into an unsigned value, and then reusing the unsigned
// encoder.  'buffer' must have room for up to 10 bytes.
template <typename T>
inline auto write_varint( u8*& buffer, T value ) -> u8* {
    uint64_t uvalue;
    //T uvalue;
    uvalue = T( value < 0 ? ~(value << 1) : (value << 1) );
    _Dn("signed value to write as: " << uvalue << " from: " << value);
    return write_unsigned_only_varint<T>( buffer, uvalue );
}


/*
 * The following version of encoders/decoders leave the pointer untouched and
 * sets the amount of bytes decoded / returns the number of bytes encoded.
 ** /
template <typename T>
inline T decode_unsigned_varint( c_u8* const data, int &decoded_bytes ) {
    int i = 0;
    T decoded_value = 0;
    int shift_amount = 0;

    do {
        decoded_value |= (T)(data[i] & 0x7F) << shift_amount;
        shift_amount += 7;
    } while ( (data[i++] & 0x80) != 0 );

    decoded_bytes = i;
    return decoded_value;
}

template <typename T>
inline T decode_signed_varint( c_u8 * const data, int &decoded_bytes ) {
    T unsigned_value = decode_unsigned_varint<T>(data, decoded_bytes);
    return (T)(
        (unsigned_value & 1 ) ?
            ~(unsigned_value >> 1) :
            (unsigned_value >> 1)
    );
}

// Encode an unsigned 64-bit varint.  Returns number of encoded bytes.
// 'buffer' must have room for up to 10 bytes.
template <typename T>
inline auto encode_unsigned_varint( u8 * const buffer, T value ) -> int {
    int encoded = 0;
    do {
        u8 next_byte = value & 0x7F;
        value >>= 7;

        if (value)
            next_byte |= 0x80;

        buffer[encoded++] = next_byte;

    } while (value);
    return encoded;
}

// Encode a signed 64-bit varint.  Works by first zig-zag transforming
// signed value into an unsigned value, and then reusing the unsigned
// encoder.  'buffer' must have room for up to 10 bytes.
template <typename T>
inline auto encode_signed_varint( u8 * const buffer, T value ) -> int {
    T uvalue;
    uvalue = T( value < 0 ? ~(value << 1) : (value << 1) );
    return encode_unsigned_varint<T>( buffer, uvalue );
}

*/

#endif

void cerrbug_a_buffer ( u8* byte_buffer, size_t byte_buffer_size ) {
    if ( byte_buffer_size < 20 )
            return;

    cerr << " AT BEGINNING:\n"
        << (int)byte_buffer[0] << ","
        << (int)byte_buffer[1] << ","
        << (int)byte_buffer[2] << ","
        << (int)byte_buffer[3] << ",\n"
        << (int)byte_buffer[4] << ","
        << (int)byte_buffer[5] << ","
        << (int)byte_buffer[6] << ","
        << (int)byte_buffer[7] << ",\n"
        << (int)byte_buffer[8] << ","
        << (int)byte_buffer[9] << ","
        << (int)byte_buffer[10] << ","
        << (int)byte_buffer[11] << ",\n"
        << (int)byte_buffer[12] << ","
        << (int)byte_buffer[13] << ","
        << (int)byte_buffer[14] << ","
        << (int)byte_buffer[15] << ",\n"
        << (int)byte_buffer[16] << ","
        << (int)byte_buffer[17] << ","
        << (int)byte_buffer[18] << ","
        << (int)byte_buffer[19] << "\n"

        << " AT END:\n"

        << (int)byte_buffer[byte_buffer_size - 19] << ","
        << (int)byte_buffer[byte_buffer_size - 18] << ","
        << (int)byte_buffer[byte_buffer_size - 17] << ","
        << (int)byte_buffer[byte_buffer_size - 16] << ",\n"
        << (int)byte_buffer[byte_buffer_size - 15] << ","
        << (int)byte_buffer[byte_buffer_size - 14] << ","
        << (int)byte_buffer[byte_buffer_size - 13] << ","
        << (int)byte_buffer[byte_buffer_size - 12] << ",\n"
        << (int)byte_buffer[byte_buffer_size - 11] << ","
        << (int)byte_buffer[byte_buffer_size - 10] << ","
        << (int)byte_buffer[byte_buffer_size - 9] << ","
        << (int)byte_buffer[byte_buffer_size - 8] << ",\n"
        << (int)byte_buffer[byte_buffer_size - 7] << ","
        << (int)byte_buffer[byte_buffer_size - 6] << ","
        << (int)byte_buffer[byte_buffer_size - 5] << ","
        << (int)byte_buffer[byte_buffer_size - 4] << ",\n"
        << (int)byte_buffer[byte_buffer_size - 3] << ","
        << (int)byte_buffer[byte_buffer_size - 2] << ","
        << (int)byte_buffer[byte_buffer_size - 1] << ","
        << (int)byte_buffer[byte_buffer_size - 0]

        << "\n\n";
}
