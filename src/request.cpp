#include "request.h"

void Request::setType(const RequestType &t)
{   
    type = t;
}

void Request::setType(std::string& t) {
    auto iterator = Request::requestTypeMap.find(t);
    if (iterator != Request::requestTypeMap.end()) {
        type = iterator->second;
        return;
    }

    throw std::invalid_argument("Provided string does not match any valid request type!"); 
}

void Request::addArgument(std::string arg)
{
    args.emplace_back(arg);
}

std::string Request::getKey() const
{
    return (args.size() > 0) ? args[0] : "";
} 

std::string Request::getValue() const
{
    return (args.size() > 1) ? args[1] : "";
}

RequestType Request::getType() const { return type; }

std::string Request::getStringType() const 
{ 
    if (getType() == RequestType::GET) { return "GET" ;} 
    if (getType() == RequestType::SET) { return "SET" ;} 
    if (getType() == RequestType::DEL) { return "DEL" ;}
    if (getType() == RequestType::NONE) { return "NONE" ;} 
    return "";
}

bool Request::isComplete() const
{
    if (type == RequestType::SET)
        return args.size() == 2;
    if (type == RequestType::GET || type == RequestType::DEL)
        return args.size() == 1;
    return false;
}

void Request::reset() {
    type = RequestType::NONE;
    args.clear();
}


// void setRequest(RequestType& type, const std::string& key, const std::string& value) {
//     if (type != RequestType::SET) {
//         std::cerr << "Wrong type" << '\n';
//         return;
//     }

//     args.clear();
//     args.push_back(key);
//     args.push_back(value);
// }

// void setRequest(RequestType& type, std::string key) {
//     if (type == RequestType::GET || type == RequestType::DEL) {
//         args.clear();
//         args.push_back(key);
//     };
// }
