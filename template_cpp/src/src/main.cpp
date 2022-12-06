#include <chrono>
#include <iostream>
#include <thread>
#include <string>
#include <sstream>

#include "parser.hpp"
#include "udp.hpp"
#include "beb.hpp"
#include "hello.h"
#include <signal.h>

std::ofstream outputFile;
UDPSocket udpSocket;

static void stop(int) {
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";

  // write/flush output file if necessary
  std::cout << "Writing output.\n";

  for(auto const &output: udpSocket.get_logs()){
    outputFile << output << "\n" ;
  }
  outputFile.close();
  
  // exit directly from signal handler
  exit(0);
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
  auto hosts = parser.hosts();
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

  // For Perfect Links:
  // config_file >> m >> i;

  // For Best Effort Broadcast:
  config_file >> m;


  config_file.close();
//==================================================================================================
//==================================================================================================

  // create a socket for that given process!
  udpSocket = UDPSocket(hosts[parser.id()-1]);
  // start the socket -> we create two threads, one for sending and one for receiving
  udpSocket.create();

  // For Perfect Links
  // if this is not the receiving process, then it can broadcast the messages!
  // if (parser.id() != i) {
  //   for (unsigned int message=1;message<=m;message ++) {
  //     udpSocket.enque(hosts[i-1], message);      
  //   }
  // }

  // For Best Effort Broadcast
  for (unsigned int message=1; message<=m; message++) {
    for (auto &host : hosts) {
      udpSocket.enque(host, message);
    }
  }
 
  // process now needs to listen indefinitely - regardless if it broadcast messages before or not!
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
