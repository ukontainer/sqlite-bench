FROM ubuntu:16.04

MAINTAINER Akira Moroo <retrage01@gmail.com>

RUN apt-get update && \
    apt-get install -y build-essential make git && \
    apt-get clean

RUN cd /usr/src && git clone https://github.com/retrage/sqlite-bench.git
WORKDIR /usr/src/sqlite-bench
RUN make
ENV PATH /usr/src/sqlite-bench:$PATH

ENTRYPOINT ["sqlite-bench", "--histogram=1"]
CMD ["--help"]
