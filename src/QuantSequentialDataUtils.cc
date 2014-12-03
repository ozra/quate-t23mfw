#include "QuantSequentialDataUtils.hh"
#ifdef INTERFACE
/**
* Created:  2014-09-18
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/

#include <cstring>
#include <stdint.h>

#include "rfx11_types.hh"
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
inline auto read_varilen_natural( u8*& data ) -> T {
    T decoded_value = 0;
    int shift_amount = 0;

    do {
        decoded_value |= (T)(*data & 0x7F) << shift_amount;
        shift_amount += 7;
    } while ( (*data++ & 0x80) != 0 );

    return decoded_value;
}

template <typename T>
inline auto write_varilen_natural( u8*& buffer, T value ) -> u8* {
    do {
        u8 next_byte = value & 0x7F;
        value >>= 7;
        if (value)  // remainder non zero? Flag for one more byte
            next_byte |= 0x80;
        *buffer++ = next_byte;

    } while (value);

    return buffer;
}

/*
Handling of integers (signed numbers) are carried out in two steps:
 - zigzag the number in to an unsigned natural number
 / use the natural number varlen encoder. And vice versa for decode.
*/
template <typename T>
inline auto read_varilen_integer( u8*& data ) -> T {
    T unsigned_value = read_varilen_natural<T>( data );

    return ((unsigned_value & 1 ) ?
            ~(unsigned_value >> 1) :
            (unsigned_value >> 1)
    );
}

template <typename T>
inline auto write_varilen_integer( u8*& buffer, T value ) -> u8* {
    uint64_t uvalue;
    //T uvalue;
    uvalue = T( value < 0 ? ~(value << 1) : (value << 1) );
    return write_varilen_natural<T>( buffer, uvalue );
}


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
