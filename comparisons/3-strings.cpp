
#include <cstdio>

void print(const char *str);

void hello() {
    print("hello");
    print("there");
}

void print(const char *str) {
    puts(str);
}
