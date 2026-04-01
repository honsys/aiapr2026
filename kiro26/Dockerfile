FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && apt-get install -y -q \
    build-essential libcurl4-openssl-dev \
    python3-dev python3-venv python3-pip \
    patchelf ccache curl git \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Copy gem binary if present, otherwise skip gem_build
RUN if [ -f gem ]; then cp gem /usr/local/bin/gem && chmod +x /usr/local/bin/gem; fi

RUN make deps && make all

EXPOSE 8099

CMD ["./hontrade"]
