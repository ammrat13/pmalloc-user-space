# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

FROM alpine:3.16.1
RUN \
    apk add --no-cache build-base cmake py3-pip \
    && pip install --no-cache-dir cpplint
WORKDIR /work/
