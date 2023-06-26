//
// Created by Jim Carter personal on 6/26/23.
//

#ifndef COLYSEUSCPP_SCHEMAUTILS_HPP
#define COLYSEUSCPP_SCHEMAUTILS_HPP

#include <iostream>
#include <stdint.h>

#include <cstring>
#include <functional>
#include <vector>
#include <string>
#include <map>

#include <typeinfo>
#include <typeindex>

namespace colyseus
{
    namespace schema
    {

        using varint_t = float; // "number"
        using string = std::string;
        using float32_t = float;
        using float64_t = double;

        enum class OPERATION {
            // add new structure/primitive
            ADD = 128,

            // replace structure/primitive
            REPLACE = 0,

            // delete field
            DELETE = 64,

            // DELETE field, followed by an ADD
            DELETE_AND_ADD = 192, // 11100000

            // TOUCH is used to determine hierarchy of nested Schema structures during serialization.
            // touches are NOT encoded.
            TOUCH = 1, // 00000000

            // MapSchema Operations
            CLEAR = 10,
        };


        enum class SPEC : unsigned char
        {
            END_OF_STRUCTURE = 0xc1, // (msgpack spec: never used)
            NIL = 0xc0,
            INDEX_CHANGE = 0xd4,
        };

        enum class CODE: unsigned char
        {
            SWITCH_TO_STRUCTURE = 255,
            TYPE_ID = 213,
        };

        struct Iterator
        {
            int offset = 0;
        };

// template <typename T>
        struct DataChange
        {
            string field;
            // T value;
            // T previousValue;
        };

        inline bool IsLittleEndian()
        {
            int i = 1;
            return (int)*((unsigned char *)&i) == 1;
        }

        inline uint8_t uint8(std::vector<uint8_t>& bytes, Iterator* it) {
            return bytes[it->offset++];
        }

        inline uint8_t uint8(const uint8_t* bytes, Iterator* it) {
            return bytes[it->offset++];
        }

        inline int8_t int8(std::vector<uint8_t>& bytes, Iterator* it) {
            return static_cast<int8_t>(uint8(bytes, it));
        }

        inline uint16_t uint16(std::vector<uint8_t>& bytes, Iterator* it) {
            return bytes[it->offset++] | bytes[it->offset++] << 8;
        }

        inline uint16_t uint16(const uint8_t* bytes, Iterator* it) {
            return bytes[it->offset++] | bytes[it->offset++] << 8;
        }

        inline int16_t int16(std::vector<uint8_t>& bytes, Iterator* it) {
            return static_cast<int16_t>(uint16(bytes, it));
        }

        inline int32_t int32(std::vector<uint8_t>& bytes, Iterator* it) {
            return bytes[it->offset++] |
                   bytes[it->offset++] << 8 |
                   bytes[it->offset++] << 16 |
                   bytes[it->offset++] << 24;
        }

        inline int32_t int32(const uint8_t* bytes, Iterator* it) {
            return bytes[it->offset++] |
                   bytes[it->offset++] << 8 |
                   bytes[it->offset++] << 16 |
                   bytes[it->offset++] << 24;
        }

        inline uint32_t uint32(std::vector<uint8_t>& bytes, Iterator* it) {
            return static_cast<uint32_t>(int32(bytes, it));
        }

        inline uint32_t uint32(const uint8_t* bytes, Iterator* it) {
            return static_cast<uint32_t>(int32(bytes, it));
        }

        inline std::string utf8Read(std::vector<uint8_t>& bytes, std::size_t offset, std::size_t length) {
            std::string str = "";
            unsigned int chr = 0;
            for (std::size_t i = offset, end = offset + length; i < end; i++) {
                uint8_t byte = bytes[i];
                if ((byte & 0x80) == 0x00) {
                    str += static_cast<char>(byte);
                    continue;
                }
                if ((byte & 0xe0) == 0xc0) {
                    str += static_cast<char>(((byte & 0x1f) << 6) | (bytes[++i] & 0x3f));
                    continue;
                }
                if ((byte & 0xf0) == 0xe0) {
                    str += static_cast<char>(((byte & 0x0f) << 12) | ((bytes[i+1] & 0x3f) << 6) | ((bytes[i+2] & 0x3f) << 0));
                    i += 2;
                    continue;
                }
                if ((byte & 0xf8) == 0xf0) {
                    chr = ((byte & 0x07) << 18) | ((bytes[i+1] & 0x3f) << 12) | ((bytes[i+2] & 0x3f) << 6) | ((bytes[i+3] & 0x3f) << 0);
                    i += 3;
                    if (chr >= 0x010000) {
                        chr -= 0x010000;
                        str += static_cast<char>((chr >> 10) + 0xD800);
                        str += static_cast<char>((chr & 0x3FF) + 0xDC00);
                    } else {
                        str += static_cast<char>(chr);
                    }
                    continue;
                }

                std::cerr << "Invalid byte " << std::hex << static_cast<int>(byte) << std::endl;
            }
            return str;
        }

        inline std::string utf8Read(const uint8_t* bytes, std::size_t offset, std::size_t length) {
            std::vector<uint8_t> bytesVector(bytes + offset, bytes + offset + length);
            return utf8Read(bytesVector, 0, length);
        }

        inline std::string decodeString(const unsigned char bytes[], Iterator *it)
        {
            uint8_t prefix = bytes[it->offset++];
            std::size_t length = 0;

            const uint8_t* newBytes = static_cast<const uint8_t*>(bytes);

            if (prefix < 0xc0) {
                // fixstr
                length = prefix & 0x1f;
            }
            else if (prefix == 0xd9) {
                length = uint8(newBytes, it);
            }
            else if (prefix == 0xda) {
                length = uint16(bytes, it);
            }
            else if (prefix == 0xdb) {
                length = uint32(bytes, it);
            }

            std::string value = utf8Read(bytes, it->offset, length);
            it->offset += length;

            return value;
//
//            auto str_size = (bytes[it->offset] & 0x1f) + 1;
//            char *str = new char[str_size];
//            std::memcpy(str, bytes + it->offset + 1, str_size);
//            str[str_size - 1] = '\0'; // endl
//            it->offset += str_size;
//
//            string value(str);
//            delete[] str;
//            return value;
        }


        inline int8_t decodeInt8(const unsigned char bytes[], Iterator *it)
        {
            return (int8_t)(bytes[it->offset++] << 24 >> 24);
        }

        inline uint8_t decodeUint8(const unsigned char bytes[], Iterator *it)
        {
            return (uint8_t)bytes[it->offset++];
        }

        inline int16_t decodeInt16(const unsigned char bytes[], Iterator *it)
        {
            int16_t value = *(int16_t *)&bytes[it->offset];
            it->offset += 2;
            return value;
        }

        inline uint16_t decodeUint16(const unsigned char bytes[], Iterator *it)
        {
            uint16_t value = *(uint16_t *)&bytes[it->offset];
            it->offset += 2;
            return value;
        }

        inline int32_t decodeInt32(const unsigned char bytes[], Iterator *it)
        {
            int32_t value = *(int32_t *)&bytes[it->offset];
            it->offset += 4;
            return value;
        }

        inline uint32_t decodeUint32(const unsigned char bytes[], Iterator *it)
        {
            uint32_t value = *(uint32_t *)&bytes[it->offset];
            it->offset += 4;
            return value;
        }

        inline int64_t decodeInt64(const unsigned char bytes[], Iterator *it)
        {
            int64_t value = *(int64_t *)&bytes[it->offset];
            it->offset += 8;
            return value;
        }

        inline uint64_t decodeUint64(const unsigned char bytes[], Iterator *it)
        {
            uint64_t value = *(uint64_t *)&bytes[it->offset];
            it->offset += 8;
            return value;
        }

        inline float32_t decodeFloat32(const unsigned char bytes[], Iterator *it)
        {
            float32_t value = *(float32_t *)&bytes[it->offset];
            it->offset += 4;
            return value;
        }

        inline float64_t decodeFloat64(const unsigned char bytes[], Iterator *it)
        {
            float64_t value = *(float64_t *)&bytes[it->offset];
            it->offset += 8;
            return value;
        }

        inline varint_t decodeNumber(const unsigned char bytes[], Iterator *it)
        {
            auto prefix = bytes[it->offset++];
            std::cout << "decodeNumber, prefix => " << ((int)prefix) << std::endl;

            if (prefix < 0x80)
            {
                // positive fixint
                return (varint_t)prefix;
            }
            else if (prefix == 0xca)
            {
                // float 32
                return decodeFloat32(bytes, it);
            }
            else if (prefix == 0xcb)
            {
                // float 64
                return (varint_t) decodeFloat64(bytes, it);
            }
            else if (prefix == 0xcc)
            {
                // uint 8
                return (varint_t)decodeUint8(bytes, it);
            }
            else if (prefix == 0xcd)
            {
                // uint 16
                return (varint_t) decodeUint16(bytes, it);
            }
            else if (prefix == 0xce)
            {
                // uint 32
                return (varint_t) decodeUint32(bytes, it);
            }
            else if (prefix == 0xcf)
            {
                // uint 64
                return (varint_t) decodeUint64(bytes, it);
            }
            else if (prefix == 0xd0)
            {
                // int 8
                return (varint_t) decodeInt8(bytes, it);
            }
            else if (prefix == 0xd1)
            {
                // int 16
                return (varint_t) decodeInt16(bytes, it);
            }
            else if (prefix == 0xd2)
            {
                // int 32
                return (varint_t) decodeInt32(bytes, it);
            }
            else if (prefix == 0xd3)
            {
                // int 64
                return (varint_t) decodeInt64(bytes, it);
            }
            else if (prefix > 0xdf)
            {
                // negative fixint
                return (varint_t) ((0xff - prefix + 1) * -1);
            }
            else
            {
                return 0;
            }
        }

        inline bool decodeBoolean(const unsigned char bytes[], Iterator *it)
        {
            return decodeUint8(bytes, it) > 0;
        }

        inline bool numberCheck(const unsigned char bytes[], Iterator *it)
        {
            auto prefix = bytes[it->offset];
            return (prefix < 0x80 || (prefix >= 0xca && prefix <= 0xd3));
        }

        inline bool arrayCheck (const unsigned char bytes[], Iterator *it) {
            return bytes[it->offset] < 0xa0;
        }

        inline bool nilCheck(const unsigned char bytes[], Iterator *it) {
            return bytes[it->offset] == (unsigned char) SPEC::NIL;
        }

        inline bool indexChangeCheck(const unsigned char bytes[], Iterator *it) {
            return bytes[it->offset] == (unsigned char) SPEC::INDEX_CHANGE;
        }


    } // namespace schema
} // namespace colyseus

#endif //COLYSEUSCPP_SCHEMAUTILS_HPP
