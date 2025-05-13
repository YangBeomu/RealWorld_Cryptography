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

      ![aes-cbc](https://github.com/user-attachments/assets/552b4165-f176-4cd9-a933-f6329ca61d24)
      ![aes-cbc-hmac](https://github.com/user-attachments/assets/9223ebc5-3ecf-4ab5-b930-777c1e3780c7)

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
      ![aes_gcm](https://github.com/user-attachments/assets/acaa527e-102c-4d02-a4fc-04794fc22c66)

  * ChaCha20-Poly1305
    * ChaCha20
      * key + nonce + counter -> 64byte blocks
      * inner block -> 16 words
      * 20 round -> 1 round call QR(Quarter Round) -> 1QR call Add, Rotate, XOR

      ![chacha20_enc1](https://github.com/user-attachments/assets/6b0dfe47-4c4b-475b-9400-419a5b213c5d)
      ![chacha20_enc2](https://github.com/user-attachments/assets/727f8659-6292-4dff-87b5-c02efaf00063)
      ![chacha20_enc3](https://github.com/user-attachments/assets/4e70d5ec-badf-4d3f-bead-0d28368b88c7)
      ![chacha20_enc4](https://github.com/user-attachments/assets/1d7bff5b-9e85-4761-8b8e-a3c9b62cdbfe)

    * Poly1305
      * acc = 0, r -> s (연관 데이터?)
      ![poly1305](https://github.com/user-attachments/assets/d15107f1-3e0d-4154-8dd3-165928353e18)

    * 전체구조
      ![chacah20_poly1305](https://github.com/user-attachments/assets/07e47a00-87f7-4714-b387-f87883ca2bf2)

  ```
  논스 오용 방지 : CIV 도입
  ```
