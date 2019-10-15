MariaDB Query Statement Sniffer
===============================

Please read doc/mqs.1 for the up to date documentation on program and options.

Dependencies
------------

* libpcap
* libluajit 5.1


How to Install
--------------

On Debian/Ubuntu:

	$ sudo apt-get install libluajit-5.1-dev libpcap0.8-dev make gcc
	$ make
	$ make package

Will produce a package that can be deployed where needed.

How to Use
----------

Reading all MySQL/MariaDB requests from a pcap file to stdout:

	$ ./mqs -r file.pcap

Reading only INSERT requests using a lua script to filter:

	$ ./mqs -r file.pcap -s ./lua-scripts/print-insert.lua


Similar to tcpdump, it is possible to daemonize and listen to a particular interface, with the -d and -i flags.

The default BPF filter is 'dst port 3306' but the -f flag can change that.


FAQ
---

**Q: I don't see any traffic despite sending requests on my MariaDB Server**

R: If configured to point to localhost, MySQL will use a Unix socket to go faster, thus preventing the TCP/IP Sniffing. Point to 127.0.0.1, such as mysql -h 127.0.0.1.
