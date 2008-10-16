default: all

all clean :
	@ cd src; ${MAKE} $@
	
docs:
	@ cd doc; ${MAKE} $@
