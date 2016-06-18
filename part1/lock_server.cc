// the lock server implementation

#include "lock_server.h"
#include <time.h>
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
  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  long clt_stamp = clt + tv.tv_nsec;
  lock_protocol::status ret = lock_protocol::OK;
  printf("acquire request from clt %d of lock %lld\n", clt, lid);
  r = 0;
  bool is_consent = false;

  // if lock of lid is not held by a client, add it and return
  // or block untill the lock is released
  assert(pthread_mutex_lock(&_req_lock) == 0);
  std::unordered_map<lock_protocol::lockid_t, std::deque<long>>::iterator
    got = requests.find(lid);
  if(got == requests.end()){
    std::deque<long> *req_clts = new std::deque<long>();
    //printf("lock %lld queue address %p\n", lid, req_clts);
    req_clts->push_back(clt_stamp);
    requests.insert(std::pair<lock_protocol::lockid_t, std::deque<long>>(
          lid, *req_clts));
    if(debug){
      printf("clt %d creates wait list for lock %lld\n", clt, lid);
    }
    is_consent = true;
    nacquire++;
  }else{
    (got->second).push_back(clt_stamp);
    if(debug){
      printf("clt %d lock %lld wait list is %s\n",
          clt, lid, printWaitList(got->second).c_str());
    }
  }
  assert(pthread_mutex_unlock(&_req_lock) == 0);
  if(is_consent){
    if(debug){
      printf("clt %d get lock %lld\n", clt, lid);
    }
    return ret;
  }
  assert(pthread_mutex_lock(&_req_cond_lock) == 0);
  for(;;){
    is_consent=true;
    assert(pthread_mutex_lock(&_req_lock) == 0);
    got = requests.find(lid);
    if((got->second).front() == clt_stamp){
      is_consent = false;
      nacquire++;
    }
    if(debug){
      printf("Spin to clt %d and wait list of lock %lld is %s\n",
          clt, lid, printWaitList(got->second).c_str());
    }
    assert(pthread_mutex_unlock(&_req_lock) == 0);
    if(is_consent){
      // Not my turn.
      assert(pthread_cond_wait(&_req_cond, &_req_cond_lock) == 0);
    }else{
      break;
    }
  }
  assert(pthread_mutex_unlock(&_req_cond_lock) == 0);
  if(debug){
    printf("clt %d get lock %lld\n", clt, lid);
  }
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
  std::unordered_map<lock_protocol::lockid_t, std::deque<long>>::iterator
    got = requests.find(lid);
  if(got != requests.end()){
    if((got->second).size() == 1){
      //printf("lock %lld queue address %p\n", lid, &(got->second));
      //delete &(got->second);
      if(debug){
        printf("clt %d is going to erase wait list %s of lock %lld\n",
            clt, printWaitList(got->second).c_str(), lid);
      }
      requests.erase(lid);
    }else{
      if(debug){
        printf("clt %d gonna pop %ld from wait list of lock %lld\n", 
            clt, (got->second).front(), lid);
      }
      (got->second).pop_front();
    }
    assert(pthread_cond_broadcast(&_req_cond) == 0);
  }
  assert(pthread_mutex_unlock(&_req_lock) == 0);
  return ret;
}

std::string 
lock_server::printWaitList(std::deque<long> q){
  std::string ret = "[ ";
  for(unsigned i = 0; i < q.size(); i++){
    ret += " " + std::to_string(q[i]) + " ";
  }
  ret += "]";
  return ret;
}





