# matsprak-2022


## How to create ll file for comparison

```bash
clang++-16 -S -emit-llvm file.cpp
```

## How to build llvm ir files

```bash
llc-16 file.ll
```

## About llvm ir

https://llvm.org/docs/LangRef.html#functions

`#0` etc is attribute groups for functions

It seems like it is possible to remove the extra fluff from generated files
without any compilation errors.


### Specific commands
