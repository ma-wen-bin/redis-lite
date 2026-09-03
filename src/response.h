#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <unordered_map>
#include <cassert>

enum class ResponseRespType {
    SimpleString, 
    SimpleError,
    Integer,
    BulkString,
    Nil
};



class Response {
    private:
    ResponseRespType type;
    int val;
    std::string data;
    const std::vector<uint8_t> TERMINATOR = {'\r', '\n'};
    std::unordered_map<const ResponseRespType, uint8_t> respMap = {
        { ResponseRespType::SimpleString , '+' },
        { ResponseRespType::SimpleError , '-' },
        { ResponseRespType::Integer , ':' },
        { ResponseRespType::BulkString , '$' },
    };

    public:
    Response(ResponseRespType t, int num) : type(t), val(num) {
        assert(t == ResponseRespType::Integer && "int constructor is only valid for ResponseRespType::Integer");
    };
    Response(ResponseRespType t, std::string data) : type(t), data(std::move(data)) {
        assert(t != ResponseRespType::Integer && "ResponseRespType::Integer must use the int constructor, not string");
    };
    ResponseRespType getRespType();
    std::vector<uint8_t> serialize();
    

};