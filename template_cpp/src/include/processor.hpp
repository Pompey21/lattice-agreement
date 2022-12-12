#include "udp_lattice.hpp"
#include "parser.hpp"

#include <set>
#include <vector>

class Processor {
    public:
        Processor(){};
        Processor(const Processor &);
        Processor(std::vector<Parser::Host> neighbors, Parser::Host localhost);
        // ~Processor();
        Processor& operator=(const Processor & other);

        void create();

        void propose(std::set<int> values);
        // void reception(bool is_proposal, bool is_ack, int proposal_num, std::set<int> values);
        void vibe_check();
        void send_message(std::string message, Parser::Host host);
        void reception();


    private:
        UDPSocket udp_socket;
        bool active;
        bool is_proposal;
        bool is_ack;
        int ack_count;
        // unsigned long ack_count;
        int proposal_count;
        // unsigned long proposal_count;
        int nack_count;
        // unsigned long nack_count;
        std::set<int> proposed_values;
        std::set<int> accepted_values;
        unsigned long number_of_neighbors;
        std::vector<Parser::Host> neighbors;
        std::vector<std::string> decisions;
};