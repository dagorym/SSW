default: all

all clean objs:
	@ cd src; ${MAKE} $@
	
docs:
	@ cd doc; ${MAKE} $@
