# MaskIt
MaskIt is a lightweight, modular C command-line utility designed to mask and unmask byte buffers. By utilizing a flexible registry of masking algorithms, the tool makes it simple to apply various transformations to data payloads. The deliberately simple structure and native compilation approach guarantee a smooth, low-friction integration into your existing projects or complex codebases.

## Features
* **Modular Registry:** Algorithms are registered via a `mask_algo_t` structure, mapping a string name to its respective `mask` and `unmask` function pointers.
* **CLI Options:** Supports command-line arguments to specify the target algorithm, contextual key, and output formatting.
* **Debug Formatting:** Includes an optional debug mode to print hexadecimal and ASCII representations of the buffer during execution.
* **Standalone Architecture:** Built entirely in C and designed to be compiled cleanly with native toolchains.

## Usage

The program parses command-line arguments to configure the masking execution.

```cmd
maskit.exe [--algo name] [--key N] [--debug] [--list]

```
* `--algo name`: Specifies the masking algorithm to use. The default is `xor`.
* `--key N`: Specifies the key to use for the operation, accepting either decimal or hexadecimal (`0xNN`) formats. The default is `0x5A`.
* `--debug`: Enables detailed hexadecimal and ASCII output before and after masking.
* `--list`: Prints all available algorithms in the registry.

## Supported Algorithms
The following masking and unmasking algorithms are currently implemented:
* **`xor`**: Applies a standard XOR operation across the buffer using the provided key.
* **`rot`**: Applies an additive rotation mask and subtractive unmask.
* **`not`**: Applies a bitwise NOT operation to flip the bits of the buffer.
* **`rol`**: Rotates the bits of each byte left during masking and right during unmasking.
* **`add`**: Adds the key value to each byte during masking and subtracts it during unmasking.

## Architecture

The MaskIt framework is structured around a central registry and dispatcher:

1. **The Registry (`maskit.c` / `maskit.h`)**: Uses a `mask_algo_t` struct to hold the algorithm name alongside its `mask` and `unmask` function pointers. The `mask_find()` function searches an array of these structures to resolve the requested algorithm at runtime.
2. **The Dispatcher**: The `mask_execute()` and `unmask_execute()` functions take a `mask_entry_t` context and invoke the correct function pointers mapped to the buffer.
3. **The Algorithms (`masking_functions.c` / `masking_functions.h`)**: Contains the discrete logic for applying and reversing transformations.

## Build Instructions

This project compiles cleanly using the native Windows MSVC compiler.

```cmd
cl.exe main.c maskit.c masking_functions.c /Fe:maskit.exe

```

## Example Usage

```
d:\dev\MaskIt>.\maskit.exe --list
Available algorithms:
xor
rol
rot
not
rol
multilayer_bitwise

d:\dev\MaskIt>
```

```
d:\dev\MaskIt>.\maskit.exe --algo multilayer_bitwise
Using algorithm: multilayer_bitwise
Key: 0x5A

01 02 03 04
4A 7A 6A 1A
01 02 03 04

d:\dev\MaskIt>
```

### Creating New Algorithms via AI

To rapidly prototype new masking algorithms, MaskIt includes a prompt framework located in `Make_Mask.md`.
Provide this file to your preferred LLM along with a natural-language description of your desired mathematical operations (e.g., "Make a mask that XORs each byte with the key, then adds 0x10"). The model will output fully compliant, portable C code formatted perfectly for integration into `masking_functions.h` and `masking_functions.c`.

Prompt:
"using the attached skill file, can you generate a multi-layered mask using XOR and bitwise operations?"

Response:
I can certainly generate a multi-layered mask for you based on the MaskIt conventions provided in the skill.

For this algorithm, I will create a three-layer bitwise transformation:

1. **XOR** the byte with the provided key.
2. Apply a **bitwise NOT** to flip the bits.
3. **Swap the nibbles** (the upper 4 bits and lower 4 bits) using bitwise shifts and OR operations.

Since each of these specific operations is symmetric (they are their own inverse), the unmasking function perfectly reverses the mathematical operations by applying them in the reverse order.

#### 1. Header Section

Add this to your `masking_functions.h` file:

```c
// HEADER
bool multilayer_bitwise_mask(uint8_t *buffer, size_t size, void *ctx);
bool multilayer_bitwise_unmask(uint8_t *buffer, size_t size, void *ctx);
extern mask_algo_t MULTILAYER_BITWISE_ALGO;

```

#### 2. Implementation Section

Add this to your `masking_functions.c` file:

```c
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

```