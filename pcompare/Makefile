.PHONY: all dist clean distclean 
MAKE = make

all :
	+$(MAKE) --directory=./librpmvercmp
	+$(MAKE) --directory=./libpcompare
	+$(MAKE) --directory=./ucompare

dist :
	+$(MAKE) --directory=./librpmvercmp dist
	+$(MAKE) --directory=./libpcompare  dist
	+$(MAKE) --directory=./ucompare


clean :
	$(MAKE) --directory=./librpmvercmp clean
	$(MAKE) --directory=./libpcompare  clean
	$(MAKE) --directory=./ucompare     clean

distclean : clean
	$(MAKE) --directory=./librpmvercmp distclean
	$(MAKE) --directory=./libpcompare  distclean
	$(MAKE) --directory=./ucompare     distclean
