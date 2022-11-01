
#include "file.h"
#include "rootblock.h"
#include "state.h"
#include <iostream>
#include <memory>

RootBlock parse(State s) {
    for (Token t; t = s.token(), s; s.next()) {
        std::cout << t.content << std::endl;
    }

    return RootBlock{};
}

int main(int argc, char *argv[]) {
    auto state = State(openFile("demo/1-basic.msk"));

    auto root = parse(state);

    return 0;
}
