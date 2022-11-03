

struct Apa {
    int x;
    int y;
    double z;
    char *c;
};

int apFunc(Apa &apa) {
    return apa.x;
}

void apFunc2(Apa &apa) {
    apa.x = 20;
    apa.y = 30;
    apa.z = 3;
}

Apa create() {
    return Apa{};
}

int useRet() {
    auto apa = create();
    return apa.x;
}

int use() {
    auto apa = Apa{};
    apa.x = 10;
    return apa.x;
}
