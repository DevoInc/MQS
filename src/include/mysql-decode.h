#ifndef _MYSQL_DECODE_H_
#define _MYSQL_DECODE_H_

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int mysql_decode(int packet_id, lua_State *luavm, const unsigned char *payload, size_t payload_size, _Bool quiet);

#endif // _MYSQL_DECODE_H_
