#ifndef CERTIFICATES_HPP
#define CERTIFICATES_HPP

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
  
extern const uint8_t mbedtls_ca_crt[];
extern const size_t mbedtls_ca_crt_len;
extern const uint8_t mbedtls_client_crt[];
extern const size_t mbedtls_client_crt_len;
extern const uint8_t mbedtls_client_key[];
extern const size_t mbedtls_client_key_len;

#ifdef __cplusplus
}
#endif

#endif