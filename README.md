microcoap_ed
============

This is a stripped down and enhanced version of [1248's](https://github.com/1248) [```microcoap```](https://github.com/1248/microcoap) library, focues on **e**ncoding and **d**ecoding CoAP (therefore the name microcoap_**ed**).

For general information on the constrained applications protocol (CoAP), see http://tools.ietf.org/html/rfc7252.

All functions regarding network operation are removed, leaving only encode and decode functions.

## Licenses
Following libraries or parts of libraries are used (with licenses):
|Library|License|Description|
|---|---|---|
|Byte Order Conversion|[Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0) / [BSD 2-Clause](https://github.com/staropram/cantcoap/blob/master/LICENSE)|Content of byte_order.h original by Furuhashi Sadayuki (MessagePack) under Apache License, Version 2.0, then modified by Ashley Mills (CantCoap) under BSD 2-Clause License|
