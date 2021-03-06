#include <iostream>
#include <cstdio>
#include <string>

enum class MetaCommandResult{
    exit,
    empty,
    unrecognized,
    flush
};

class InputBuffer{
public:
    std::string buffer;

    InputBuffer() = default;

    void readInput(){
        std::getline(std::cin, buffer);
    }

    const char* str() const{
        return buffer.c_str();
    }

    bool isMetaCommand(){
        return (buffer.empty() || buffer[0] == '.');
    }

    MetaCommandResult performMetaCommand(){
        if(buffer == ".exit"){
            return MetaCommandResult::exit;
        }
        else if(buffer == ".flush"){
            return MetaCommandResult::flush;
        }
        else if(buffer.empty()){
            return MetaCommandResult::empty;
        }
        else{
            return MetaCommandResult::unrecognized;
        }
    }
};

inline void printPrompt(){
    printf("db > ");
}