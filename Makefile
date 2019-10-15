all:
	make -C src/

clean:
	rm -f *.tar.bz2
	rm -f *~
	make clean -C src/


package:
	rm -rf /tmp/mqs-$(shell cat VERSION)
	mkdir /tmp/mqs-$(shell cat VERSION)
	make clean
	make
	cp -r * /tmp/mqs-$(shell cat VERSION)/
	tar cvfj mqs-$(shell cat VERSION).tar.bz2 /tmp/mqs-$(shell cat VERSION)/ --transform='s/tmp\///g'
	rm -rf /tmp/mqs-$(shell cat VERSION)/
	@echo ""
	@echo "Package ready: mqs-$(shell cat VERSION).tar.bz2"
