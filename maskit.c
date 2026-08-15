#include "maskit.h"
#include "masking_functions.h"   // XOR_ALGO, ROL_ALGO, etc.

/* ---------------- Registry ---------------- */

static mask_algo_t *mask_table[] = {
    &XOR_ALGO,
    &ROL_ALGO,
    &ROT_ALGO,
    &NOT_ALGO,
    &ROL_ALGO,
    &MULTILAYER_BITWISE_ALGO
};

static size_t mask_table_count = sizeof(mask_table) / sizeof(mask_table[0]);

mask_algo_t *mask_find(const char *name)
{
    for (size_t i = 0; i < mask_table_count; i++)
        if (strcmp(mask_table[i]->name, name) == 0)
            return mask_table[i];

    return NULL;
}

/* ---------------- Dispatcher ---------------- */

bool mask_execute(mask_entry_t *entry)
{
    if (!entry || !entry->algo || !entry->algo->mask)
        return false;

    return entry->algo->mask(entry->buffer, entry->size, entry->ctx);
}

bool unmask_execute(mask_entry_t *entry)
{
    if (!entry || !entry->algo || !entry->algo->unmask)
        return false;

    return entry->algo->unmask(entry->buffer, entry->size, entry->ctx);
}


// Optional, only used for troubleshooting/debugging
void mask_list(void)
{
    for (size_t i = 0; i < mask_table_count; i++)
        printf("%s\n", mask_table[i]->name);
}
