.PHONY: build
.PHONY: run
.PHONY: clean

build:
	gcc -fsanitize=address main.c backend/lexer.c backend/parser.c -o bin/main.o

run: build
	./bin/main.o

clean:
	rm -rf bin/*.o
