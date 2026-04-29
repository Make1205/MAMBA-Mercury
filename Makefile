.PHONY: all clean test-all-fast

all:
	$(MAKE) -C ref all

clean:
	$(MAKE) -C ref clean
	-$(MAKE) -C avx2 clean

test-all-fast:
	bash scripts/test_all.sh
