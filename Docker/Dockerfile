FROM ubuntu:latest
MAINTAINER Peter Gottesman <peter@petergottesman.com>

ADD testprog.c /testprog.c

RUN apt-get update && apt-get install -y gcc cmake git
RUN apt-get install -y autoconf automake libtool m4
RUN apt-get install -y g++ flex

RUN git clone https://github.com/open-mpi/ompi-release /ompi
WORKDIR /ompi
RUN ./autogen.pl
RUN ./configure --with-platform=optimized --prefix="/build"
RUN make all install
ENV LD_LIBRARY_PATH="/build/lib"
ENV PATH="$PATH:/build/bin"

RUN mpicc /testprog.c -o testprog
RUN mpirun --allow-run-as-root testprog 

