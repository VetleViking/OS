FROM debian:latest

ENV PREFIX="/usr/local/i386elfgcc" \
    TARGET="i386-elf" \
    PATH="/usr/local/i386elfgcc/bin:$PATH"

# Install necessary packages
RUN apt-get update && \
    apt-get install -y \
    nasm \
    qemu-kvm \
    build-essential \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    curl \
    grub2-common \
    xorriso \
    grub-pc-bin

# Download and build binutils
RUN mkdir -p /tmp/src && \
    cd /tmp/src && \
    curl -O http://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz && \
    tar xf binutils-2.39.tar.gz && \
    mkdir binutils-build && \
    cd binutils-build && \
    ../binutils-2.39/configure --target=$TARGET --enable-interwork --enable-multilib --disable-nls --disable-werror --prefix=$PREFIX && \
    make all install

# Download and build GCC
RUN cd /tmp/src && \
    curl -O https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz && \
    tar xf gcc-12.2.0.tar.gz && \
    mkdir gcc-build && \
    cd gcc-build && \
    ../gcc-12.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-libssp --enable-languages=c,c++ --without-headers && \
    make all-gcc && \
    make all-target-libgcc && \
    make install-gcc && \
    make install-target-libgcc

RUN rm -rf /tmp/src

WORKDIR /workspace

CMD ["bash"]
