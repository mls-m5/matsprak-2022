
#include "file.h"
#include "rootblock.h"
#include "state.h"
#include <iostream>
#include <memory>

#define vout std::cout

RootBlock parse(State s) {
    auto root = RootBlock{};

    if (s.token() == Token::Module) {
        s.next();
        root.moduleName = s.token();
        s.next().expect(Token::Semicolon);

        vout << "module " << root.moduleName.content << std::endl;
    }

    return RootBlock{};
}

int main(int argc, char *argv[]) {
    auto state = State(openFile("demo/1-basic.msk"));

    auto root = parse(state);

    return 0;
}
