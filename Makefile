.PHONY: default all clean objs all_clean docs tests check

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

# Builds everything (SSW, BattleSim, and all three test runners) and then
# runs all three CppUnit suites in sequence, propagating failure: SSWTests
# (core/weapons/strategic/ships), TacticalTests (tactical fixtures), and
# GuiTests (GUI fixtures, run headless under xvfb-run). Any suite exiting
# non-zero stops the target immediately with a non-zero exit code.
check: all
	cd tests && ./SSWTests
	cd tests/tactical && ./TacticalTests
	cd tests/gui && xvfb-run -a ./GuiTests

