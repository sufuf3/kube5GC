# Modify from: https://raw.githubusercontent.com/acetcom/nextepc/749d632b86c7faa0550367ec80acc53185812b4a/support/docker/build/Dockerfile

FROM sufuf3/nextepc-base:latest

WORKDIR /root
COPY setup.sh /root

ARG USER=acetcom
ARG REPO=nextepc
ARG BRANCH=master
RUN cd /usr/src && git clone https://github.com/sufuf3/kube5GC.git nextepc
RUN cd /usr/src/nextepc && git checkout nctu5gc
ADD https://api.github.com/repos/sufuf3/kube5gc/git/refs/heads/nctu5gc /root/nextepc-ver.json

RUN cd /usr/src/nextepc && \
    autoreconf -f -i && \
    ./configure \
        --prefix=/usr \
        --sysconfdir=/etc \
        --localstatedir=/var && \
    make -j `nproc` install
