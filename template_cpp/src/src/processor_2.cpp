// #include <message_lattice.hpp>
#include "udp_lattice.hpp"

#include "processor_2.hpp"

#include<thread>
#include <chrono>         // std::chrono::seconds
#include <algorithm>
#include <set>
#include <vector>
#include <mutex>
#include <string>
#include <set>


static std::string prepare_message(std::set<int> values, int proposal_num, std::string type, std::string shot_id) {
    std::string message = "";
    message = type + "," + shot_id + ',' + std::to_string(proposal_num) + ',';

    for (std::set<int>::iterator it = values.begin(); it != values.end(); ++it) {
        message += std::to_string(*it) + ',';
        std::cout << "value: " << *it << std::endl;
    }
    message.pop_back(); // just to remove the last column comma!

    return message;
}

static std::string prepare_message_for_writing(std::set<int> values) {
    std::string message = "";
    for (std::set<int>::iterator it = values.begin(); it != values.end(); ++it) {
        message = message + std::to_string(*it) + ' ';
    }
    return message;
}

static std::vector<std::string> parse_message(std::string message) {
    std::vector<std::string> tokens;
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    while ((pos = message.find(delimiter)) != std::string::npos) {
        token = message.substr(0,pos);
        tokens.push_back(token);
        message.erase(0, pos + delimiter.length());
    }
    return tokens;
}

static std::vector<std::string> parse_message_2(std::string strToSplit) {
    char delimeter = ',';
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}

static std::set<int> parse_message_get_numbers(std::vector<std::string> tokens) {
    tokens.erase(tokens.begin());
    tokens.erase(tokens.begin());
    tokens.erase(tokens.begin());
    std::set<int> values = std::set<int>();
    for (std::vector<std::string>::iterator it=tokens.begin(); it!=tokens.end(); ++it) {
        values.insert(std::stoi(*it));
    }
    return values;
}

static std::string parse_message_get_type(std::vector<std::string> tokens) {
    return tokens[0];
}

static std::string parse_message_get_shot_id(std::vector<std::string> tokens) {
    return tokens[1];
}

static int parse_message_get_proposal_num(std::vector<std::string> tokens) {
    return std::stoi(tokens[2]);
}

static std::set<int> parse_message_get_numbers_2(std::vector<std::string> strToSplit)
{
    strToSplit.erase(strToSplit.begin());
    strToSplit.erase(strToSplit.begin());
    std::set<int> result;
    int item;
    for (auto it = strToSplit.begin(); it != strToSplit.end(); ++it) {
        item = std::stoi(*it);
        // std::cout << "item: " << item << std::endl;
        result.insert(item);
    }
    return result;
}

//==================================================================================================
//==================================== PROCESSOR ===================================================

Processor::Processor(std::vector<Parser::Host> neighbors, Parser::Host localhost) {
    // neighbors;
    // localhost;
    this->neighbors = neighbors;
    this->number_of_neighbors = neighbors.size();
    this->udp_socket = UDPSocket(localhost);

    std::cout << this->neighbors.size() << std::endl;

    this->decided = false;
    this->decision_logs = std::vector<std::string>();

}

Processor& Processor::operator=(const Processor & other) {
    this->udp_socket = other.udp_socket;
    this->number_of_neighbors = other.number_of_neighbors;
    this->neighbors = other.neighbors;
    this->decided = other.decided;
    this->decision_logs = other.decision_logs;
    
    return *this;
}


// TODO : implement receiption method of processor
void Processor::reception() {
    while (true) {
        Msg_Lattice msg = udp_socket.pop_buffer_received_messages();
        if (msg != Msg_Lattice()) {
            // TODO : parse message
            std::string message = msg.content;
            std::vector<std::string> message_parts = parse_message_2(message);

            std::cout << "\nMessage received: " << message << std::endl;
            std::cout << "received from: " << msg.sender.id << std::endl;

            std::cout << "length: " << message_parts.size() << std::endl;

            std::string message_type = parse_message_get_type(message_parts);
            std::cout << "message type: " << message_type << std::endl;

            std::string message_proposal_number = parse_message_get_proposal_num(message_parts);
            std::cout << "message proposal number: " << message_proposal_number << std::endl;

            std::cout << "process' proposal number: " << this->proposal_number << std::endl;

            std::string shot_id = parse_message_get_shot_id(message_parts);
            std::cout << "shot id: " << shot_id << std::endl;

            std::set<int> sent_proposed_values = parse_message_get_numbers_2(message_parts); // these are the values
            
            // std::cout << "number of numbers sent: " << sent_proposed_values.size() << std::endl;
            std::cout << "message proposed values: " << std::endl;
            for (std::set<int>::iterator it=sent_proposed_values.begin(); it!=sent_proposed_values.end(); ++it) {
                std::cout << *it << " ";
            }

            std::cout << std::endl;
            std::cout << "accepted set: " << std::endl;
            std::set<int> accepted_vals = this->shots[shot_id].get_accepted_values();

            for (std::set<int>::iterator it=accepted_vals.begin(); it!=accepted_vals.end(); ++it) {
                std::cout << *it << " ";
            }

            std::cout << std::endl;

            if (message_type == "PROPOSAL") {
                if (std::includes(sent_proposed_values.begin(), sent_proposed_values.end(), accepted_vals.begin(), accepted_vals.end())) {
                    this->shots[shot_id].add_accepted_values(sent_proposed_values);
                    std::cout << "sending back an ACK" << std::endl;
                    // prepare the message
                    std::string message = prepare_message(this->shots[shot_id].get_accepted_values(), std::stoi(message_proposal_number), "ACK", shot_id);

                    this->send_message(message, msg.sender);
                }
                else {
                    // merge the values
                    this->shots[shot_it].merge_accepted_values(sent_proposed_values);
                    std::cout << "sending back an NACK" << std::endl;
                    // prepare the message
                    std::string message = prepare_message(this->shots[shot_id].get_accepted_values(), std::stoi(message_proposal_number), "NACK", shot_id);

                    this->send_message(message, msg.sender);
                }

            }
            else if (message_type == "ACK" && this->shots[shot_id].get_proposal_count() == std::stoi(message_proposal_number) && this->shots[shot_id].is_active()){
                std::cout << "ACK received - inside the if checker eeeeeeoooooo" << std::endl;
                // increment ack_count in Shot
                this->shots[shot_id].increment_ack_count();
            }
            else if (message_type == "NACK" && this->shots[shot_id].get_proposal_count() == std::stoi(message_proposal_number) && this->shots[shot_id].is_active()){
                // increment nack_count in Shot
                this->shots[shot_id].increment_nack_count();
                // merge proposed_values with sent_proposed_values
                this->shots[shot_id].merge_proposed_values(sent_proposed_values);
            }
            // TODO : vibe_check()
            std::cout << "Reception Completed\n" << std::endl;

        }
    }
}

void Processor::create() {
    this->udp_socket.create();

    std::thread receive_thread(&Processor::reception, this);

    receive_thread.detach();
}


void Processor::propose(std::vector<std::set<int>> multi_shots) {
    int id_counter = 1;
    for (auto shot : multi_shots) {
        Shot shot_object = Shot(id_counter, shot);
        this->shots[std::to_string(id_counter)] = shot_object;

        // TODO : send proposal to all neighbors
        std::cout << "sending proposal to all neighbors" << std::endl;

        std::string message = "";
        message = prepare_message(shot, this->shots[std::to_string(id_counter)].get_proposal_count(), "PROPOSAL", std::to_string(id_counter));

        std::cout << message << std::endl;

        for (auto &host : this->neighbors) {
            udp_socket.enque(host, message);
        }

        std::cout << "Proposal sent!" << std::endl;
        std::cout << "-------------\n" << std::endl;
    
    }
}

void Processor::send_message(std::string message, Parser::Host receiver) {
    this->udp_socket.enque(receiver, message);
}

void Processor::vibe_check(std::string shot_id) {
    // Shot shot = this->shots[shot_id];

    // std::cout << "-----------------" << std::endl;
    // std::cout << "Vibe Check!" << std::endl;
    // std::cout << "Proposal Count: " << shot.get_proposal_count() << std::endl;
    // std::cout << "Ack Count: " << shot.get_ack_count() << std::endl;
    // std::cout << "Nack Count: " << shot.get_nack_count() << std::endl;
    // std::cout << "-----------------\n" << std::endl;

    // int f_1 = static_cast<int>(this->number_of_neighbors/2)+1;
    // std::cout << "f+1: " << f_1 << std::endl;

    // if (shot.get_nack_count() > 0 && (shot.get_ack_count()+shot.get_nack_count()) >= f_1 && this->active) {
    //     std::cout << "just got here" << std::endl;
    //     shot.increment_proposal_count();
    //     shot.set_ack_count(0);
    //     shot.set_nack_count(0);
    //     // TODO : send proposal to all neighbors
    //     std::string message = prepare_message(shot.get_proposed_values(), shot.get_proposal_count(), "PROPOSAL", shot_id);
    //     for (auto &host : this->neighbors) {
    //       this->udp_socket.enque(host, message);
    //     }
    // }
    // else if (shot.get_ack_count() >= f_1 && shot.is_active()) {
    //     // TODO : decide on the values
    //     std::string message_to_write = prepare_message_for_writing(this->proposed_values);
    //     this->active = false;
    //     this->decided_lock.lock();
    //     this->decided = true; 
    //     this->decided_lock.unlock();
    //     std::cout << "decided on: " << this->decided << std::endl;
    //     this->decision_logs.push_back(message_to_write);
    //     this->proposal_count = this->proposal_count + 1;
    // }

    // // TODO : check if decided -> if not decided, then send proposal to all neighbors again
    // else if (!this->decided) {
    //     // TODO : send proposal to all neighbors BUT NOT ITSELF!
    //     std::string message = prepare_message(this->proposed_values, shot.get_proposal_count(), "PROPOSAL");
    //     for (auto &host : this->neighbors) {
    //         Parser::Host moi = this->udp_socket.get_localhost();
    //         if (host.ip != moi.ip) {
    //             this->udp_socket.enque(host, message);
    //         }
    //     }
    // }
}

bool Processor::is_decided() {
    // if num_shots == num_decided_shots
    int num_decided_shots = 0;
    for (auto shot : this->shots) {
        if (shot.is_decided()) {
            num_decided_shots++;
        }
    }
    if (num_decided_shots == this->shots.size()) {
        return true;
    }
    return false;
}



std::vector<std::string> Processor::get_decision_logs() {
    return this->decision_logs;
}