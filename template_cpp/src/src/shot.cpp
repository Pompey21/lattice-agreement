#include "shot.hpp"
#include "parser.hpp"

#include <set>
#include <vector>
#include <mutex>
#include <map>
#include <string>

Shot::Shot(const Shot & other){
    this->active = other.active;
    this->decision_log = other.decision_log;
    this->ack_count = other.ack_count;
    this->nack_count = other.nack_count;
    this->proposal_count = other.proposal_count;
    this->shot_id = other.shot_id;
    this->proposed_values = other.proposed_values;
    this->accepted_values = other.accepted_values;
    this->decided = other.decided;
}

Shot::Shot(int shot_id, std::set<int> values) {
    this->active = true;
    this->decision_log = "";
    this->ack_count = 0;
    this->nack_count = 0;
    this->proposal_count = 1;
    this->shot_id = shot_id;
    this->proposed_values = values;
    this->accepted_values = std::set<int>();
    this->decided = false;
}

Shot& Shot::operator=(const Shot & other){
    this->active = other.active;
    this->decision_log = other.decision_log;
    this->ack_count = other.ack_count;
    this->nack_count = other.nack_count;
    this->proposal_count = other.proposal_count;
    this->shot_id = other.shot_id;
    this->proposed_values = other.proposed_values;
    this->accepted_values = other.accepted_values;
    this->decided = other.decided;
    return *this;
}



