#include <string.h>
#include "encryption.h"

// Simple XOR encryption for demonstration purposes
// In a real-world application, use a strong encryption library
#define KEY 0x42

void custom_encrypt(const char *input, char *output)
{
    int len = strlen(input);
    for (int i = 0; i < len; i++)
    {
        output[i] = input[i] ^ KEY;
    }
    output[len] = '\0';
}

void custom_decrypt(const char *input, char *output)
{
    int len = strlen(input);
    for (int i = 0; i < len; i++)
    {
        output[i] = input[i] ^ KEY;
    }
    output[len] = '\0';
}
