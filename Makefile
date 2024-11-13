CC=gcc
CFLAGS=-Wall -Wextra -g
LIBS=
SRC=src/
BUILD=build/
BIN=bin/

DEPS=  $(BUILD)pparser.o $(BUILD)parser.o $(BUILD)lexer.o $(BUILD)string_view.o  $(BUILD)regexp.o $(BUILD)pparser_lexer.o $(BUILD)pparser_parser.o
all: init lines pparser
lines:
	@echo "C:"
	@wc -l $$( find -wholename './*.[hc]') | tail -n 1
$(BUILD)%.o: $(SRC)%.c
	 $(CC) $(CFLAGS) -o $@ $^ -c
$(BIN)pparser: $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
pparser: $(BIN)pparser
clean:
	rm -rf $(BIN)*
	rm -rf $(BUILD)*
install:
	cp $(BIN)pparser /bin/
init:
	