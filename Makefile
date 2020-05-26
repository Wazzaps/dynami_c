
CFLAGS := -I include -Os -fno-stack-protector
LDFLAGS := -Os

build/tester: build/tests/tester.o build/dynamic/dynamic.o
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ -o $@

build/tests/%.o: tests/%.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

build/dynamic/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@