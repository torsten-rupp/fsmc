FROM ubuntu:22.04

ARG uid=0
ARG gid=0

# set marker in environment for running inside a docker
ENV CONTAINER=docker

# disable interactive installion
ENV DEBIAN_FRONTEND=noninteractive

# update
RUN apt-get -y update --fix-missing

# install packages
RUN apt-get -y install \
  debhelper \
  devscripts \
  git \
  joe \
  less \
  m4 \
  openssl \
  pkg-config \
  sudo \
  tar \
  unzip \
  wget \
  xz-utils \
  zip \
  ;

# install packages for building
RUN apt-get -y install \
  gcc \
  g++ \
  libc6 \
  libc6-dev \
  cmake \
  make \
  flex \
  valgrind \
  binutils \
  ;

# install byacc with backtrace support
RUN wget https://invisible-island.net/datafiles/release/byacc.tar.gz
RUN    tar xf byacc.tar.gz \
    && cd byacc-* \
    && ./configure --enable-btyacc \
    && make \
    && cp yacc /usr/local/bin/byacc \
  ;

RUN mkdir /.cache;

# add users
RUN groupadd -g $gid jenkins || true
#RUN useradd -m jenkins -u $uid -g $gid -p `openssl passwd -crypt jenkins`;
#RUN useradd -m test -p `openssl passwd -crypt test`;

# enable sudo for users
RUN echo "ALL ALL = (ALL) NOPASSWD: ALL" > /etc/sudoers.d/all
#COPY sudoers /etc/sudoers.d/bar
#RUN usermod -aG sudo jenkins
#RUN usermod -aG sudo test
