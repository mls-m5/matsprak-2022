
all: 1-variables.ll 2-structs.ll 3-strings.ll 4-functions.ll

%.ll: %.cpp
	clang++-16 -S -emit-llvm $<
