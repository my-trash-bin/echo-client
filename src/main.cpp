#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

static void die(const std::string &message) {
  std::cerr << message << std::endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: ip_address port" << std::endl;
    exit(EXIT_SUCCESS);
  }
  const char *serverIp = argv[1];
  int port = atoi(argv[2]);

  struct hostent *host = gethostbyname(serverIp);
  sockaddr_in sendSockAddr;
  bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
  sendSockAddr.sin_family = AF_INET;
  sendSockAddr.sin_addr.s_addr =
      inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
  sendSockAddr.sin_port = htons(port);
  int clientSd = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSd < 0)
    die("Error creating socket");
  int status =
      connect(clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
  if (status < 0)
    die("Error connecting socket");
  while (1) {
    std::cout << "> ";
    std::string data;
    getline(std::cin, data);
    send(clientSd, (const void *)data.c_str(), data.length(), 0);
    char buffer[1025];
    const ssize_t read_bytes = recv(clientSd, (void *)buffer, 1024, 0);
    if (read_bytes < 0)
      die("Error receiving data");
    if (read_bytes == 0) {
      std::cout << "bye" << std::endl;
      break;
    }
    buffer[read_bytes] = '\0';
    std::cout << "| " << buffer << std::endl;
  }
  close(clientSd);
  return 0;
}
