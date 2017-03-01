TARGETS=mmaptest mutextest

all: $(TARGETS)

mmaptest: mmaptest.c
	gcc -O0 -ggdb3 -o "$@" "$<"

mutextest: mutextest.c
	gcc -O0 -ggdb3 -pthread -o "$@" "$<"

clean:
	rm -f $(TARGETS)

anew: clean all
