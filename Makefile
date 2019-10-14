DEBUG_OPTS=-g
#OPTIM_OPTS=-o2
OPTS=-Wall -std=c99 $(DEBUG_OPTS) $(OPTIM_OPTS)
CFLAGS=$(OPTS) `pkg-config --cflags libwnck-3.0`

LDFLAGS =$(OPTS) `pkg-config --libs libwnck-3.0`
RM=rm -f

PRG=workspace-display-change
TEST=test
PRG_SRC=workspace-display-change.c workspace-change-manager.c
TEST_SRC=
SRCS=

OBJS_PRG=$(patsubst %.c,%.o,$(SRCS) $(PRG_SRC))
OBJS_TEST=$(patsubst %.c,%.o,$(SRCS) $(TEST_SRC))

DEPEND=.deps

all: $(PRG)

$(TEST): $(OBJS_TEST)
	$(CC) $(LDFLAGS) $(OBJS_TEST) -o $(TEST)

$(PRG): $(OBJS_PRG)
	$(CC) $(LDFLAGS) $(OBJS_PRG) -o $(PRG)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

depend: $(DEPEND)

$(DEPEND):
	$(CC) $(CFLAGS) -M $(SRCS) $(PRG_SRC) $(TEST_SRC) > $@

clean:
	$(RM) $(OBJS_TEST) $(OBJS_PRG)  $(PRG) $(TEST)

distclean: clean
	$(RM) *~
	$(RM) $(DEPEND)
	$(RM) 

-include $(DEPEND)
