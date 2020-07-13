CC      = clang
CFLAGS  = -Wall -Wextra -Wpedantic -std=gnu11
LDFLAGS = -lncurses
OUTDIR  = out

chip8:  $(patsubst %.c, $(OUTDIR)/%.o, $(wildcard *.c))
	$(CC) $^ -o $@ -O2 $(CFLAGS) $(LDFLAGS)

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	$(CC) -c $^ -o $@ -O2 $(CFLAGS)

debug: chip8-debug

chip8-debug: $(patsubst %.c, $(OUTDIR)/%-debug.o, $(wildcard *.c))
	$(CC) $^ -o $@ -Og $(CFLAGS) $(LDFLAGS)

$(OUTDIR)/%-debug.o: %.c | $(OUTDIR)
	$(CC) -c $^ -o $@ -Og $(CFLAGS)

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	$(RM) chip8 chip8-debug $(OUTDIR)/*
