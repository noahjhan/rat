all:
	@$(MAKE) -C ./build

rat:
	@$(MAKE) -C ./build
	@./build/rat

clean:
	@rm -rf ./build > /dev/null 2>&1
	@mkdir ./build
	@cd ./build && cmake .. > /dev/null 2>&1

compile:
	@$(MAKE) -C ./build
	@./build/rat
	@clang data/compile.ll -o build/compile

