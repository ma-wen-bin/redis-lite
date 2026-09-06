#include "request.h"

void Request::setType(const RequestType &t)
{   
    type = t;
}

void Request::setType(std::string& t) {
    auto iterator = commandTable.find(t);
    if (iterator != commandTable.end()) {
        type = iterator->second.type;
        spec = &iterator->second;
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

std::vector<std::string> Request::getArguments() const
{
    return args;
}

RequestType Request::getType() const { return type; }

bool Request::isComplete() const
{   
    if (!spec) return false;
    return (spec->minArgs <= args.size() && args.size() <= spec->maxArgs);
}

void Request::reset() {
    type = RequestType::NONE;
    args.clear();
}
