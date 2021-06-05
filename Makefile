BINS=client.out server.out
LIBS=-lpthread

all: $(BINS)

%.out: %.c
	gcc $^ -o $@ $(LIBS)

clean:
	rm -f $(BINS)
