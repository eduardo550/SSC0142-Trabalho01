BINS=client.out server.out
LIBS=-lpthread
FLAGS=-Wall -g

all: $(BINS)

%.out: %.c
	gcc $^ -o $@ $(LIBS) $(FLAGS)

clean:
	rm -f $(BINS)
