// compilation: cl.exe mask_file.c maskit.c masking_functions.c utils.c /Fe:mask_file.exe
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
    if (strncmp(s, "0x", 2) == 0 || strncmp(s, "0X", 2) == 0)
        return (uint8_t)strtoul(s + 2, NULL, 16);

    return (uint8_t)strtoul(s, NULL, 10);
}

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        printf("[-] Failed to open input file: %s\n", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (sz <= 0)
    {
        printf("[-] Input file is empty or unreadable.\n");
        fclose(fp);
        return NULL;
    }

    uint8_t *buf = malloc(sz);
    if (!buf)
    {
        printf("[-] Memory allocation failed.\n");
        fclose(fp);
        return NULL;
    }

    if (fread(buf, 1, sz, fp) != (size_t)sz)
    {
        printf("[-] Failed to read file contents.\n");
        free(buf);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    *out_size = (size_t)sz;
    return buf;
}

static bool write_file(const char *path, const uint8_t *buf, size_t size)
{
    FILE *fp = fopen(path, "wb");
    if (!fp)
    {
        printf("[-] Failed to open output file: %s\n", path);
        return false;
    }

    if (fwrite(buf, 1, size, fp) != size)
    {
        printf("[-] Failed to write output file.\n");
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

int main(int argc, char **argv)
{
    const char *algo_name = "xor";   /* default */
    uint8_t key = 0x5A;              /* default */
    bool list_only = false;

    const char *input_path = NULL;
    const char *output_path = NULL;

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
        else if (strcmp(argv[i], "--in") == 0 && i + 1 < argc)
        {
            input_path = argv[++i];
        }
        else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc)
        {
            output_path = argv[++i];
        }
        else
        {
            printf("Unknown option: %s\n", argv[i]);
            printf("Usage: maskit.exe [--algo name] [--key N] [--debug] [--list] --in file --out file\n");
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

    if (!input_path || !output_path)
    {
        printf("[-] You must specify --in <file> and --out <file>\n");
        return 1;
    }

    /* ---------------- Read input file ---------------- */
    size_t buf_size = 0;
    uint8_t *buf = read_file(input_path, &buf_size);
    if (!buf)
        return 1;

    /* ---------------- Lookup algorithm ---------------- */
    mask_algo_t *algo = mask_find(algo_name);
    if (!algo)
    {
        printf("Algorithm '%s' not found.\n", algo_name);
        printf("Use --list to see available algorithms.\n");
        free(buf);
        return 1;
    }

    mask_entry_t entry = {
        .buffer = buf,
        .size   = buf_size,
        .ctx    = &key,
        .algo   = algo
    };

    /* ---------------- Execution ---------------- */
    printf("Using algorithm: %s\n", algo_name);
    printf("Key: 0x%02X\n", key);
    printf("Input: %s\n", input_path);
    printf("Output: %s\n\n", output_path);

    /* Print initial */
    //enable_debug ? print_hex_ascii(buf, buf_size) : print_hex(buf, buf_size);

    /* Mask */
    mask_execute(&entry);
    //enable_debug ? print_hex_ascii(buf, buf_size) : print_hex(buf, buf_size);

    /* Write masked output */
    if (!write_file(output_path, buf, buf_size))
    {
        free(buf);
        return 1;
    }

    printf("[+] Wrote masked output to %s\n", output_path);

    /* Unmask (optional demonstration) */
    //unmask_execute(&entry);
    //printf("\n[Debug] After unmask (should match original):\n");
    //enable_debug ? print_hex_ascii(buf, buf_size) : print_hex(buf, buf_size);

    free(buf);
    return 0;
}
