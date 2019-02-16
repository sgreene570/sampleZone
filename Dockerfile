FROM fedora
MAINTAINER Stephen Greene (sgreene570@gmail.com)
RUN yum install -y git gcc make wget alsa-lib-devel ncurses-devel
RUN git clone https://github.com/sgreene570/sampleZone
RUN wget https://csh.rit.edu/~purple/test.wav
WORKDIR "/sampleZone"
RUN make
RUN cp sampleZone /bin
ENTRYPOINT ["sampleZone", "/test.wav"]
