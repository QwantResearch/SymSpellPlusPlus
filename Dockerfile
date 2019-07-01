# Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

FROM debian:9-slim

LABEL AUTHORS="Qwant Research"

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get -y update && \
    apt-get -y install \
        cmake \
        g++ \
        libyaml-cpp-dev \
        git \
        cmake

COPY . /opt/url_segmenter

WORKDIR /opt/url_segmenter

RUN ./install.sh

#RUN apt-get -y remove \
#      libboost-locale1.65-dev \
#      libboost-regex1.65-dev \
#      libyaml-cpp-dev

RUN groupadd -r qnlp && useradd --system -s /bin/bash -g qnlp qnlp

USER qnlp 

