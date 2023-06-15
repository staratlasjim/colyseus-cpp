#include <iostream>
#include "src/core/schema.h"
#include "src/core/PrimitiveTypes.h"

#include "src/poco/Axios.hpp"

#include <assert.h>
#include <stdio.h>
#include <string>

#include <Poco/Exception.h>


int main() {
    std::cout << "Hello, World!" << std::endl;

    const unsigned char encodedState[] = {0, 128, 1, 255, 2, 0, 128, 3, 255, 255, 4, 0, 0, 0, 128, 5, 255, 255, 255,
                                          255, 6, 0, 0, 0, 0, 0, 0, 0, 128, 7, 255, 255, 255, 255, 255, 255, 31, 0, 8,
                                          255, 255, 127, 255, 9, 255, 255, 255, 255, 255, 255, 239, 127, 10, 208, 128,
                                          11, 204, 255, 12, 209, 0, 128, 13, 205, 255, 255, 14, 210, 0, 0, 0, 128, 15,
                                          203, 0, 0, 224, 255, 255, 255, 239, 65, 16, 203, 0, 0, 0, 0, 0, 0, 224, 195,
                                          17, 203, 255, 255, 255, 255, 255, 255, 63, 67, 18, 203, 61, 255, 145, 224,
                                          255, 255, 239, 199, 19, 203, 255, 255, 255, 255, 255, 255, 239, 127, 20, 171,
                                          72, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 21, 1};

    std::unique_ptr<PrimitiveTypes> p = std::make_unique<PrimitiveTypes>();

    std::cerr << "============ about to decode\n";
    p->decode(encodedState, sizeof(encodedState) / sizeof(unsigned char));
    std::cerr << "============ decoded ================================================================ \n";

    std::cout << "p.int8 " << p->int8 << "\n";
    std::cout << "p.uint8 " << p->uint8 << "\n";
    std::cout << "p.int16 " << p->int16 << "\n";
    std::cout << "p.uint16 " << p->uint16 << "\n";
    std::cout << "p.int32 " << p->int32 << "\n";
    std::cout << "p.uint32 " << p->uint32 << "\n";
    std::cout << "p.int64 " << p->int64 << "\n";
    std::cout << "p.uint64 " << p->uint64 << "\n";
    std::cout << "p.float32 " << p->float32 << "\n";
    std::cout << "p.float64 " << p->float64 << "\n";
    std::cout << "p.varint_int8 " << p->varint_int8 << "\n";
    std::cout << "p.varint_uint8 " << p->varint_uint8 << "\n";
    std::cout << "p.varint_int16 " << p->varint_int16 << "\n";
    std::cout << "p.varint_uint16 " << p->varint_uint16 << "\n";
    std::cout << "p.varint_int32 " << p->varint_int32 << "\n";
    std::cout << "p.varint_uint32 " << p->varint_uint32 << "\n";
    std::cout << "p.varint_int64 " << p->varint_int64 << "\n";
    std::cout << "p.varint_uint64 " << p->varint_uint64 << "\n";
    std::cout << "p.varint_float32 " << p->varint_float32 << "\n";
    std::cout << "p.varint_float64 " << p->varint_float64 << "\n";
    std::cout << "p.str " << p->str << "\n";
    std::cout << "p.boolean " << p->boolean << "\n";

    std::cout << std::endl;


    std::cout << "~~~ Going into http mode " << std::endl;

//    test.LoadGoogle();

//    try {
//        Axios axios;
//        ptree data;
//        data.put("password", "xa123");
//        Axios::Response res = axios.post("http://localhost:2567/matchmake/joinOrCreate/dummy", data);
//
////        res.json.get
//    } catch (Poco::Exception &e) {
//        // Handle JSON parsing errors
//        std::cout << e.message() << std::endl;
//    }



//    ws = WebSocket::from_url("ws://localhost:2567/foo");
//    assert(ws);
//    ws->send("goodbye");
//    ws->send("hello");
//    while (ws->getReadyState() != WebSocket::CLOSED) {
//        ws->poll();
//        ws->dispatch(handle_message);
//    }
//    delete ws;

    return 0;
}
/*
 find_package(Boost REQUIRED)
target_link_libraries(colyseus_cpp PRIVATE Boost::boost)
 */

