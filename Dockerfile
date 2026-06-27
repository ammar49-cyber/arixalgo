# syntax=docker/dockerfile:1
FROM ubuntu:22.04 AS builder

LABEL org.opencontainers.image.title="ARIX-Algo"
LABEL org.opencontainers.image.description="Next-generation AI architecture with security built into the foundation"
LABEL org.opencontainers.image.version="0.1.0"
LABEL org.opencontainers.image.licenses="MIT"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    python3 \
    python3-pip \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY CMakeLists.txt CMakePresets.json VERSION ./
COPY src/ src/
COPY tests/ tests/
COPY scripts/ scripts/

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DARIX_BUILD_TESTS=ON -DARIX_BUILD_BENCHMARKS=OFF && \
    cmake --build . -j"$(nproc)"

FROM ubuntu:22.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/arix-algo

COPY --from=builder /src/build/bin/ ./bin/
COPY --from=builder /src/build/lib/ ./lib/
COPY VERSION .

CMD ["/bin/bash"]
