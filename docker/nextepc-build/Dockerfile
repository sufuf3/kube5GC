# Modify from: https://raw.githubusercontent.com/acetcom/nextepc/749d632b86c7faa0550367ec80acc53185812b4a/support/docker/build/Dockerfile

FROM sufuf3/nextepc-base:latest

WORKDIR /root
COPY setup.sh /root

ARG USER=nctucs
ARG REPO=free5gc
ARG BRANCH=master
RUN cd /usr/src && git clone https://github.com/sufuf3/kube5GC.git free5gc
RUN cd /usr/src/free5gc && git checkout nctu5gc
RUN go get -u -v "github.com/gorilla/mux"
RUN go get -u -v "golang.org/x/net/http2"
RUN go get -u -v "golang.org/x/sys/unix"
ADD https://api.github.com/repos/sufuf3/kube5gc/git/refs/heads/nctu5gc /root/nextepc-ver.json

RUN cd /usr/src/free5gc && \
    autoreconf -f -i && \
    ./configure \
        --prefix=`pwd`/install \
        --sysconfdir=/etc \
        --localstatedir=/var && \
    make -j `nproc` install
