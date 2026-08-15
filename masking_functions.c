#include "masking_functions.h"

/* ---------------- XOR ---------------- */

bool xor_mask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t key = *(uint8_t*)ctx;

    for (size_t i = 0; i < size; i++)
        buffer[i] ^= key;

    return true;
}

bool xor_unmask(uint8_t *buffer, size_t size, void *ctx)
{
    return xor_mask(buffer, size, ctx);
}

mask_algo_t XOR_ALGO = {
    .name   = "xor",
    .mask   = xor_mask,
    .unmask = xor_unmask
};

bool rot_mask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t rot = *(uint8_t*)ctx;

    for (size_t i = 0; i < size; i++)
        buffer[i] = (uint8_t)(buffer[i] + rot);

    return true;
}

bool rot_unmask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t rot = *(uint8_t*)ctx;

    for (size_t i = 0; i < size; i++)
        buffer[i] = (uint8_t)(buffer[i] - rot);

    return true;
}

mask_algo_t ROT_ALGO = {
    .name   = "rot",
    .mask   = rot_mask,
    .unmask = rot_unmask
};

bool not_mask(uint8_t *buffer, size_t size, void *ctx)
{
    for (size_t i = 0; i < size; i++)
        buffer[i] = ~buffer[i];

    return true;
}

mask_algo_t NOT_ALGO = {
    .name   = "not",
    .mask   = not_mask,
    .unmask = not_mask
};

// ROL

bool rol_mask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t n = *(uint8_t*)ctx & 7;

    for (size_t i = 0; i < size; i++)
        buffer[i] = (buffer[i] << n) | (buffer[i] >> (8 - n));

    return true;
}

bool ror_unmask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t n = *(uint8_t*)ctx & 7;

    for (size_t i = 0; i < size; i++)
        buffer[i] = (buffer[i] >> n) | (buffer[i] << (8 - n));

    return true;
}

mask_algo_t ROL_ALGO = {
    .name   = "rol",
    .mask   = rol_mask,
    .unmask = ror_unmask
};

// ADDSUB
bool add_mask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t val = *(uint8_t*)ctx;

    for (size_t i = 0; i < size; i++)
        buffer[i] = (uint8_t)(buffer[i] + val);

    return true;
}

bool sub_unmask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t val = *(uint8_t*)ctx;

    for (size_t i = 0; i < size; i++)
        buffer[i] = (uint8_t)(buffer[i] - val);

    return true;
}

mask_algo_t ADDSUB_ALGO = {
    .name   = "add",
    .mask   = add_mask,
    .unmask = sub_unmask
};

// Gemini Pro Skill Generated
// IMPLEMENTATION
bool multilayer_bitwise_mask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t key = *(uint8_t*)ctx;
    
    for (size_t i = 0; i < size; i++)
    {
        // Layer 1: XOR with key
        uint8_t val = buffer[i] ^ key;
        
        // Layer 2: Bitwise NOT
        val = ~val;
        
        // Layer 3: Swap nibbles
        buffer[i] = (uint8_t)(((val & 0x0F) << 4) | ((val & 0xF0) >> 4));
    }
    
    return true;
}

bool multilayer_bitwise_unmask(uint8_t *buffer, size_t size, void *ctx)
{
    uint8_t key = *(uint8_t*)ctx;
    
    for (size_t i = 0; i < size; i++)
    {
        // Reverse Layer 3: Swap nibbles (swapping again restores original)
        uint8_t val = (uint8_t)(((buffer[i] & 0x0F) << 4) | ((buffer[i] & 0xF0) >> 4));
        
        // Reverse Layer 2: Bitwise NOT
        val = ~val;
        
        // Reverse Layer 1: XOR with key
        buffer[i] = val ^ key;
    }
    
    return true;
}

mask_algo_t MULTILAYER_BITWISE_ALGO = {
    .name   = "multilayer_bitwise",
    .mask   = multilayer_bitwise_mask,
    .unmask = multilayer_bitwise_unmask
};