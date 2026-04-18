default: all tests

all clean objs all_clean:
	@ cd src; ${MAKE} $@
	@ cd tests; ${MAKE} $@
	
docs:
	@ cd doc; ${MAKE} $@

tests:
	@ cd tests; ${MAKE} $@

