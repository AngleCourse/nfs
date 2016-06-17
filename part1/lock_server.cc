// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server(bool _debug):
  nacquire (0), debug (_debug)
{
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
  return ret;
}


