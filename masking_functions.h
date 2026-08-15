#pragma once
#include "maskit.h"

/* XOR */
bool xor_mask(uint8_t *buffer, size_t size, void *ctx);
bool xor_unmask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t XOR_ALGO;

// ROT
bool rot_mask(uint8_t *buffer, size_t size, void *ctx);
bool rot_unmask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t ROT_ALGO;

// NOT
bool not_mask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t NOT_ALGO;

// ROL
bool rol_mask(uint8_t *buffer, size_t size, void *ctx);
bool ror_unmask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t ROL_ALGO;

// ADDSUB
bool add_mask(uint8_t *buffer, size_t size, void *ctx);
bool sub_unmask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t ADD_ALGO;

// Gemini Pro Skill Generated
// HEADER
bool multilayer_bitwise_mask(uint8_t *buffer, size_t size, void *ctx);
bool multilayer_bitwise_unmask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t MULTILAYER_BITWISE_ALGO;