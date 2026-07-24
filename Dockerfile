FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    cmake ninja-build g++ git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build

ENTRYPOINT ["./build/gateway_server"]
