#include "response.h"


RespType Response::getRespType() {
    return type;
}

std::vector<uint8_t> Response::serialize() {

    std::vector<uint8_t> response;

    if (type == RespType::Nil) {
        response.push_back('$');
        response.push_back('-');
        response.push_back('1');
        response.insert(response.end(), TERMINATOR.begin(), TERMINATOR.end());
        return response;
    }

    uint8_t header = respMap[type];
    response.push_back(header);

    if (type == RespType::SimpleString || type == RespType::SimpleError) {
        response.insert(response.end(), data.begin(), data.end());
    } else if (type == RespType::Integer) {
        std::string numStr = std::to_string(val);
        response.insert(response.end(), numStr.begin(), numStr.end());
    } else if (type == RespType::BulkString) {
        std::string lenStr = std::to_string(data.size());
        response.insert(response.end(), lenStr.begin(), lenStr.end());
        response.insert(response.end(), TERMINATOR.begin(), TERMINATOR.end());
        response.insert(response.end(), data.begin(), data.end());
    }

    response.insert(response.end(), TERMINATOR.begin(), TERMINATOR.end());
    return response;
}

