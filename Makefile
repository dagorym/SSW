default: all

all clean objs all_clean:
	@ cd src; ${MAKE} $@
	
docs:
	@ cd doc; ${MAKE} $@
