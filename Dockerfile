FROM ubuntu:latest AS build
WORKDIR /app

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    libcpprest-dev \
    libboost-all-dev \
    libssl-dev \
    cmake \
    git

COPY . .

# Compile
RUN make


FROM debian:trixie-slim

COPY --from=build /app/udpecho /app/udpserver /app/udpclient /app/tcpclient /app/tcpserver /bin