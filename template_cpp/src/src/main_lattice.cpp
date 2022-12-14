#include <chrono>
#include <iostream>
#include <set>
#include <thread>
#include <string>
#include <sstream>
#include <vector>
#include <sstream>

#include "parser.hpp"
#include "udp_lattice.hpp"
// #include "beb.hpp"
#include "hello.h"
#include <signal.h>
#include "processor.hpp"

#define ALL(x) x.begin(), x.end()
#define INS(x) inserter(x,x.begin())

std::ofstream outputFile;
UDPSocket udpSocket;
// BEBSocket bebSocket;
Processor processor;

static void stop(int) {
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";

  // write/flush output file if necessary
  std::cout << "Writing output.\n";

  // for(auto const &output: udpSocket.get_logs()){
  //   outputFile << output << "\n" ;
  // }
  // outputFile.close();

  for(auto const &output: processor.get_decision_logs()){
    outputFile << output << "\n" ;
  }
  outputFile.close();
  
  // exit directly from signal handler
  exit(0);
}

static std::vector<int> split(std::string str) {
    std::vector<int> result;
    std::string word = "";
    for (auto x : str) {
        if (x == ' ') {
            // std::cout << word << std::endl;
            word = "";
        }
        else {
            word = word + x;
            int num = std::stoi(word);
            result.push_back(num);
        }
    }
    // std::cout << word << std::endl;
    return result;
}

std::set<int> split_4(std::string strToSplit)
{
    char delimeter = ',';
    std::stringstream ss(strToSplit);
    std::string item;
    std::set<int> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.insert(std::stoi(item));
    }
    return splittedStrings;
}




static std::vector<std::vector<int>> read_config(std::ifstream config_file) {
  std::vector<std::vector<int>> lines_vector_form;
  if (config_file.is_open())
    {
        std::string line;

        while(getline(config_file, line))
       {
            if(line.size() > 0){
                // vecOfStrs.push_back(str);
                // std::cout << line << std::endl;
                std::vector<int> line_vector_form = split(line);
                lines_vector_form.push_back(line_vector_form);
            }
        }
    }
    else 
    {
        std::cerr << "Couldn't open config file for reading.\n";
    }
    return lines_vector_form;
}

static void get_criteria(std::vector<std::vector<int>> criteria){
    std::vector<int> criteria_criteria = criteria[0];
    int number_of_processes = criteria_criteria[0];
    int max_number_of_messages_per_process = criteria_criteria[1];
    int max_number_of_messages_per_round = criteria_criteria[2];
}

static std::vector<int> get_numbers(std::vector<std::vector<int>> numbers){
    std::vector<int> numbers_criteria = numbers[1];
    return numbers_criteria;
}



int main(int argc, char **argv) {
  signal(SIGTERM, stop);
  signal(SIGINT, stop);

  // `true` means that a config file is required.
  // Call with `false` if no config file is necessary.
  bool requireConfig = true;
  unsigned long m,i;

  // Parse the arguments
  Parser parser(argc, argv);
  parser.parse();

  // hello();
  std::cout << std::endl;

//==================================================================================================
// This was already here
//==================================================================================================
  std::cout << "My PID: " << getpid() << "\n";
  std::cout << "From a new terminal type `kill -SIGINT " << getpid() << "` or `kill -SIGTERM "
            << getpid() << "` to stop processing packets\n\n";

  std::cout << "My ID: " << parser.id() << "\n\n";

  std::cout << "List of resolved hosts is:\n";
  std::cout << "==========================\n";
  std::vector<Parser::Host> hosts = parser.hosts();
  for (auto &host : hosts) {
    std::cout << host.id << "\n";
    std::cout << "Human-readable IP: " << host.ipReadable() << "\n";
    std::cout << "Machine-readable IP: " << host.ip << "\n";
    std::cout << "Human-readbale Port: " << host.portReadable() << "\n";
    std::cout << "Machine-readbale Port: " << host.port << "\n";
    std::cout << "\n";
  }
  std::cout << "\n";

  std::cout << "Path to output:\n";
  std::cout << "===============\n";
  std::cout << parser.outputPath() << "\n\n";
  outputFile.open(parser.outputPath());

  std::cout << "Path to config:\n";
  std::cout << "===============\n";
  std::cout << parser.configPath() << "\n\n";

  std::cout << "Doing some initialization...\n\n";

  std::cout << "Broadcasting and delivering messages...\n\n";

  std::ifstream config_file(parser.configPath());

  std::vector<std::string> numbers_strs;
  if (config_file.is_open())
  {
      std::string line;

      while(getline(config_file, line))
     {
          if(line.size() > 0){
              // vecOfStrs.push_back(str);
              // std::cout << line << std::endl;
              numbers_strs.push_back(line);
          }
      }
  }
  else 
  {
      std::cerr << "Couldn't open config file for reading.\n";
  }

  std::string system_criteria = numbers_strs[0];
  numbers_strs = std::vector<std::string>(numbers_strs.begin()+1, numbers_strs.end());
  std::string numbers = numbers_strs[0];
// ===================================================================================================

  // For Perfect Links:
  // config_file >> m >> i;

  // For Best Effort Broadcast:
  // config_file >> m;


  config_file.close();
//==================================================================================================
//==================================================================================================
  // auto processor = Processor(hosts, hosts[parser.id()-1]);

  // create a socket for that given process!
  // udpSocket = UDPSocket(hosts[parser.id()-1]);
  // // start the socket -> we create two threads, one for sending and one for receiving
  // udpSocket.create();

  processor = Processor(hosts, hosts[parser.id()-1]);
  std::cout << "Processor created" << std::endl;
  // std::cout << processor.neighbors.size() << std::endl;
  processor.create();

  // std::vector<std::string> proposal_strs = split_4("1234,1234");

  std::set<int> proposal = split_4("1234,1235");
  
  std::cout << "==========================\n";

  // for (auto elem : proposal) {
  //   std::cout << elem << std::endl;
  // }

  processor.propose(proposal);

  while (!processor.is_decided()){}

  std::cout << "HASTA LA VISTA BABY" << std::endl;

  // For Perfect Links
  // if this is not the receiving process, then it can broadcast the messages!
  // if (parser.id() != i) {
  //   for (unsigned int message=1;message<=m;message ++) {
  //     udpSocket.enque(hosts[i-1], message);      
  //   }
  // }

  // For Best Effort Broadcast
  // for (unsigned int message=1; message<=m; message++) {
  //   for (auto &host : hosts) {
  //     udpSocket.enque(host, message);
  //   }
  // }

  // for (std::string message : numbers_strs) {
  //   for (auto &host : hosts) {
  //     udpSocket.enque(host, message);
  //   }
  // }
 
  // process now needs to listen indefinitely - regardless if it broadcast messages before or not!
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
