#!/bin/bash

if ! [ -d "ompi"]; then
    git clone https://github.com/open-mpi/ompi-release /root/ompi
    cd /root/ompi
    autogen.pl && ./configure --with-platform=optimized --prefix="/root/builds/openmpi" && make all install
    export LD_LIBRARY_PATH="/root/builds/openmpi/lib"
    export PATH="$PATH:/root/builds/openmpi/bin"
fi
