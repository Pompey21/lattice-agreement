#include "parser.hpp"
#include <string>


struct Msg_Lattice {
    Parser::Host sender;
    Parser::Host receiver;
    unsigned long msg_id;
    std::string content;
    bool is_ack;
    public:
        bool operator==( const Msg_Lattice& other ) {
            if (other.is_ack) 
                return sender.ip == other.receiver.ip &&
                        sender.port == other.receiver.port &&
                        receiver.ip == other.sender.ip &&
                        receiver.port == other.sender.port &&
                        msg_id == other.msg_id;
            else
                return sender.ip == other.sender.ip &&
                        sender.port == other.sender.port &&
                        receiver.ip == other.receiver.ip &&
                        receiver.port == other.receiver.port &&
                        msg_id == other.msg_id;
    }
};