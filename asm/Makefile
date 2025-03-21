llvm:
	@mkdir -p bin
	@clang llvm.ll -o bin/llvm

run:
	@mkdir -p bin
	@clang llvm.ll -o bin/llvm
	@./bin/llvm

program:
	@mkdir -p bin
	@clang -S -emit-llvm program.c -o program.ll
	@clang program.ll -o bin/program

asm:
	@clang program.ll -o bin/program
	@./bin/program

clean:
	@rm -rf bin/*
	@rm program.ll