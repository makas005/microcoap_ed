/* ---------------------------------------------------------------------------------------------------------------------
Parts of this file (specifically enum coap_code_t) are adapted from the library 'cantcoap' by 'staropram' on
github (https://github.com/staropram/cantcoap). This library is published under BSD-2-Clause license with the following
content:

Copyright (c) 2013, Ashley Mills.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------------------------------------------------
*/

#ifndef COAP_H
#define COAP_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#define MAXOPT 16

//http://tools.ietf.org/html/rfc7252#section-3
typedef struct
{
    uint8_t ver;                /**< CoAP version number */
    uint8_t t;                  /**< CoAP Message Type */
    uint8_t tkl;                /**< Token length: indicates length of the Token field */
    uint8_t code;               /**< CoAP status code. Can be request (0.xx), success reponse (2.xx), 
                                 * client error response (4.xx), or rever error response (5.xx) 
                                 * For possible values, see http://tools.ietf.org/html/rfc7252#section-12.1 */
    uint16_t id;                /**< Message ID*/
} coap_header_t;

typedef struct
{
    const uint8_t *p;
    size_t len;
} coap_buffer_t;

typedef struct
{
    uint8_t *p;
    size_t len;
} coap_rw_buffer_t;

typedef struct
{
    uint8_t num;                /* Option number. See http://tools.ietf.org/html/rfc7252#section-5.10 */
    coap_buffer_t buf;          /* Option value */
} coap_option_t;

typedef struct
{
    coap_header_t hdr;          /* Header of the packet */
    coap_buffer_t tok;          /* Token value, size as specified by hdr.tkl */
    uint8_t numopts;            /* Number of options */
    coap_option_t opts[MAXOPT]; /* Options of the packet. For possible entries see
                                 * http://tools.ietf.org/html/rfc7252#section-5.10 */
    coap_buffer_t payload;      /* Payload carried by the packet */
} coap_packet_t;

/////////////////////////////////////////

//http://tools.ietf.org/html/rfc7252#section-12.2
typedef enum
{
    COAP_OPTION_IF_MATCH = 1,
    COAP_OPTION_URI_HOST = 3,
    COAP_OPTION_ETAG = 4,
    COAP_OPTION_IF_NONE_MATCH = 5,
    COAP_OPTION_OBSERVE = 6,
    COAP_OPTION_URI_PORT = 7,
    COAP_OPTION_LOCATION_PATH = 8,
    COAP_OPTION_URI_PATH = 11,
    COAP_OPTION_CONTENT_FORMAT = 12,
    COAP_OPTION_MAX_AGE = 14,
    COAP_OPTION_URI_QUERY = 15,
    COAP_OPTION_ACCEPT = 17,
    COAP_OPTION_LOCATION_QUERY = 20,
    COAP_OPTION_BLOCK_2 = 23,
    COAP_OPTION_BLOCK_1 = 27,
    COAP_OPTION_PROXY_URI = 35,
    COAP_OPTION_PROXY_SCHEME = 39
} coap_option_num_t;

//http://tools.ietf.org/html/rfc7252#section-12.1.1
typedef enum
{
    COAP_TYPE_CON = 0,
    COAP_TYPE_NONCON = 1,
    COAP_TYPE_ACK = 2,
    COAP_TYPE_RESET = 3
} coap_msgtype_t;

//http://tools.ietf.org/html/rfc7252#section-5.2
//http://tools.ietf.org/html/rfc7252#section-12.1.2
#define MAKE_RSPCODE(clas, det) ((clas << 5) | (det))
typedef enum
{
    COAP_EMPTY=0x00,
    COAP_GET,
    COAP_POST,
    COAP_PUT,
    COAP_DELETE,
    COAP_LASTMETHOD=0x1F,
    COAP_CREATED=0x41,
    COAP_DELETED,
    COAP_VALID,
    COAP_CHANGED,
    COAP_CONTENT,
    COAP_BAD_REQUEST=0x80,
    COAP_UNAUTHORIZED,
    COAP_BAD_OPTION,
    COAP_FORBIDDEN,
    COAP_NOT_FOUND,
    COAP_METHOD_NOT_ALLOWED,
    COAP_NOT_ACCEPTABLE,
    COAP_PRECONDITION_FAILED=0x8C,
    COAP_REQUEST_ENTITY_TOO_LARGE=0x8D,
    COAP_UNSUPPORTED_CONTENT_FORMAT=0x8F,
    COAP_INTERNAL_SERVER_ERROR=0xA0,
    COAP_NOT_IMPLEMENTED,
    COAP_BAD_GATEWAY,
    COAP_SERVICE_UNAVAILABLE,
    COAP_GATEWAY_TIMEOUT,
    COAP_PROXYING_NOT_SUPPORTED,
    COAP_UNDEFINED_CODE=0xFF
} coap_code_t;

//http://tools.ietf.org/html/rfc7252#section-12.3
typedef enum
{
    COAP_CONTENTTYPE_NONE = -1, // bodge to allow us not to send option block
    COAP_CONTENTTYPE_TEXT_PLAIN = 0,
    COAP_CONTENTTYPE_APPLICATION_LINKFORMAT = 40,
    COAP_CONTENTTYPE_APPLICATION_XML = 41,
    COAP_CONTENTTYPE_APPLICATION_OCTECT_STREAM = 42,
    COAP_CONTENTTYPE_APPLICATION_EXI = 47,
    COAP_CONTENTTYPE_APPLICATION_JSON = 50,
} coap_content_type_t;

///////////////////////

typedef enum
{
    COAP_ERR_NONE = 0,
    COAP_ERR_HEADER_TOO_SHORT = 1,
    COAP_ERR_VERSION_NOT_1 = 2,
    COAP_ERR_TOKEN_TOO_SHORT = 3,
    COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER = 4,
    COAP_ERR_OPTION_TOO_SHORT = 5,
    COAP_ERR_OPTION_OVERRUNS_PACKET = 6,
    COAP_ERR_OPTION_TOO_BIG = 7,
    COAP_ERR_OPTION_LEN_INVALID = 8,
    COAP_ERR_BUFFER_TOO_SMALL = 9,
    COAP_ERR_UNSUPPORTED = 10,
    COAP_ERR_OPTION_DELTA_INVALID = 11,
    COAP_ERR_TOKEN_LENGTH_MISMATCH = 12,    /**< Only used in building coap, when tkl in header mismatch with token buffer */
    COAP_ERR_TOKEN_TOO_LONG = 13           /**< Only used in building coap, when tkl in header > 8 */
} coap_error_t;

///////////////////////

typedef int (*coap_endpoint_func)(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo);
#define MAX_SEGMENTS 2  // 2 = /foo/bar, 3 = /foo/bar/baz
typedef struct
{
    int count;
    const char *elems[MAX_SEGMENTS];
} coap_endpoint_path_t;

typedef struct
{
    coap_code_t method;               /* (i.e. POST, PUT or GET) */
    coap_endpoint_func handler;         /* callback function which handles this 
                                         * type of endpoint (and calls 
                                         * coap_make_response() at some point) */
    const coap_endpoint_path_t *path;   /* path towards a resource (i.e. foo/bar/) */ 
    const char *core_attr;              /* the 'ct' attribute, as defined in RFC7252, section 7.2.1.:
                                         * "The Content-Format code "ct" attribute 
                                         * provides a hint about the 
                                         * Content-Formats this resource returns." 
                                         * (Section 12.3. lists possible ct values.) */
} coap_endpoint_t;


///////////////////////
coap_error_t coap_build(uint8_t *buf, size_t *buflen, const coap_packet_t *pkt);

/// @brief Initializes header version, type, code(method) and message id
/// @param pkt Packet pointer to store data to
/// @param type Message type, can be:
///     @arg     COAP_TYPE_CON: Confirmable Request
///     @arg     COAP_TYPE_NONCON: Non-Confirmable Request
///     @arg     COAP_TYPE_ACK: Acknowledgement response
///     @arg     COAP_TYPE_RESET: Reset response
/// @param method Request method(code), must be one of coap_code_t
/// @param id Message ID
/// @return True if init was successful, false if not (Parameters invalid)
bool coap_header_init(coap_packet_t *pkt, const coap_msgtype_t type, const coap_code_t method, const uint16_t id);

/// @brief Adds token to packet header
/// @param pkt Packet pointer to store data to
/// @param token Pointer to token array
/// @param len Length of token
void coap_header_add_token(coap_packet_t *pkt, const uint8_t* token, const size_t len);

/// @brief Add an option to the packet
/// @param pkt Packet pointer to store data to
/// @param option Option definition/option number
/// @param option_pt Option pointer to buffer that is stored as option bytes
/// @param option_pt_len Length of option bytes
void coap_add_option(coap_packet_t *pkt, coap_option_num_t option, uint8_t* option_pt, size_t option_pt_len);

typedef enum  {
    COAP_BLOCKSIZE_16 = 0,
    COAP_BLOCKSIZE_32,
    COAP_BLOCKSIZE_64,
    COAP_BLOCKSIZE_128,
    COAP_BLOCKSIZE_256,
    COAP_BLOCKSIZE_512,
    COAP_BLOCKSIZE_1024
} coap_blocksize_t;

/// @brief Creates option BLOCK1 or BLOCK2
/// Takes information about block size and creates option representation of it. The output size varies, depending on how
/// large num is. Output length varies from 1 to 3 bytes. 
/// @param[out] option_buffer Buffer to store created option to. MUST be at least 1 byte if num < 15,
/// 2 byte if num < 4096, and 3 byte if num < 1.048.576. Num above 1.048.576 is not supported by RFC7959.
/// @param[in] szx Size of the block. Can be one of the following values:
///     Can be one of the following values:
///         @arg COAP_BLOCKSIZE_16: 16 byte Block size
///         @arg COAP_BLOCKSIZE_32: 32 byte Block size
///         @arg COAP_BLOCKSIZE_64: 64 byte Block size
///         @arg COAP_BLOCKSIZE_128: 128 byte Block size
///         @arg COAP_BLOCKSIZE_256: 256 byte Block size
///         @arg COAP_BLOCKSIZE_512: 512 byte Block size
///         @arg COAP_BLOCKSIZE_1024: 1024 byte Block size
/// @param[in] m More Flag. If set, there are more messages after the current one.
/// @param[in] num Block number. Block 0 is starting block.
/// @return Size of the block option.
uint8_t coap_make_option_blockwise(uint8_t *option_buffer, const coap_blocksize_t szx, const bool m, const uint32_t num);

void coap_dumpPacket(coap_packet_t *pkt);
int coap_parse(coap_packet_t *pkt, const uint8_t *buf, size_t buflen);
int coap_buffer_to_string(char *strbuf, size_t strbuflen, const coap_buffer_t *buf);
const coap_option_t *coap_findOptions(const coap_packet_t *pkt, uint8_t num, uint8_t *count);

void coap_dump(const uint8_t *buf, size_t buflen, bool bare);
int coap_make_response(coap_rw_buffer_t *scratch, coap_packet_t *pkt, const uint8_t *content, size_t content_len, uint16_t msgid, const coap_buffer_t* tok, coap_code_t rspcode, coap_content_type_t content_type);
int coap_handle_req(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt);
void coap_option_nibble(uint32_t value, uint8_t *nibble);
void coap_order_options(const coap_option_t *opts, const uint8_t num_opts, uint8_t* ordered_indices);

#ifdef __cplusplus
}
#endif

#endif
