#pragma once

#include "udp_lattice.hpp"
#include "parser.hpp"
#include "shot.hpp"

#include <set>
#include <vector>
#include <mutex>
#include <map>
#include <string>

class Processor {
    public:
        Processor(){};
        Processor(const Processor &);
        Processor(std::vector<Parser::Host> neighbors, Parser::Host localhost, std::map<int, Shot> shots);
        Processor& operator=(const Processor & other);

        void create();
        void propose(Shot shot);
        void vibe_check(Shot shot);
        void reception();
        bool is_decided();
        void send_message(std::string message, Parser::Host host);

        std::vector<std::string> get_decision_logs();

    private:
        UDPSocket udp_socket;
        unsigned long number_of_neighbors;
        std::vector<Parser::Host> neighbors;

        std::map<int, Shot> shots;
        std::mutex decided_lock;
        bool decided;
        std::vector<std::string> decision_logs;
};