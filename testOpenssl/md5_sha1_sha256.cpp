#include <iostream>
#include <stdio.h>
#include <cstring>
#include <openssl/sha.h>   
#include <openssl/crypto.h>  
#include <openssl/md5.h>

using namespace std;  
  
const char *src_str = "hello world"; 

void printHexstr(const unsigned char *md, int len)  
{  
    int i = 0;  
    for (i = 0; i < len; i++)  
    {  
        printf("%02x", md[i]);  
    }  
    printf("\n");  
}  
  
void testMD5()
{
    printf("---------MD5 start-----\n");
    unsigned char md[MD5_DIGEST_LENGTH];  
    MD5((unsigned char *)src_str, strlen(src_str), md);
    printHexstr(md, MD5_DIGEST_LENGTH);  

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx,(char*)src_str,strlen(src_str));
    MD5_Final(md,&ctx);
    printHexstr(md, MD5_DIGEST_LENGTH);  
    printf("---------MD5 end-----\n\n");
}

void testSHA1()  
{
    printf("---------SHA1 start-----\n");
    SHA_CTX c;  
    unsigned char md[SHA_DIGEST_LENGTH];  
    SHA1((unsigned char *)src_str, strlen(src_str), md);  
    printHexstr(md, SHA_DIGEST_LENGTH);  
  
    SHA1_Init(&c);  
    SHA1_Update(&c, src_str, strlen(src_str));  
    SHA1_Final(md, &c);  
    OPENSSL_cleanse(&c, sizeof(c));  
    printHexstr(md, SHA_DIGEST_LENGTH);  
    printf("---------SHA1 end-----\n\n");
}


void testSHA256() 
{  
    printf("---------SHA256 start-----\n");
    SHA256_CTX c;  
    unsigned char md[SHA256_DIGEST_LENGTH];  
    SHA256((unsigned char *)src_str, strlen(src_str), md);  
    printHexstr(md, SHA256_DIGEST_LENGTH);  
  
    SHA256_Init(&c);  
    SHA256_Update(&c, src_str, strlen(src_str));  
    SHA256_Final(md, &c);  
    OPENSSL_cleanse(&c, sizeof(c));  
    printHexstr(md, SHA256_DIGEST_LENGTH); 
    printf("---------SHA256 end-----\n\n");        
}  
  
int main()  
{  
    testMD5();  
    testSHA1(); 
    testSHA256();  
    return 0;  
}