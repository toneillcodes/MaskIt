#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

// Optional, only used for troubleshooting/debugging
#include <stdio.h>

typedef struct mask_algo {
    const char *name;
    bool (*mask)(uint8_t *buffer, size_t size, void *ctx);
    bool (*unmask)(uint8_t *buffer, size_t size, void *ctx);
} mask_algo_t;

typedef struct mask_entry {
    uint8_t     *buffer;
    size_t       size;
    void        *ctx;
    mask_algo_t *algo;
} mask_entry_t;

/* Dispatcher */
bool mask_execute(mask_entry_t *entry);
bool unmask_execute(mask_entry_t *entry);

/* Registry lookup */
mask_algo_t *mask_find(const char *name);

/* Registry list */
void mask_list(void);
