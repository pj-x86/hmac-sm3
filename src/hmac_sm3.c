#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sm3.h"

/**
https://en.wikipedia.org/wiki/HMAC

function hmac is
    input:
        key:        Bytes    // Array of bytes
        message:    Bytes    // Array of bytes to be hashed
        hash:       Function // The hash function to use (e.g. SHA-1)
        blockSize:  Integer  // The block size of the hash function (e.g. 64 bytes for SHA-1)
        outputSize: Integer  // The output size of the hash function (e.g. 20 bytes for SHA-1)

    // Compute the block sized key
    block_sized_key = computeBlockSizedKey(key, hash, blockSize)

    o_key_pad ← block_sized_key xor [0x5c blockSize]   // Outer padded key
    i_key_pad ← block_sized_key xor [0x36 blockSize]   // Inner padded key

    return  hash(o_key_pad ∥ hash(i_key_pad ∥ message))

function computeBlockSizedKey is
    input:
        key:        Bytes    // Array of bytes
        hash:       Function // The hash function to use (e.g. SHA-1)
        blockSize:  Integer  // The block size of the hash function (e.g. 64 bytes for SHA-1)

    // Keys longer than blockSize are shortened by hashing them
    if (length(key) > blockSize) then
        key = hash(key)

    // Keys shorter than blockSize are padded to blockSize by padding with zeros on the right
    if (length(key) < blockSize) then
        return  Pad(key, blockSize) // Pad key with zeros to make it blockSize bytes long

    return  key
 */

#define BLOCK_SIZE 64
#define HASH_SIZE 32

/* 功能：HMAC-SM3 摘要算法
 * 参数：
 *		data(in): 原始数据
 *		data_len(in): 原始数据的长度
 *		key(in) : 密钥
 *		key_len(in): 密钥长度
 *		output(out): 哈希值
 */
void hmac_sm3(const unsigned char *data, size_t data_len, const unsigned char *key, size_t key_len, unsigned char *output)
{
    unsigned char digest[HASH_SIZE] = {0};

    unsigned char buf[BLOCK_SIZE] = {0};
    unsigned char buf2[BLOCK_SIZE] = {0};
    char ipad = 0x36;
    char opad = 0x5c;

    bzero(buf, BLOCK_SIZE);
    bzero(buf2, BLOCK_SIZE);

    // 在密钥 key 后面补0来创建一个长为B(BLOCK_SIZE)的字符串。如果key的长度key_len大于BLOCK_SIZE，则先进行哈希运算，使其长度key_len=HASH_SIZE
    if (key_len > BLOCK_SIZE)
    {
        SM3Calc(key, key_len, digest);

        memcpy(buf, digest, HASH_SIZE);
    }
    else
    {
        memcpy(buf, key, key_len);
    }

    memcpy(buf2, buf, BLOCK_SIZE);

    // 将上一步生成的字符串与ipad(0x36)、opad(0x5c)做异或运算，形成结果字符串
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        *(buf + i) = *(buf + i) ^ ipad;
        *(buf2 + i) = *(buf2 + i) ^ opad;
    }

    // 将数据 data 拼接到 buf 末尾，并对新字符串进行内层哈希运算，产生内层的摘要 digest
    size_t hash_buf_size = BLOCK_SIZE + (data_len < HASH_SIZE ? HASH_SIZE : data_len);
    unsigned char hash_buf[hash_buf_size];
    bzero(hash_buf, hash_buf_size);
    memcpy(hash_buf, buf, BLOCK_SIZE);
    memcpy(hash_buf + BLOCK_SIZE, data, data_len);

    SM3Calc(hash_buf, BLOCK_SIZE + data_len, digest);

    // 将内层摘要 digest 拼接到 buf2 末尾，并对新字符串进行外层哈希运算，产生最终的摘要 digest，结束
    bzero(hash_buf, hash_buf_size);
    memcpy(hash_buf, buf2, BLOCK_SIZE);
    memcpy(hash_buf + BLOCK_SIZE, digest, HASH_SIZE);

    SM3Calc(hash_buf, BLOCK_SIZE + HASH_SIZE, output);
}

/**
 * 先安装 openssl-1.1.1 版本
 * 结果验证对比: echo -n "1111" | openssl dgst -sm3 -hmac ceshi
 */
int main(int argc, char *argv[])
{
    char data[256] = "1111";
    char key[256] = "74e5560dc81b4bc50b4bab0cbdeb6643";

    unsigned char digest[HASH_SIZE] = {0};
    char output[HASH_SIZE * 2 + 1] = {0};

    if (argc >= 3)
    {
        strcpy(data, argv[1]);
        strcpy(key, argv[2]);
    }

    hmac_sm3((unsigned char *)data, strlen(data), (unsigned char *)key, strlen(key), digest);

    printf("%s 's hmac-sm3 digest: \n", data);

    for (int i = 0; i < SM3_HASH_SIZE; i++)
    {
        printf("%02x ", digest[i]);
    }

    for (int i = 0; i < SM3_HASH_SIZE; ++i)
		sprintf(output + (i << 1), "%02x", (unsigned char)digest[i]);
	output[SM3_HASH_SIZE*2] = 0;

    printf("\n%s\n", output);

    return 0;
}