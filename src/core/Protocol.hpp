//
// Created by Jim Carter personal on 6/15/23.
//

#ifndef COLYSEUSCPP_PROTOCOL_HPP
#define COLYSEUSCPP_PROTOCOL_HPP

#include <cstdint>

enum class Protocol : int32_t
{
    // Room-related (10~19)
    HANDSHAKE = 9,
    JOIN_ROOM = 10,
    JOIN_ERROR = 11,
    LEAVE_ROOM = 12,
    ROOM_DATA = 13,
    ROOM_STATE = 14,
    ROOM_STATE_PATCH = 15,
    ROOM_DATA_SCHEMA = 16,
};

#endif //COLYSEUSCPP_PROTOCOL_HPP
