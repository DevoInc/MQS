#ifndef _PACKET_LOOP_H_
#define _PACKET_LOOP_H_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <pcap.h>

int packet_loop(lua_State *luavm, pcap_t *handle, _Bool quiet);

#endif // _PACKET_LOOP_H_
