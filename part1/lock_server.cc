// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server(bool _debug):
  nacquire (0), debug (_debug), requests()
{
  assert(pthread_mutex_init(&_req_lock, 0) == 0);
  assert(pthread_mutex_init(&_req_cond_lock, 0) == 0);
  assert(pthread_cond_init(&_req_cond, 0) == 0);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}

/**
 * Server side acquire procedure.
 */
lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r){
  lock_protocol::status ret = lock_protocol::OK;
  printf("acquire request from clt %d of lock %lld\n", clt, lid);
  r = 0;
  bool is_consent = false;

  // if lock of lid is not held by a client, add it and return
  // or block untill the lock is released
  assert(pthread_mutex_lock(&_req_lock) == 0);
  std::unordered_map<lock_protocol::lockid_t, std::queue<int>>::const_iterator
    got = requests.find(lid);
  if(got == requests.end()){
    std::queue<int> *req_clts = new std::queue<int>();
    printf("lock %lld queue address %p\n", lid, req_clts);
    req_clts->push(clt);
    requests.insert(std::pair<lock_protocol::lockid_t, std::queue<int>>(
          lid, *req_clts));
    is_consent = true;
  }
  assert(pthread_mutex_unlock(&_req_lock) == 0);
  if(is_consent){
    return ret;
  }
  assert(pthread_mutex_lock(&_req_cond_lock) == 0);
  for(;;){
    assert(pthread_mutex_lock(&_req_lock) == 0);
    got = requests.find(lid);
    if((got->second).front() == clt){
      is_consent = false;
    }
    assert(pthread_mutex_unlock(&_req_lock) == 0);
    if(is_consent){
      // Not my turn.
      assert(pthread_cond_wait(&_req_cond, &_req_cond_lock) == 0);
    }else{
      break;
    }
  }
  assert(pthread_mutex_lock(&_req_cond_lock) == 0);
  return ret;
}
/**
 * Server side release procedure.
 */
lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r){
  lock_protocol::status ret = lock_protocol::OK;
  printf("release request from clt %d of lock %lld\n", clt, lid);
  r = 0;
  assert(pthread_mutex_lock(&_req_lock) == 0);
  std::unordered_map<lock_protocol::lockid_t, std::queue<int>>::const_iterator
    got = requests.find(lid);
  if((got->second).size() == 1){
    printf("lock %lld queue address %p\n", lid, &(got->second));
    //delete &(got->second);
    requests.erase(lid);
  }else{
    std::queue<int> q = got->second;
    q.pop();
  }
  assert(pthread_mutex_unlock(&_req_lock) == 0);
  assert(pthread_cond_signal(&_req_cond) == 0);
  return ret;
}


