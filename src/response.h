#include <string>


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

    public:
    Response(RespType t, int num) : type(t), val(num) {};
    Response(RespType t, std::string data) : type(t), data(data) {};
    RespType getRespType();
    std::vector<uint8_t> serialize();
    

};