#include <stdio.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <stdlib.h>
#include <string.h>
int main()
{
  int i = 0;
  char r1[64];
  for (i = 0; i < 64; i++) {
      r1[i] = rand() % 10 + 'a';
    }
  char key1[64];
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, r1, strlen(r1));
  SHA256_Final(hash, &sha256);
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
      printf("%02x", hash[i]);
  }
  return 0;
}
