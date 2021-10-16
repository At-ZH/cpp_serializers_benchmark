# CPP serializers benchmark 

Add capnproto in the benchmark. Now, there's some fault in the case of capnproto, never to use.

### tested libraries
* [bitsery](https://github.com/fraillt/bitsery) 5.2.1
* [boost](https://www.boost.org/) 1.77.0
* [cereal](https://uscilab.github.io/cereal/) 1.3.0
* [flatbuffers](https://google.github.io/flatbuffers/) 2.0.0
* [yas](https://github.com/niXman/yas) 7.1.0
* [protobuf](https://developers.google.com/protocol-buffers/) 3.17.3
* [zpp](https://github.com/eyalz800/serializer) 0.5
* [msgpack](https://github.com/msgpack/msgpack-c) 3.3.0
* [capnproto](https://github.com/capnproto/capnproto.git) 0.9.1


## Building & testing

1. Build project
    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```
2. Run tests with `ctest -VV` **OR**
3. Generate testing results *(requires nodejs)*
    ```bash
    cd ../tools/
    npm install
    npm start
    ```
# cpp_serializers_benchmark
