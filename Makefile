NAME     = chip8

SRCDIR   = src
INCDIR   = inc
OUTDIR   = out

CC       = clang
CFLAGS   = -Wall -Wextra -Wpedantic -std=gnu11
LDFLAGS  = -lncurses
INCFLAGS = -I$(INCDIR)
DEPFLAGS = -MMD -MP -MF$(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.d, $<)

RELFLAGS = -O2
DBGFLAGS = -g -Og

.PHONY: all release debug clean

release: $(NAME)

all: release debug

$(NAME): $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(wildcard $(SRCDIR)/*.c))
	$(CC) $^ -o $@ $(CFLAGS) $(RELFLAGS) $(LDFLAGS)

$(OUTDIR)/%.o: $(SRCDIR)/%.c | $(OUTDIR)
	$(CC) -c $< -o $@ $(INCFLAGS) $(DEPFLAGS) $(CFLAGS) $(RELFLAGS)

debug: $(NAME)-dbg

$(NAME)-dbg: $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%-dbg.o, $(wildcard $(SRCDIR)/*.c))
	$(CC) $^ -o $@ $(CFLAGS) $(DBGFLAGS) $(LDFLAGS)

$(OUTDIR)/%-dbg.o: $(SRCDIR)/%.c | $(OUTDIR)
	$(CC) -c $< -o $@ $(INCFLAGS) $(DEPFLAGS) $(CFLAGS) $(DBGFLAGS)

$(OUTDIR):
	mkdir -p $(OUTDIR)

clean:
	$(RM) $(NAME) $(NAME)-dbg $(OUTDIR)/*

-include $(OUTDIR)/*.d
