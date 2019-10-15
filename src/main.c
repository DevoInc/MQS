#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <pcap.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <log.h>

#include <packet-loop.h>

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518
#define TO_MS 1000
/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14
/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

void print_help(char *argv0)
{
	printf("%s [OPTION]\n", argv0);
	printf("\nList of options:\n");
	printf("-d \t\tRun as Daemon\n");
	printf("-f \"BPF\" \tUse BPF to filter\n");
	printf("-h \t\tPrint this help\n");
	printf("-i INTERFACE \tSet the interface to monitor\n");
	printf("-p \t\tPromiscuous mode\n");
	printf("-q \t\tQuiet mode, do not print anything\n");	
	printf("-r FILE \tDecode the PCAP file provided\n");
	printf("-s FILE \tScript to use to output queries to\n");
}

int lua_add_utils_package(lua_State *luavm)
{
	char *newlua_path;
	
	lua_getglobal(luavm, "package");
	lua_getfield(luavm, -1, "path");
//	printf("current path:%s\n", lua_tostring(luavm, -1));
	asprintf(&newlua_path, "%s;./mqs-lualib/?.lua;/usr/local/share/mqs/mqs-lualib;/usr/share/mqs/mqs-lualib", lua_tostring(luavm, -1));
	lua_pop(luavm, 1);
	lua_pushstring(luavm, newlua_path);
	lua_setfield(luavm, -2, "path" );
	lua_pop(luavm, 1);
	free(newlua_path);
	return 0;
}

int main(int argc, char **argv)
{

	pcap_t *pcaph;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;
	
	// Options
	int opt;
	char *iface=NULL;
	char *readfile=NULL;
	char *filter=NULL;
	char *script=NULL;
	_Bool promisc=0;
	_Bool daemonize=0;
	_Bool quiet=0;
	
	lua_State *luavm;
	
	int retval;
	

	log_set_default_color(1);

	if (argc == 2) {
		if (!strcmp(argv[1], "--help")) {
			print_help(argv[0]);
			return -1;
		}
	}
	
	while ((opt = getopt(argc, argv, "df:hi:pqr:s:")) != -1) {
		switch(opt) {
		case 'd':
			daemonize = 1;
			break;
		case 'f':
			filter = optarg;
			break;
		case 'h':
			print_help(argv[0]);
			return -1;
		case 'i':
			iface = optarg;
			break;
		case 'p':
			promisc = 1;
			break;
		case 'q':
			quiet = 1;
			break;			
		case 'r':
			readfile = optarg;
			break;
		case 's':
			script = optarg;
			break;
		default:
			break;

		}
	}

	if (quiet) {
		if (script) {
			log_notice("Quiet mode on, but script provided. We cannot guarantee the quietness\n");
		}
		log_set_level(LOG_LEVEL_QUIET);
	} else {
		log_set_level(LOG_LEVEL_NOTICE);
	}

	
	luavm = luaL_newstate();
	if (!luavm) {
		log_crit("Cannot initilize out Lua VM!");
	}	
	luaL_openlibs(luavm);
	

	if (!script) {
		log_notice("No lua script provided, will simply print MariaDB statements to stdout\n");
		luavm = NULL;
	} else {
		lua_add_utils_package(luavm);
		luaL_loadfile(luavm, script);
		lua_pcall(luavm, 0, 0, 0);
	}
	
	if (daemonize && readfile) {
		log_crit("We cannot daemonize as we are just reading a file.");
		return -1;
	}

	if (daemonize) {
		daemon(1, 1);
	}
	
	if (iface) {
		if (readfile) {
			log_error("Cannot sniff on an interface AND read a file at the same time. Choose one.");
			return -1;
		}
		log_notice("Sniffing on '%s'.\n", iface);
	} else {
		if (!readfile) {
			log_notice("No interface set. Sniffing on 'any'.\n");
			iface = "any";
		}
	}


	if (readfile) {
		pcaph = pcap_open_offline(readfile, errbuf);
		if (!pcaph) {
			log_crit("Could not read file %s: %s\n", readfile, errbuf);
			return -1;
		}
	} else {
		pcaph = pcap_open_live(iface, BUFSIZ, promisc, TO_MS, errbuf);
		if (!pcaph) {
			log_crit("Could not open device %s: %s\n", iface, errbuf);
			return -1;
		}
	}
	if (!filter) {
		filter = "dst port 3306";
	}

	retval = pcap_compile(pcaph, &fp, filter, 0, PCAP_NETMASK_UNKNOWN);
	if (retval < 0) {
		log_crit("Error compiling filter: %s\n", pcap_geterr(pcaph));
		return -1;
	}
	retval = pcap_setfilter(pcaph, &fp);
	if (retval < 0) {
		log_crit("Could not install filter: %s\n", pcap_geterr(pcaph));
		return -1;
	}
	
	retval = packet_loop(luavm, pcaph, quiet);

	log_notice("Closing...");
	
	pcap_close(pcaph);
	
	return 0;
}
