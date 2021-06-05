BINS=client server
LIBS=-lpthread

all: $(BINS)

%: %.c
	gcc $^ -o $@ $(LIBS)

clean:
	rm -f $(BINS)
