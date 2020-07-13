CC      = clang
CFLAGS  = -Wall -Wextra -Wpedantic -std=gnu11
LDFLAGS = -lncurses
SRCDIR  = src
INCDIR  = inc
OUTDIR  = out

chip8: $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(wildcard $(SRCDIR)/*.c))
	$(CC) $^ -o $@ -O2 $(CFLAGS) $(LDFLAGS)

$(OUTDIR)/main.o: $(SRCDIR)/main.c
	$(CC) -c $^ -o $@ -O2 $(CFLAGS) -I$(INCDIR)

$(OUTDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/%.h | $(OUTDIR)
	$(CC) -c $< -o $@ -O2 $(CFLAGS) -I$(INCDIR)

debug: chip8-debug

chip8-debug: $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%-debug.o, $(wildcard $(SRCDIR)/*.c))
	$(CC) $^ -o $@ -Og $(CFLAGS) $(LDFLAGS)

$(OUTDIR)/main-debug.o: $(SRCDIR)/main.c
	$(CC) -c $^ -o $@ -Og $(CFLAGS) -I$(INCDIR)

$(OUTDIR)/%-debug.o: $(SRCDIR)/%.c $(INCDIR)/%.h | $(OUTDIR)
	$(CC) -c $< -o $@ -Og $(CFLAGS) -I$(INCDIR)

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	$(RM) chip8 chip8-debug $(OUTDIR)/*

