.PHONY: default all clean objs all_clean docs tests

default: all

all:
	@ $(MAKE) -C src all
	@ $(MAKE) -C tests all SKIP_MODEL_DEPS=1

clean objs all_clean:
	@ $(MAKE) -C src $@
	@ $(MAKE) -C tests $@
	
docs:
	@ $(MAKE) -C doc $@

tests:
	@ $(MAKE) -C tests all SKIP_MODEL_DEPS=1

