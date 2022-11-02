

struct Apa {
    int x;
    int y;
    double z;
};

int apFunc(Apa &apa) {
    return apa.x;
}

void apFunc2(Apa &apa) {
    apa.x = 20;
    apa.y = 30;
    apa.z = 3;
}
