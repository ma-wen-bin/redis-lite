#include <string>
#include <vector>
#include <stdint.h>
#include <unordered_map>
#include <cassert>

enum class RespType {
    SimpleString, 
    SimpleError,
    Integer,
    BulkString,
    Nil
};



class Response {
    private:
    RespType type;
    int val;
    std::string data;
    const std::vector<uint8_t> TERMINATOR = {'\r', '\n'};
    std::unordered_map<const RespType, uint8_t> respMap = {
        { RespType::SimpleString , '+' },
        { RespType::SimpleError , '-' },
        { RespType::Integer , ':' },
        { RespType::BulkString , '$' },
    };

    public:
    Response(RespType t, int num) : type(t), val(num) {
        assert(t == RespType::Integer && "int constructor is only valid for RespType::Integer");
    };
    Response(RespType t, std::string data) : type(t), data(std::move(data)) {
        assert(t != RespType::Integer && "RespType::Integer must use the int constructor, not string");
    };
    RespType getRespType();
    std::vector<uint8_t> serialize();
    

};