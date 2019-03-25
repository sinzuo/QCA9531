#ifndef __BASE64_H__
#define __BASE64_H__

int base64_decode(unsigned char *out,const unsigned char *in, int inlen, int maxlen);

int base64_encode(unsigned char *out, const unsigned char *in, int inlen, int maxlen);

#endif
