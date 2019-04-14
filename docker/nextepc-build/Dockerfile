# Modify from: https://raw.githubusercontent.com/acetcom/nextepc/749d632b86c7faa0550367ec80acc53185812b4a/support/docker/build/Dockerfile

FROM sufuf3/nextepc-base:latest

WORKDIR /root
COPY setup.sh /root
RUN chmod 777 /root/setup.sh

ARG USER=nctucs
ARG REPO=free5gc
ARG BRANCH=master
RUN cd /usr/src && git clone https://bitbucket.org/nctu_5g/free5gc.git /usr/src/free5gc
# Ca't not use v1.0.0, cause the config file is useless for the program
RUN cd /usr/src/free5gc && git checkout da4e100
RUN go get -u -v "github.com/gorilla/mux"
RUN go get -u -v "golang.org/x/net/http2"
RUN go get -u -v "golang.org/x/sys/unix"
ADD https://api.github.com/repos/sufuf3/kube5gc/git/refs/heads/nctu5gc /root/nextepc-ver.json

RUN cd /usr/src/free5gc && \
    autoreconf -iv && \
    ./configure --prefix=`pwd`/install && \
    make -j `nproc` && \
    make install

WORKDIR /usr/src/free5gc
