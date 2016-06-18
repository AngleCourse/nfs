// this is the lock server
// the lock client has a similar interface

#ifndef lock_server_h
#define lock_server_h

#include <string>
#include <queue>
#include <unordered_map>
#include <pthread.h>
#include <sys/types.h>
#include "lock_protocol.h"
#include "lock_client.h"
#include "rpc.h"

class lock_server {

 protected:
  int nacquire;
  bool debug;
  pthread_mutex_t _req_lock;
  pthread_mutex_t _req_cond_lock;
  pthread_cond_t  _req_cond;

  std::unordered_map<lock_protocol::lockid_t, std::queue<int>> requests;

 public:
  lock_server(bool _debug=false);
  ~lock_server() {};
  lock_protocol::status stat(int clt, lock_protocol::lockid_t lid, int &);
  lock_protocol::status acquire(int clt, lock_protocol::lockid_t lid, int &);
  lock_protocol::status release(int clt, lock_protocol::lockid_t lid, int &);
};

#endif 







