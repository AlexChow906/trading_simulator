#include <cstdio>

#include "gateway/gateway.h"

int main() {
    Gateway gateway(8888);
    gateway.run();
}
