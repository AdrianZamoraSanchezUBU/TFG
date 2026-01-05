# Required dependencies 
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libc6 \
    libstdc++6 \
    libgcc-s1 \
    \
    llvm-18 \
    libllvm18 \
    clang \
    clang-18 \
    \
    libedit2 \
    libxml2 \
    zlib1g \
    libzstd1 \
    libicu74 \
    libtinfo6 \
    liblzma5 \
    libbsd0 \
    libmd0 \
    \
    libantlr4-runtime-dev \
    \
    libspdlog-dev \
    libfmt-dev \
    \
    libffi8 \
    \
    && rm -rf /var/lib/apt/lists/*

# Compiler folder
WORKDIR /opt/tlang

# Copy build required object files to the compiler folder 
COPY build/ /opt/tlang/

# Copy the demo examples
COPY tests/input/demo/ /opt/tlang/examples/

# For command-like usage
ENV PATH="/opt/tlang:${PATH}"

# Setting the default folder
WORKDIR /opt/tlang

# Main executable
ENTRYPOINT ["TCompiler"]