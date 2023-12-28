/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2020-2023 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/
 
#include <sodium.h>
#include <Arduino.h>

#include "SRP.h"
#include "HAP.h"

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

SRP6A::SRP6A(){

  // initialize MPI structures
  
  mbedtls_mpi_init(&N);     
  mbedtls_mpi_init(&g);
  mbedtls_mpi_init(&s);
  mbedtls_mpi_init(&x);
  mbedtls_mpi_init(&v);
  mbedtls_mpi_init(&A);
  mbedtls_mpi_init(&b);
  mbedtls_mpi_init(&B);
  mbedtls_mpi_init(&S);
  mbedtls_mpi_init(&k);
  mbedtls_mpi_init(&u);
  mbedtls_mpi_init(&K);
  mbedtls_mpi_init(&M1);
  mbedtls_mpi_init(&M1V);
  mbedtls_mpi_init(&M2);
  mbedtls_mpi_init(&_rr);
  mbedtls_mpi_init(&t1);
  mbedtls_mpi_init(&t2);
  mbedtls_mpi_init(&t3);

  // load N and g into MPI structures
  
  mbedtls_mpi_read_string(&N,16,N3072);
  mbedtls_mpi_lset(&g,g3072);
    
}

//////////////////////////////////////

SRP6A::~SRP6A(){

  mbedtls_mpi_free(&N);     
  mbedtls_mpi_free(&g);
  mbedtls_mpi_free(&s);
  mbedtls_mpi_free(&x);
  mbedtls_mpi_free(&v);
  mbedtls_mpi_free(&A);
  mbedtls_mpi_free(&b);
  mbedtls_mpi_free(&B);
  mbedtls_mpi_free(&S);
  mbedtls_mpi_free(&k);
  mbedtls_mpi_free(&u);
  mbedtls_mpi_free(&K);
  mbedtls_mpi_free(&M1);
  mbedtls_mpi_free(&M1V);
  mbedtls_mpi_free(&M2);
  mbedtls_mpi_free(&_rr);
  mbedtls_mpi_free(&t1);
  mbedtls_mpi_free(&t2);
  mbedtls_mpi_free(&t3);
  
}

//////////////////////////////////////

void SRP6A::createVerifyCode(const char *setupCode, uint8_t *verifyCode, uint8_t *salt){

  TempBuffer<uint8_t> tBuf(80);             // temporary buffer for staging 
  TempBuffer<uint8_t> tHash(64);            // temporary buffer for storing SHA-512 results 
  char *icp;                                // storage for I:P

  // generate random salt, s

  randombytes_buf(salt,16);                 // generate 16 random bytes for salt

  // create I:P
  
  asprintf(&icp,"%s:%.3s-%.2s-%.3s",I,setupCode,setupCode+3,setupCode+5);

  // compute x = SHA512( s | SHA512( I | ":" | P ) )

  memcpy(tBuf,salt,16);                                          // write salt into first 16 bytes of staging buffer            
  mbedtls_sha512_ret((uint8_t *)icp,strlen(icp),tBuf+16,0);      // create hash of username:password and write into last 64 bytes of staging buffer
  mbedtls_sha512_ret(tBuf,80,tHash,0);                           // create second hash of salted, hashed username:password 
  mbedtls_mpi_read_binary(&x,tHash,64);                          // load hash result into x

  // compute v = g^x %N
  
  mbedtls_mpi_exp_mod(&v,&g,&x,&N,&_rr);                         // create verifier, v (_rr is an internal "helper" structure that mbedtls uses to speed up subsequent exponential calculations)
  mbedtls_mpi_write_binary(&v,verifyCode,384);                   // write v into verifyCode

  free(icp);
}

//////////////////////////////////////

void SRP6A::createPublicKey(const uint8_t *verifyCode, const uint8_t *salt){

  TempBuffer<uint8_t> tBuf(768);                  // temporary buffer for staging
  TempBuffer<uint8_t> tHash(64);                  // temporary buffer for storing SHA-512 results
  TempBuffer<uint8_t> privateKey(32);             // temporary buffer for generating private key random numbers

  // load stored salt, s, and verification code, v

  mbedtls_mpi_read_binary(&s,salt,16);            // load salt into s for use in later steps
  mbedtls_mpi_read_binary(&v,verifyCode,384);     // load verifyCode into v for use below

  // generate random private key, b
  
  randombytes_buf(privateKey,32);                 // generate 32 random bytes for private key                     
  mbedtls_mpi_read_binary(&b,privateKey,32);      // load private key into b
    
  // compute k = SHA512( N | PAD(g) )
  
  mbedtls_mpi_write_binary(&N,tBuf,384);          // write N into first half of staging buffer
  mbedtls_mpi_write_binary(&g,tBuf+384,384);      // write g into second half of staging buffer (fully padded with leading zeros)
  mbedtls_sha512_ret(tBuf,768,tHash,0);           // create hash of data
  mbedtls_mpi_read_binary(&k,tHash,64);           // load hash result into k  

  // compute B = (kv + g^b) %N
  
  mbedtls_mpi_mul_mpi(&t1,&k,&v);                 // t1 = k*v
  mbedtls_mpi_exp_mod(&t2,&g,&b,&N,&_rr);         // t2 = g^b %N
  mbedtls_mpi_add_mpi(&t3,&t1,&t2);               // t3 = t1 + t2
  mbedtls_mpi_mod_mpi(&B,&t3,&N);                 // B = t3 %N      = ACCESSORY PUBLIC KEY

}

//////////////////////////////////////

void SRP6A::createSessionKey(){

  uint8_t tBuf[768];    // temporary buffer for staging
  uint8_t tHash[64];    // temporary buffer for storing SHA-512 results

  // compute u = SHA512( PAD(A) | PAD(B) )
  
  mbedtls_mpi_write_binary(&A,tBuf,384);          // write A into first half of staging buffer
  mbedtls_mpi_write_binary(&B,tBuf+384,384);      // write B into second half of staging buffer
  mbedtls_sha512_ret(tBuf,768,tHash,0);           // create hash of data
  mbedtls_mpi_read_binary(&u,tHash,64);           // load hash result into mpi structure u

  // compute S = (Av^u)^b %N

  mbedtls_mpi_exp_mod(&t1,&v,&u,&N,&_rr);         // t1 = v^u %N
  mbedtls_mpi_mul_mpi(&t2,&A,&t1);                // t2 = A*t1
  mbedtls_mpi_mod_mpi(&t1,&t2,&N);                // t1 = t2 %N  (this is needed to reduce size of t2 before next calculation)
  mbedtls_mpi_exp_mod(&S,&t1,&b,&N,&_rr);         // S = t1^b %N

  // compute K = SHA512( S )
  
  mbedtls_mpi_write_binary(&S,tBuf,384);          // write S into staging buffer (only first half of buffer will be used)
  mbedtls_sha512_ret(tBuf,384,tHash,0);           // create hash of data
  mbedtls_mpi_read_binary(&K,tHash,64);           // load hash result into mpi structure K.  This is the SRP SHARED SECRET KEY

  mbedtls_mpi_write_binary(&K,sharedSecret,64);   // store SHARED SECRET in easy-to-use binary (uint8_t) format
  
}

//////////////////////////////////////

int SRP6A::verifyProof(){

  uint8_t tBuf[976];    // temporary buffer for staging
  uint8_t tHash[64];    // temporary buffer for storing SHA-512 results

  size_t count=0;       // total number of bytes for final hash  
  size_t sLen;
  
  mbedtls_mpi_write_binary(&N,tBuf,384);             // write N into staging buffer
  mbedtls_sha512_ret(tBuf,384,tHash,0);              // create hash of data
  mbedtls_sha512_ret(&g3072,1,tBuf,0);               // create hash of g, but place output directly into staging buffer

  for(int i=0;i<64;i++)                              // H(g) ->  H(g) XOR H(N), with results in first 64 bytes of staging buffer
    tBuf[i]^=tHash[i];
 
  mbedtls_sha512_ret((uint8_t *)I,strlen(I),tBuf+64,0);     // create hash of userName and concatenate result to end of staging buffer

  mbedtls_mpi_write_binary(&s,tBuf+128,16);           // concatenate s to staging buffer

  sLen=mbedtls_mpi_size(&A);                          // get actual size of A  
  mbedtls_mpi_write_binary(&A,tBuf+144,sLen);         // concatenate A to staging buffer.  Note A is NOT padded with leading zeros (so may be less than 384 bytes)
  count=144+sLen;                                     // total bytes written to staging buffer so far

  sLen=mbedtls_mpi_size(&B);                          // get actual size of B  
  mbedtls_mpi_write_binary(&B,tBuf+count,sLen);       // concatenate B to staging buffer.  Note B is NOT padded with leading zeros (so may be less than 384 bytes)
  count+=sLen;                                        // increment total bytes written to staging buffer
  
  mbedtls_mpi_write_binary(&K,tBuf+count,64);         // concatenate K to staging buffer (should always be 64 bytes since it is a hashed value)
  count+=64;                                          // final total of bytes written to staging buffer

  mbedtls_sha512_ret(tBuf,count,tHash,0);             // create hash of data
  mbedtls_mpi_read_binary(&M1V,tHash,64);             // load hash result into mpi structure M1V
  
  if(!mbedtls_mpi_cmp_mpi(&M1,&M1V))                  // cmp_mpi uses same logic as strcmp: returns 0 if EQUAL, otherwise +/- 1
    return(1);                                        // success - proof from HAP Client is verified
    
  return(0);
}

//////////////////////////////////////

void SRP6A::createProof(){

  uint8_t tBuf[512];    // temporary buffer for staging

  // compute M2 = H( A | M1 | K )
  
  mbedtls_mpi_write_binary(&A,tBuf,384);          // write A into staging buffer
  mbedtls_mpi_write_binary(&M1,tBuf+384,64);      // concatenate M1 (now verified) to staging buffer
  mbedtls_mpi_write_binary(&K,tBuf+448,64);       // concatenate K to staging buffer
  mbedtls_sha512_ret(tBuf,512,tBuf,0);            // create hash of data
  mbedtls_mpi_read_binary(&M2,tBuf,64);           // load hash results into mpi structure M2
  
}

//////////////////////////////////////

void SRP6A::print(mbedtls_mpi *mpi){
     
  size_t sLen;

  mbedtls_mpi_write_string(mpi,16,NULL,0,&sLen);
  TempBuffer<char> sBuf(sLen);
  mbedtls_mpi_write_string(mpi,16,sBuf,sLen,&sLen);
  
  Serial.printf("%d %s\n",(sLen-1)/2,sBuf.get());         // subtract 1 for null-terminator, and then divide by 2 to get number of bytes (e.g. 4F = 2 characters, but represents just one mpi byte)
}

//////////////////////////////////////

constexpr char SRP6A::N3072[];
constexpr char SRP6A::I[];
const uint8_t SRP6A::g3072;
