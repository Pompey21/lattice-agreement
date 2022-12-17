// #include <message_lattice.hpp>
#include <udp_lattice.hpp>
#include <set>
#include <processor_2.hpp>

#include<thread>
#include <chrono>         // std::chrono::seconds
#include <algorithm>
#include <set>
#include <vector>
#include <mutex>
#include <string>


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

//==================================================================================================
//==================================== PROCESSOR ===================================================

Processor::Processor(std::vector<Parser::Host> neighbors, Parser::Host localhost) {
    // neighbors;
    // localhost;
    this->neighbors = neighbors;
    this->number_of_neighbors = neighbors.size();
    this->udp_socket = UDPSocket(localhost);

    std::cout << this->neighbors.size() << std::endl;



    
}