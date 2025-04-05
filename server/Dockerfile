FROM gcc:latest

RUN apt-get update && apt-get install -y \
    libpq-dev \
    libcjson-dev \
    libcurl4-openssl-dev \
    && apt-get clean

WORKDIR /app

COPY . .

# RUN gcc -o server server.c -lpq -lcjson
RUN gcc -o server server.c -I/usr/include/postgresql -lpq -lcjson


CMD ["./server"]
