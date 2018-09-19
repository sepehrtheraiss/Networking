FROM gcc:7.3.0
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN gcc -o server server.c
CMD ["./server"]
