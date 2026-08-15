// compilation: cl.exe main.c maskit.c masking_functions.c uitls.c /Fe:maskit.exe
#include "maskit.h"

bool enable_debug = false;

static void print_hex(const uint8_t *buf, size_t size)
{
    for (size_t i = 0; i < size; i++)
        printf("%02X ", buf[i]);
    printf("\n");
}

static void print_hex_ascii(const uint8_t *buf, size_t size)
{
    for (size_t i = 0; i < size; i += 16)
    {
        printf("%08X  ", (unsigned)i);

        for (size_t j = 0; j < 16; j++)
            (i + j < size) ? printf("%02X ", buf[i + j]) : printf("   ");

        printf(" ");

        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < size)
            {
                uint8_t c = buf[i + j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
        }

        printf("\n");
    }
}

static uint8_t parse_key(const char *s)
{
    /* Accept decimal or hex (0xNN) */
    if (strncmp(s, "0x", 2) == 0 || strncmp(s, "0X", 2) == 0)
        return (uint8_t)strtoul(s + 2, NULL, 16);

    return (uint8_t)strtoul(s, NULL, 10);
}

int main(int argc, char **argv)
{
    const char *algo_name = "xor";   /* default */
    uint8_t key = 0x5A;              /* default */
    bool list_only = false;

    /* ---------------- CLI Parsing ---------------- */
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--algo") == 0 && i + 1 < argc)
        {
            algo_name = argv[++i];
        }
        else if (strcmp(argv[i], "--key") == 0 && i + 1 < argc)
        {
            key = parse_key(argv[++i]);
        }
        else if (strcmp(argv[i], "--debug") == 0)
        {
            enable_debug = true;
        }
        else if (strcmp(argv[i], "--list") == 0)
        {
            list_only = true;
        }
        else
        {
            printf("Unknown option: %s\n", argv[i]);
            printf("Usage: maskit.exe [--algo name] [--key N] [--debug] [--list]\n");
            return 1;
        }
    }

    /* If user requested algorithm list */
    if (list_only)
    {
        printf("Available algorithms:\n");
        mask_list();
        return 0;
    }

    /* ---------------- Buffer ---------------- */
    uint8_t buf[] = { 1, 2, 3, 4 };

    /* ---------------- Lookup algorithm ---------------- */
    mask_algo_t *algo = mask_find(algo_name);
    if (!algo)
    {
        printf("Algorithm '%s' not found.\n", algo_name);
        printf("Use --list to see available algorithms.\n");
        return 1;
    }

    mask_entry_t entry = {
        .buffer = buf,
        .size   = sizeof(buf),
        .ctx    = &key,
        .algo   = algo
    };

    /* ---------------- Execution ---------------- */
    printf("Using algorithm: %s\n", algo_name);
    printf("Key: 0x%02X\n\n", key);

    /* Print initial */
    enable_debug ? print_hex_ascii(buf, sizeof(buf)) : print_hex(buf, sizeof(buf));

    /* Mask */
    mask_execute(&entry);
    enable_debug ? print_hex_ascii(buf, sizeof(buf)) : print_hex(buf, sizeof(buf));

    /* Unmask */
    unmask_execute(&entry);
    enable_debug ? print_hex_ascii(buf, sizeof(buf)) : print_hex(buf, sizeof(buf));

    return 0;
}
