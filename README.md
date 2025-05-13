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


## Authenticateion Encryption
### AES-CBC-HMAC
  * Encrypt-then-MAC
    * AES-CBC-HMAC 구조

      ![alt text](aes-cbc.png)
      ![alt text](aes-cbc-hmac.png)

```
MAC-then-Encrypt
  vaudenary padding oracle attack 취약점 존재
```

### All-in-one : AEAD (Authenticated encryption with associated data)
스트림 방식이다.

  * AES_GCM
    * Galois/Counter Mode
      * 논스 + counter -> IV 로 취급받기도 함
    * GMAC 
      * GHASH 사용 (almost XORed universal hash, AXU)
      * one-time MAC -> many-time MAC (웨그먼-카터 기법)

    * 전체구조
      ![alt text](aes_gcm.png)

  * ChaCha20-Poly1305
    * ChaCha20
      * key + nonce + counter -> 64byte blocks
      * inner block -> 16 words
      * 20 round -> 1 round call QR(Quarter Round) -> 1QR call Add, Rotate, XOR

      ![alt text, align=center](image.png)
      ![alt text](image-1.png)
      ![alt text](image-3.png)
      ![alt text](image-4.png)
    * Poly1305
      * acc = 0, r -> s (연관 데이터?)
      ![alt text](image-5.png)

    * 전체구조
      ![alt text](image-6.png)

  ```
  논스 오용 방지 : CIV 도입
  ```
