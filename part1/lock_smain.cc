// unmarshall RPCs from lock_smain and hand them to lock_server

#include "rpc.h"
#include <arpa/inet.h>      //IP address manipulation.
#include "lock_server.h"

int
main(int argc, char *argv[])
{
  //Set no buffer.
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  srandom(getpid());

  if(argc != 2){
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(1);
  }


  // Create a lock server.
  lock_server ls;
  // Create a RPC server
  rpcs server(htons(atoi(argv[1])));
  server.reg(lock_protocol::stat, &ls, &lock_server::stat);


  while(1)
    sleep(1000);
}
