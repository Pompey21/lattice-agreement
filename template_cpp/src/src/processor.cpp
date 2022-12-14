// #include <message_lattice.hpp>
#include <udp_lattice.hpp>
#include <set>
#include <processor.hpp>

#include<thread>
#include <chrono>         // std::chrono::seconds
#include <algorithm>
#include <set>
#include <vector>

//==================================================================================================
//==================================================================================================
/*
When preparing the message, we need to add the type of the message, the proposal number and the values
*/
static std::string prepare_message(std::set<int> values, int proposal_num, std::string type) {
    std::cout << "Prepare_message method()" << std::endl;
    std::string message = "";

    message = type + "," + std::to_string(proposal_num) + ",";
    
    for (std::set<int>::iterator it=values.begin(); it!=values.end(); ++it) {
        message = message + std::to_string(*it) + ",";
        std::cout << "value: " << *it << std::endl;
    }
    message.pop_back();

    std::cout << "------END-------" << std::endl;
    
    return message;
}

static std::string prepare_message_for_writing(std::set<int> values) {
    std::string message = "";
    for (std::set<int>::iterator it=values.begin(); it!=values.end(); ++it) {
        message = message + std::to_string(*it) + " ";
    }
    
    return message;
}

/*
When parsing the message, we need to obtain the type of the message, the proposal number and the values
*/
static std::vector<std::string> parse_message(std::string message) {
    std::vector<std::string> tokens;
    std::string delimiter = ",";
    size_t pos = 0;
    std::string token;
    while ((pos = message.find(delimiter)) != std::string::npos) {
        token = message.substr(0, pos);
        tokens.push_back(token);
        message.erase(0, pos + delimiter.length());
    }
    return tokens;
}

static std::vector<std::string> parse_message_2(std::string strToSplit)
{
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
    std::set<int> values = std::set<int>();
    for (std::vector<std::string>::iterator it=tokens.begin(); it!=tokens.end(); ++it) {
        values.insert(std::stoi(*it));
    }
    return values;
}

static std::string parse_message_get_type(std::vector<std::string> tokens) {
    return tokens[0];
}

static std::string parse_message_get_proposal_number(std::vector<std::string> tokens) {
    return tokens[1];
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
//==================================================================================================

Processor::Processor(std::vector<Parser::Host> neighbors, Parser::Host localhost) {
    // neighbors;
    // localhost;
    this->neighbors = neighbors;
    this->number_of_neighbors = neighbors.size();
    this->udp_socket = UDPSocket(localhost);

    std::cout << this->neighbors.size() << std::endl;

    // this->udp_socket.create();


    this->active = false;
    this->is_proposal = true;
    this->is_ack = false;
    this->ack_count = 0;
    this->nack_count = 0;
    this->proposal_count = 0;
    this->proposed_values = std::set<int>();
    this->accepted_values = std::set<int>();
    this->decisions = std::vector<std::string>();
    this->decided = false;
    this->decision_logs = std::vector<std::string>();
}

Processor& Processor::operator=(const Processor& other) {
    this->neighbors = other.neighbors;
    this->number_of_neighbors = other.number_of_neighbors;
    this->udp_socket = other.udp_socket;
    this->active = other.active;
    this->is_proposal = other.is_proposal;
    this->is_ack = other.is_ack;
    this->ack_count = other.ack_count;
    this->nack_count = other.nack_count;
    this->proposal_count = other.proposal_count;
    this->proposed_values = other.proposed_values;
    this->accepted_values = other.accepted_values;
    this->decisions = other.decisions;
    return *this;
}

// TODO : implement reception method of processor
void Processor::reception() {
    while (true) {
        Msg_Lattice msg = udp_socket.pop_buffer_received_messages();
        if (msg != Msg_Lattice()) {
            // TODO : parse message
            std::string message = msg.content;
            std::vector<std::string> message_parts = parse_message_2(message);

            std::cout << "message received: " << message << std::endl;
            std::cout << "length: " << message_parts.size() << std::endl;
            // std::cout << "message parts: " << message_parts[2] << std::endl;


            std::string message_type = parse_message_get_type(message_parts);
            std::cout << "message type: " << message_type << std::endl;

            std::string message_proposal_number = parse_message_get_proposal_number(message_parts);
            std::cout << "message proposal number: " << message_proposal_number << std::endl;

            std::cout << "=========" << std::endl;
            std::cout << "message parts: " << message_parts[2] << std::endl;
            std::cout << "message parts: " << message_parts[3] << std::endl;
            std::cout << "=========" << std::endl;


            std::set<int> sent_proposed_values = parse_message_get_numbers_2(message_parts); // these are the values
            std::cout << "number of numbers sent: " << sent_proposed_values.size() << std::endl;
            std::cout << "message proposed values: " << std::endl;
            for (std::set<int>::iterator it=sent_proposed_values.begin(); it!=sent_proposed_values.end(); ++it) {
                std::cout << *it << std::endl;
            }
            

            if (message_type == "PROPOSAL") {
                // if (std::includes(this->accepted_values.begin(), this->accepted_values.end(), sent_proposed_values.begin(), sent_proposed_values.end())) {
                if (std::includes(sent_proposed_values.begin(), sent_proposed_values.end(), this->accepted_values.begin(), this->accepted_values.end())) {
                    this->accepted_values = sent_proposed_values;
                    // TODO : send ACK
                    // prepare the message
                    std::string message = prepare_message(this->accepted_values, this->proposal_count, "ACK");
                    this->send_message(message, msg.sender);
                }
                else {
                    // this->accepted_values = std::set_union(this->accepted_values.begin(), this->accepted_values.end(), sent_proposed_values.begin(), sent_proposed_values.end(), this->accepted_values.begin());
                    this->accepted_values.merge(sent_proposed_values);
                    // TODO : send NACK
                    std::string message = prepare_message(this->accepted_values, this->proposal_count, "NACK");
                    this->send_message(message, msg.sender);
                }

            }
            else if (message_type == "ACK" && this->proposal_count == std::stoi(message_proposal_number) && this->active) {
                this->ack_count = this->ack_count + 1;
            }
            else if (message_type == "NACK" && this->proposal_count == std::stoi(message_proposal_number) && this->active) {
                this->nack_count = this->nack_count + 1;
                // this->proposed_values = std::set_union(this->proposed_values.begin(), this->proposed_values.end(), sent_proposed_values.begin(), sent_proposed_values.end(), this->proposed_values.begin());
                this->proposed_values.merge(sent_proposed_values);
            }
            this->vibe_check();
        }
    }
}

void Processor::create() {
    this->udp_socket.create();
    // TODO : create thread for reception

    std::thread receive_thread(&Processor::reception, this);

    // send_thread.detach(); 
    receive_thread.detach();
    
}

void Processor::propose(std::set<int> values) {
    this->proposed_values = values;
    this->active = true;
    this->proposal_count = this->proposal_count + 1;
    // TODO : send proposal to all neighbors
    std::string message = "";
    message = prepare_message(this->proposed_values, this->proposal_count, "PROPOSAL");

    std::cout << "Sending proposal" << std::endl;
    std::cout << message << std::endl;

    for (auto &host : this->neighbors) {
      udp_socket.enque(host, message);
    }
}


void Processor::send_message(std::string message, Parser::Host host) {
    this->udp_socket.enque(host, message);
}

void Processor::vibe_check() {
    if (this->nack_count > 0 && (this->ack_count+this->nack_count > static_cast<int>(this->number_of_neighbors)) && this->active) {
        this->proposal_count = this->proposal_count + 1;
        this->ack_count = 0;
        this->nack_count = 0;
        // TODO : send proposal to all neighbors
        std::string message = prepare_message(this->proposed_values, this->proposal_count, "PROPOSAL");
        for (auto &host : this->neighbors) {
          this->udp_socket.enque(host, message);
        }
    }
    else if (this->ack_count > static_cast<int>(this->number_of_neighbors/2) && this->active) {
        // TODO : decide on the values
        std::string message_to_write = prepare_message_for_writing(this->proposed_values);
        this->decisions.push_back(message_to_write);
        this->active = false;
        this->decided = true; 
        std::cout << "decided on: " << this->decided << std::endl;
        this->decision_logs.push_back(message_to_write);
    }
}

bool Processor::is_decided() {
    return this->decided;
}

std::vector<std::string> Processor::get_decision_logs() {
    return this->decision_logs;
}



