# From: https://raw.githubusercontent.com/acetcom/nextepc/7c7cfd1cbf5a0899535ee468096c3471ab48b4a8/support/docker/ubuntu/bionic/base/Dockerfile
ARG dist=ubuntu
ARG tag=latest
#FROM ${dist}:${tag}
FROM golang:1.11-stretch

MAINTAINER Sukchan Lee <acetcom@gmail.com>

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        libtool \
        gcc \
        flex \
        bison \
        git \
        libsctp-dev \
        libgnutls28-dev \
        libgcrypt-dev \
        libssl-dev \
        libidn11-dev \
        libmongoc-dev \
        libbson-dev \
        libyaml-dev \
        build-essential \
        iproute2 \
        ca-certificates \
        netbase \
        iptables \
        pkg-config && \
    apt-get clean
