#pragma once

#include "udp_lattice.hpp"
#include "parser.hpp"

#include <set>
#include <vector>
#include <mutex>
#include <string>

class Shot {
    public:
        Shot(int shot_id, std::set<int> values);
        Shot(const Shot &);
        Shot& operator=(const Shot & other);

        bool active;
        std::string decision_log;
        int ack_count;
        int nack_count;
        int proposal_count;
        int shot_id;
        std::set<int> proposed_values;
        std::set<int> accepted_values;
        std::mutex decided_lock;
        bool decided;
};

