#include "udp_lattice.hpp"
#include "parser.hpp"

#include <set>
#include <vector>
#include <mutex>
#include <string>

class Shot {
    public:
        Shot(){};
        Shot(const Shot &);
        Shot& operator=(const Shot & other);

        void send_message(std::string message, Parser::Host host);
        bool is_decided();
        std::string get_decision_log();
        
    private:
        bool active;
        std::string decision_log;
        int ack_count;
        int nack_count;
        int proposal_count;
        int shot_id;
        std::set<int> proposed_values;
        std::set<int> accepted_values;
        unsigned long number_of_neighbors;
        std::vector<Parser::Host> neighbors;
        std::mutex decided_lock;
        bool decided;
        std::vector<std::string> decision_logs;
};

