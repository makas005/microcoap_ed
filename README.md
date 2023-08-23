microcoap_ed
============

This is a stripped down and enhanced version of [1248's](https://github.com/1248) [```microcoap```](https://github.com/1248/microcoap) library, focues on **e**ncoding and **d**ecoding CoAP (therefore the name microcoap_**ed**).

For general information on the constrained applications protocol (CoAP), see http://tools.ietf.org/html/rfc7252.

All functions regarding network operation are removed, leaving only encode and decode functions.

## Building
microcoap_ed uses CMake as a build system. To use cmake create a build directory and navigate into it, then run 
`cmake <path_to_source>`. If you build directory is inside the projects root dir, use `cmake ..` for example.

CMake will use the default generator, this will most likely be Unix Makefiles on Unix systems or Visual Studio project 
files on Windows systems. To change use the `-G` option. If you want to build using mingw on Windows for example, run
`cmake -G "MinGW Makefiles" ..`, if you want to use Ninja run `cmake -G Ninja ..`.

CMake will now create the build files. Run the build tool you created the build files for 
(`make` or `ninja` for example), or use `cmake --build .` to let CMake run the build for you. Output library will be in
`build/src`.

## Running tests
To run the tests, run CMake with target group test: `cmake [-G "Your Generator"] -DTARGET_GROUP=test ..`.
Then build: `cmake --build .`.

You can then use ctest to run all tests by simply calling `ctest`.


## Licenses
Following libraries or parts of libraries are used (with licenses):
|Library|License|Description|
|---|---|---|
|Byte Order Conversion (as part of cantcoap)|[Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0) / [BSD 2-Clause](https://github.com/staropram/cantcoap/blob/master/LICENSE)|Content of byte_order.h original by Furuhashi Sadayuki (MessagePack) under Apache License, Version 2.0, then modified by Ashley Mills (CantCoap) under BSD 2-Clause License|
|cantcoap|[BSD 2-Clause](https://github.com/staropram/cantcoap/blob/master/LICENSE)|
