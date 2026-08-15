# SKILL.md — MaskIt Algorithm Generator Skill

## Purpose
This skill generates portable masking and unmasking algorithms in C that conform to the MaskIt framework’s conventions. Given a natural‑language description of a data transformation idea, the skill outputs:

- A header declaration section (function prototypes and `extern mask_algo_t`)
- A C implementation compatible with MaskIt’s style (mask function, unmask function, and the `mask_algo_t` struct definition)

The generated code is always single‑file portable, operates in-place on byte buffers, and is free of external dependencies.

## Input Format
Users describe the desired masking algorithm in natural language.

Examples:

- “Make an algorithm that XORs each byte with a key, then adds 5.”
- “Create a bitwise mask that rotates left by 3 during masking and right by 3 during unmasking.”
- “Generate a substitution mask that flips the bits and subtracts the key.”

## Output Format
The skill must output two sections:

### 1. Header Section
Prototypes and struct declaration to be added to `masking_functions.h`:

```c
// HEADER
bool custom_mask(uint8_t *buffer, size_t size, void *ctx);
bool custom_unmask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t CUSTOM_ALGO;

```

### 2. Implementation Section

Functions and struct definition to be added to `masking_functions.c`:

```c
// IMPLEMENTATION
bool custom_mask(uint8_t *buffer, size_t size, void *ctx)
{
    // implementation...
    return true;
}

bool custom_unmask(uint8_t *buffer, size_t size, void *ctx)
{
    // implementation...
    return true;
}

mask_algo_t CUSTOM_ALGO = {
    .name   = "custom",
    .mask   = custom_mask,
    .unmask = custom_unmask
};

```

The skill must never output additional files, includes, or dependencies.

## Core Rules

### Function Signatures

All masking and unmasking functions must use:

```c
bool algorithm_mask(uint8_t *buffer, size_t size, void *ctx);
bool algorithm_unmask(uint8_t *buffer, size_t size, void *ctx);

```

### Context Handling

* The `ctx` variable represents the cryptographic key or modifier.
* It must be cast to the appropriate type before use (typically `uint8_t`).
```c
uint8_t key = *(uint8_t*)ctx;

```



### Buffer Operations

* Algorithms must operate *in-place* on the `buffer` array.
* Loop from `0` to `size - 1`.

```c
for (size_t i = 0; i < size; i++)
{
    buffer[i] = // operations
}

```

### Reversibility

* The `unmask` function MUST perfectly reverse the mathematical operations of the `mask` function.
* If the operation is symmetric (like a pure XOR or NOT), the unmask function may simply wrap or re-use the mask function.

### No Dynamic Memory

Algorithms must operate purely on the provided buffer with no allocations (`malloc`, `calloc`, etc.).

### No External Dependencies

Only use basic arithmetic, bitwise operations, and standard C types (`uint8_t`, `size_t`, `bool`).

## Naming Rules

* Function names: Lowercase with underscores, suffixing the operation (e.g., `xor_mask`, `addsub_unmask`).
* Struct names: Uppercase with `_ALGO` suffix (e.g., `XOR_ALGO`, `ADDSUB_ALGO`).
* CLI names: The `.name` property in the struct must be a short, lowercase string identifying the algorithm (e.g., `"xor"`, `"rot"`).

## Examples (Canonical Patterns)

### Symmetric (XOR)

```c
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

```

### Asymmetric (ROT)

```c
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

```

## Error Handling

The skill must not generate:

* Memory allocation
* File I/O
* Logging
* External dependencies
* Global state or static lookup tables

If the user requests an algorithm that cannot be mathematically reversed, the skill must respond:

“MaskIt algorithms must be mathematically reversible to support the unmask execution phase. This operation destroys data. Here is a reversible alternative instead…”

## Determinism Requirements

The skill must produce:

* Consistent formatting
* Consistent indentation
* Consistent naming
* Mathematical accuracy for reversibility

## User Prompt Examples

### Example 1

User:

“Make a mask that XORs each byte with the key, then adds 0x10.”

Skill Output:

Header + implementation where `mask` applies XOR then ADD, and `unmask` applies SUB then XOR, ensuring perfect reversibility.

### Example 2

User:

“Create a simple NOT mask.”

Skill Output:

Header + implementation defining a bitwise NOT operation, mapping both `mask` and `unmask` in the struct to the same function since NOT is symmetric.

## End of SKILL.md