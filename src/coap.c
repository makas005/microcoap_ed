#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "coap.h"
#include "byte_order.h"

#ifdef DEBUG
void coap_dumpHeader(coap_header_t *hdr)
{
    printf("Header:\n");
    printf("  ver  0x%02X\n", hdr->ver);
    printf("  t    0x%02X\n", hdr->t);
    printf("  tkl  0x%02X\n", hdr->tkl);
    printf("  code 0x%02X\n", hdr->code);
    printf("  id   0x%02X%02X\n", (uint8_t)((hdr->id) >> 8), (uint8_t)(hdr->id));
}
#endif

#ifdef DEBUG
void coap_dump(const uint8_t *buf, size_t buflen, bool bare)
{
    if (bare)
    {
        while(buflen--)
            printf("%02X%s", *buf++, (buflen > 0) ? " " : "");
    }
    else
    {
        printf("Dump: ");
        while(buflen--)
            printf("%02X%s", *buf++, (buflen > 0) ? " " : "");
        printf("\n");
    }
}
#endif

int coap_parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (buflen < 4)
        return COAP_ERR_HEADER_TOO_SHORT;
    hdr->ver = (buf[0] & 0xC0) >> 6;
    if (hdr->ver != 1)
        return COAP_ERR_VERSION_NOT_1;
    hdr->t = (buf[0] & 0x30) >> 4;
    hdr->tkl = buf[0] & 0x0F;
    hdr->code = buf[1];
    hdr->id = endian_load16(uint16_t, &buf[2]);
    return 0;
}

int coap_parseToken(coap_buffer_t *tokbuf, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (hdr->tkl == 0)
    {
        tokbuf->p = NULL;
        tokbuf->len = 0;
        return 0;
    }
    else
    if (hdr->tkl <= 8)
    {
        if (4U + hdr->tkl > buflen)
            return COAP_ERR_TOKEN_TOO_SHORT;   // tok bigger than packet
        tokbuf->p = buf+4;  // past header
        tokbuf->len = hdr->tkl;
        return 0;
    }
    else
    {
        // invalid size
        return COAP_ERR_TOKEN_TOO_SHORT;
    }
}

// advances p
int coap_parseOption(coap_option_t *option, uint16_t *running_delta, const uint8_t **buf, size_t buflen)
{
    const uint8_t *p = *buf;
    uint8_t headlen = 1;
    uint16_t len, delta;

    if (buflen < headlen) // too small
        return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;

    delta = (p[0] & 0xF0) >> 4;
    len = p[0] & 0x0F;

    // These are untested and may be buggy
    if (delta == 13)
    {
        headlen++;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        delta = p[1] + 13;
        p++;
    }
    else
    if (delta == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        delta = ((p[1] << 8) | p[2]) + 269;
        p+=2;
    }
    else
    if (delta == 15)
        return COAP_ERR_OPTION_DELTA_INVALID;

    if (len == 13)
    {
        headlen++;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        len = p[1] + 13;
        p++;
    }
    else
    if (len == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        len = ((p[1] << 8) | p[2]) + 269;
        p+=2;
    }
    else
    if (len == 15)
        return COAP_ERR_OPTION_LEN_INVALID;

    if ((p + 1 + len) > (*buf + buflen))
        return COAP_ERR_OPTION_TOO_BIG;

    option->num = delta + *running_delta;
    option->buf.p = p+1;
    option->buf.len = len;

    // advance buf
    *buf = p + 1 + len;
    *running_delta += delta;

    return 0;
}

// http://tools.ietf.org/html/rfc7252#section-3.1
int coap_parseOptionsAndPayload(coap_option_t *options, uint8_t *numOptions, coap_buffer_t *payload, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    size_t optionIndex = 0;
    uint16_t delta = 0;
    const uint8_t *p = buf + 4 + hdr->tkl;
    const uint8_t *end = buf + buflen;
    int rc;
    if (p > end)
        return COAP_ERR_OPTION_OVERRUNS_PACKET;   // out of bounds

    // 0xFF is payload marker
    while((optionIndex < *numOptions) && (p < end) && (*p != 0xFF))
    {
        if (0 != (rc = coap_parseOption(&options[optionIndex], &delta, &p, end-p)))
            return rc;
        optionIndex++;
    }
    *numOptions = optionIndex;

    if (p+1 < end && *p == 0xFF)  // payload marker
    {
        payload->p = p+1;
        payload->len = end-(p+1);
    }
    else
    {
        payload->p = NULL;
        payload->len = 0;
    }

    return 0;
}

#ifdef DEBUG
void coap_dumpOptions(coap_option_t *opts, size_t numopt)
{
    size_t i;
    printf(" Options:\n");
    for (i=0;i<numopt;i++)
    {
        printf("  0x%02X [ ", opts[i].num);
        coap_dump(opts[i].buf.p, opts[i].buf.len, true);
        printf(" ]\n");
    }
}
#endif

#ifdef DEBUG
void coap_dumpPacket(coap_packet_t *pkt)
{
    coap_dumpHeader(&pkt->hdr);
    coap_dumpOptions(pkt->opts, pkt->numopts);
    printf("Payload: ");
    coap_dump(pkt->payload.p, pkt->payload.len, true);
    printf("\n");
}
#endif

int coap_parse(coap_packet_t *pkt, const uint8_t *buf, size_t buflen)
{
    int rc;

    if (0 != (rc = coap_parseHeader(&pkt->hdr, buf, buflen)))
        return rc;
    if (0 != (rc = coap_parseToken(&pkt->tok, &pkt->hdr, buf, buflen)))
        return rc;
    pkt->numopts = MAXOPT;
    if (0 != (rc = coap_parseOptionsAndPayload(pkt->opts, &(pkt->numopts), &(pkt->payload), &pkt->hdr, buf, buflen)))
        return rc;
    return 0;
}

// options are always stored consecutively, so can return a block with same option num
const coap_option_t *coap_findOptions(const coap_packet_t *pkt, uint8_t num, uint8_t *count)
{
    // FIXME, options is always sorted, can find faster than this
    size_t i;
    const coap_option_t *first = NULL;
    *count = 0;
    for (i=0;i<pkt->numopts;i++)
    {
        if (pkt->opts[i].num == num)
        {
            if (NULL == first)
                first = &pkt->opts[i];
            (*count)++;
        }
        else
        {
            if (NULL != first)
                break;
        }
    }
    return first;
}

coap_blocksize_t coap_option_blockwise_get_szx(const coap_option_t *block_option) {
    uint8_t lsb = block_option->buf.p[block_option->buf.len - 1]; // last byte in option buffer stores szx.
    return (lsb & 0x07); //Last three bits are szx encoded.
}

uint32_t coap_option_blockwise_get_num(const coap_option_t *block_option)
{
    uint32_t num = 0;
    if(block_option->buf.len == 1) {
        //num is 4 MSBs
        num = block_option->buf.p[0] >> 4;
    }
    else if (block_option->buf.len == 2) {
        //num is complete first byte and 4 MSBs of second byte
        num = (block_option->buf.p[0] << 8) & (block_option->buf.p[1] >> 4);
    }
    else {
        //num two first bytes complete and 4 MSBs of third byte
        num = (block_option->buf.p[0] << 16) & (block_option->buf.p[1] << 8) & (block_option->buf.p[2] >> 4);
    }
    return num;
}

bool coap_option_blockwise_get_m(const coap_option_t *block_option)
{
    uint8_t lsb = block_option->buf.p[block_option->buf.len - 1]; // last byte in option buffer stores m.
    return (lsb & 0x08); //Fourth last bit is m flag.
}

int coap_buffer_to_string(char *strbuf, size_t strbuflen, const coap_buffer_t *buf)
{
    if (buf->len+1 > strbuflen)
        return COAP_ERR_BUFFER_TOO_SMALL;
    memcpy(strbuf, buf->p, buf->len);
    strbuf[buf->len] = 0;
    return 0;
}

coap_error_t coap_build(uint8_t *buf, size_t *buflen, const coap_packet_t *pkt)
{
    size_t opts_len = 0;
    size_t i;
    uint8_t *p;
    uint16_t running_delta = 0;

    // build header
    if (*buflen < (4U + pkt->hdr.tkl))
        return COAP_ERR_BUFFER_TOO_SMALL;
    if (pkt->hdr.ver != 1) {
        return COAP_ERR_VERSION_NOT_1;
    }

    buf[0] = (pkt->hdr.ver & 0x03) << 6;
    buf[0] |= (pkt->hdr.t & 0x03) << 4;
    buf[0] |= (pkt->hdr.tkl & 0x0F);
    buf[1] = pkt->hdr.code;
    endian_store16(&buf[2], pkt->hdr.id);
    
    // inject token
    p = buf + 4;
    if(pkt->hdr.tkl > 8) {
        return COAP_ERR_TOKEN_TOO_LONG;
    }

    if ((pkt->hdr.tkl > 0) && (pkt->hdr.tkl != pkt->tok.len))
        return COAP_ERR_TOKEN_LENGTH_MISMATCH;
    
    if (pkt->hdr.tkl > 0)
        memcpy(p, pkt->tok.p, pkt->hdr.tkl);

    p += pkt->hdr.tkl;

    // // http://tools.ietf.org/html/rfc7252#section-3.1
    // inject options
    uint8_t option_indices[MAXOPT] = {0};
    coap_order_options(pkt->opts, pkt->numopts, option_indices);

    for (i=0;i<pkt->numopts;i++)
    {
        uint32_t optDelta;
        uint8_t len, delta = 0;

        if (((size_t)(p-buf)) > *buflen)
             return COAP_ERR_BUFFER_TOO_SMALL;
        optDelta = pkt->opts[option_indices[i]].num - running_delta;
        coap_option_nibble(optDelta, &delta);
        coap_option_nibble((uint32_t)pkt->opts[option_indices[i]].buf.len, &len);

        *p++ = (0xFF & (delta << 4 | len));
        if (delta == 13)
        {
            *p++ = (optDelta - 13);
        }
        else
        if (delta == 14)
        {
            *p++ = ((optDelta-269) >> 8);
            *p++ = (0xFF & (optDelta-269));
        }
        if (len == 13)
        {
            *p++ = (pkt->opts[option_indices[i]].buf.len - 13);
        }
        else
        if (len == 14)
  	    {
            *p++ = (pkt->opts[option_indices[i]].buf.len >> 8);
            *p++ = (0xFF & (pkt->opts[option_indices[i]].buf.len-269));
        }

        memcpy(p, pkt->opts[option_indices[i]].buf.p, pkt->opts[option_indices[i]].buf.len);
        p += pkt->opts[option_indices[i]].buf.len;
        running_delta = pkt->opts[option_indices[i]].num;
    }

    opts_len = (p - buf) - 4;   // number of bytes used by options

    if (pkt->payload.len > 0)
    {
        if (*buflen < 4 + 1 + pkt->payload.len + opts_len)
            return COAP_ERR_BUFFER_TOO_SMALL;
        buf[4 + opts_len] = 0xFF;  // payload marker
        memcpy(buf+5 + opts_len, pkt->payload.p, pkt->payload.len);
        *buflen = opts_len + 5 + pkt->payload.len;
    }
    else
        *buflen = opts_len + 4;
    return COAP_ERR_NONE;
}

bool coap_header_init(coap_packet_t *pkt, const coap_msgtype_t type, const coap_code_t method, const uint16_t id)
{
    //type options out ouf bound
    if (type < 0 || type > 3) {
        return false;
    }

    pkt->hdr.ver = 1; //Version must be always 1 according to https://www.rfc-editor.org/rfc/rfc7252#section-3
    pkt->hdr.t = type;
    pkt->hdr.code = method;
    pkt->hdr.id = id;
    return true;
}

void coap_header_add_token(coap_packet_t *pkt, const uint8_t* token, const size_t len)
{
    pkt->hdr.tkl = len;
    pkt->tok.len = len;
    pkt->tok.p = token;
}


void coap_add_option(coap_packet_t *pkt, coap_option_num_t option, uint8_t* option_pt, size_t option_pt_len)
{
    pkt->opts[pkt->numopts].buf.p = option_pt;
    pkt->opts[pkt->numopts].buf.len = option_pt_len;
    pkt->opts[pkt->numopts].num = option;
    pkt->numopts++;
}

uint8_t coap_make_option_blockwise(uint8_t *option_buffer, const coap_blocksize_t szx, const bool m, const uint32_t num)
{
    if(szx > 6 || szx < 0 || num > 1048576)
    {
        return 0;
    }

    uint8_t option_length = 0;
    if(num < 16) {
        /* According to RFC7959 this is the option representation for 4 bit num:
           0 1 2 3 4 5 6 7
          +-+-+-+-+-+-+-+-+
          |  NUM  |M| SZX |
          +-+-+-+-+-+-+-+-+
        */
        option_length = 1;
        *option_buffer = ((num & 0x0F) << 4) | (m << 3) | (szx & 0x07);
    }
    else if (num < 4096) {
        /* According to RFC7959 this is the option representation for 12 bit num:
           0                   1
           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |          NUM          |M| SZX |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */
        option_length = 2;
        *option_buffer = (num >> 4) & 0xFF;
        *(option_buffer + 1) = ((num & 0x0F) << 4) | (m << 3) | (szx & 0x07);
    }
    else {
        /* According to RFC7959 this is the option representation for 12 bit num:
           0                   1                   2
           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |                   NUM                 |M| SZX |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
        */
        option_length = 3;
        *option_buffer = (num >> 12) & 0xFF;
        *(option_buffer + 1) = (num >> 4) & 0xFF;
        *(option_buffer + 2) = ((num & 0x0F) << 4) | (m << 3) | (szx & 0x07);
    }
    return option_length;
}

void coap_option_nibble(uint32_t value, uint8_t *nibble)
{
    if (value<13)
    {
        *nibble = (0xFF & value);
    }
    else
    if (value<=0xFF+13)
    {
        *nibble = 13;
    } else if (value<=0xFFFF+269)
    {
        *nibble = 14;
    }
}

int coap_make_response(coap_rw_buffer_t *scratch, coap_packet_t *pkt, const uint8_t *content, size_t content_len, uint16_t msgid, const coap_buffer_t* tok, coap_code_t rspcode, coap_content_type_t content_type)
{
    pkt->hdr.ver = 0x01;
    pkt->hdr.t = COAP_TYPE_ACK;
    pkt->hdr.tkl = 0;
    pkt->hdr.code = rspcode;
    pkt->hdr.id = msgid;
    pkt->numopts = 0;

    // need token in response
    if (tok) {
        pkt->hdr.tkl = tok->len;
        pkt->tok = *tok;
    }

    if (content_type != COAP_CONTENTTYPE_NONE) {
        pkt->numopts = 1;
    // safe because 1 < MAXOPT
    pkt->opts[0].num = COAP_OPTION_CONTENT_FORMAT;
    pkt->opts[0].buf.p = scratch->p;
    if (scratch->len < 2)
        return COAP_ERR_BUFFER_TOO_SMALL;
    scratch->p[0] = ((uint16_t)content_type & 0xFF00) >> 8;
    scratch->p[1] = ((uint16_t)content_type & 0x00FF);
    pkt->opts[0].buf.len = 2;
    }
    pkt->payload.p = content;
    pkt->payload.len = content_len;
    return 0;
}

void coap_order_options(const coap_option_t *opts, const uint8_t num_opts, uint8_t* ordered_indices)
{
    if(num_opts == 1) {
        *ordered_indices = 0;
        return;
    }

    uint8_t i, key;
    int8_t j;
    /*initialize ordered_incices in range from 0...num_opts,
    reflecting the current order in opts[]*/
    for(i = 0; i < num_opts; i++) {
        ordered_indices[i] = i;
    }


    for (i = 1; i < num_opts; i++) {
        key = opts[ordered_indices[i]].num;
        j = i - 1;
  
        /* Move elements of arr[0..i-1], that are
          greater than key, to one position ahead
          of their current position */
        while (j >= 0 && opts[ordered_indices[j]].num > key) {
            ordered_indices[j + 1] = ordered_indices[j];
            j--;
        }
        ordered_indices[j + 1] = i;
    }
}