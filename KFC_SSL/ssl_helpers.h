#ifndef ssl_helpers_h
#define ssl_helpers_h

// ------------
// SSL helpers
// ------------
BIGNUM* Load_RSA_BIGNUM(TStream& Stream);

void Save_RSA_BIGNUM(const BIGNUM* pBN, TStream& Stream);

#endif // ssl_helpers_h