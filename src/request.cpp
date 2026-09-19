#include "request.h"

bool Request::setCommand(const std::string& token) {
    commandName = token;
    auto iterator = commandTable.find(token);
    if (iterator == commandTable.end()) return false;

    type = iterator->second.type;
    spec = &iterator->second;
    return true;
}

void Request::addArgument(std::string arg)
{
    args.emplace_back(arg);
}

void Request::setError(std::string message)
{
    errorMessage = std::move(message);
}

bool Request::hasError() const { return !errorMessage.empty(); }

void Request::setFatalError(std::string message) {
    errorMessage = std::move(message);
    fatal = true;
}

bool Request::isFatal() const { return fatal; }

const std::string& Request::getError() const { return errorMessage; }

const std::string& Request::getCommandName() const { return commandName; }

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
    spec = nullptr;
    commandName.clear();
    errorMessage.clear();
    fatal = false;
    args.clear();
}
