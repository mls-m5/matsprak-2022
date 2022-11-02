
#include "file.h"
#include "parse/module.h"
#include "state.h"

int main(int argc, char *argv[]) {
    auto state = State(openFile("demo/1-basic.msk"));

    auto root = parse(state);

    return 0;
}
