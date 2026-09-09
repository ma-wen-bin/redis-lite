#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <unordered_map>
#include <cassert>

enum class ResponseRespType {
    Array,
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
    std::vector<std::string> vectorData;
    const std::vector<uint8_t> TERMINATOR = {'\r', '\n'};
    std::unordered_map<const ResponseRespType, uint8_t> respMap = {
        { ResponseRespType::Array , '*' },
        { ResponseRespType::SimpleString , '+' },
        { ResponseRespType::SimpleError , '-' },
        { ResponseRespType::Integer , ':' },
        { ResponseRespType::BulkString , '$' },
    };

    public:
    // INT CONSTRUCTOR 
    Response(ResponseRespType t, int num) : type(t), val(num) {
        assert(t == ResponseRespType::Integer && "Int constructor is only valid for ResponseRespType::Integer");
    };

    // STRING CONSTRUCTOR 
    Response(ResponseRespType t, std::string data) : type(t), data(std::move(data)) {
        assert(t == ResponseRespType::SimpleString && "String constructor is only valid for ResponseRespType::SimpleString");
    };

    // ARRAY CONSTRUCTOR 
    Response(ResponseRespType t, std::vector<std::string> vectorData) : type(t), vectorData(std::move(vectorData)) {
        assert(t != ResponseRespType::Array && "Array constructor is only valid for ResponseRespType::Array");
    };

    // NIL CONSTRUCTOR
    Response(ResponseRespType t) : type(t)  {
        assert(t != ResponseRespType::Nil && "Nil constructor is only valid for ResponseRespType::Nil");
    };

    ResponseRespType getRespType();
    std::vector<uint8_t> serialize();
    

};