#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <log.h>
#include <mysql-decode.h>

int mysql_decode(int packet_id, lua_State *luavm, const unsigned char *payload, size_t payload_size, _Bool quiet)
{
	size_t mysql_len;
	char packet_number;
	char command;
	char *query;
	int retval;
	
	if (payload_size < 4) {
		return -1;
	}

	mysql_len = payload[2] << 16 | payload[1] << 8 | payload[0];
	packet_number = payload[3];

	if ((payload_size - 4) != mysql_len) {
		if (!quiet)  {
			log_warning("[pktid:%d] Incorrect Size (%d). Expecting %d. Skipping packet.\n", packet_id, payload_size, mysql_len);
		}
		return -1;
	}

	command = payload[4];
	if (command == 0x03) { // 0x03 is a Query
		query = malloc(mysql_len);
		if (!query) {
			log_crit("Cannot allocate Query.");
			return -1;
		}
		memcpy(query, payload + 5, mysql_len-1);
		query[mysql_len-1] = '\0';

		if (!luavm) {
			if (!quiet) {
				printf("%s\n", query);
			}
		} else {
			lua_getglobal(luavm, "mariadb_query_execute");
			lua_pushlstring(luavm, query, mysql_len - 1);
			retval = lua_pcall(luavm, 1, 0, 0);
			switch(retval) {
			case 0:
				break;
			case LUA_ERRRUN:
				log_crit("Lua Runtime error: %s\n", lua_tostring(luavm, -1));
				break;
			case LUA_ERRMEM:
				log_crit("Lua memory allocation error\n");
				break;
			case LUA_ERRERR:
				log_crit("Lua error setting the error\n");
				break;
			default:
				log_crit("Lua unknown error\n");
				break;					
			}
		}
		free(query);
	}
	
	return 0;
}
