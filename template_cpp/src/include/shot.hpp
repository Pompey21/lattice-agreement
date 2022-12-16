#include "udp_lattice.hpp"
#include "parser.hpp"

#include <set>
#include <vector>
#include <mutex>

class Shot {
    public:
        Shot(){};
        
    private:
        bool active;
        
}

// class Processor {

//     private:
//         UDPSocket udp_socket;
//         bool active;
//         bool is_proposal;
//         bool is_ack;
//         int ack_count;
//         // unsigned long ack_count;
//         int proposal_count;
//         // unsigned long proposal_count;
//         int nack_count;
//         // unsigned long nack_count;
//         std::set<int> proposed_values;
//         std::set<int> accepted_values;
//         unsigned long number_of_neighbors;
//         std::vector<Parser::Host> neighbors;
//         std::vector<std::string> decisions;
//         std::mutex decided_lock;
//         bool decided;
//         std::vector<std::string> decision_logs;
// };