# RealWorld_Cryptography_C++_Example
## lib
### window
  * uri : https://slproweb.com/products/Win32OpenSSL.html

## MAC
### HMAC
  #### k1 || message -> hash1, hash1 || k2 -> result
    k1 = k ^ ipad (inter padding)
    k2 = k ^ opad (out ppadding)

### KMAC
  #### Encode(key, input, output lnegth) || cShake -> KMAC