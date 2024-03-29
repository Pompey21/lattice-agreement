#include "udp_lattice.hpp"
#include "parser.hpp"
#include "shot.hpp"

#include <set>
#include <vector>
#include <mutex>

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
        bool is_decided();
        std::vector<std::string> get_decision_logs();


    private:
        UDPSocket udp_socket;
        bool active;
        int ack_count;
        int proposal_count;
        int nack_count;
        std::set<int> proposed_values;
        std::set<int> accepted_values;
        unsigned long number_of_neighbors;
        std::vector<Parser::Host> neighbors;
        std::mutex decided_lock;
        bool decided;
        std::vector<std::string> decision_logs;
};