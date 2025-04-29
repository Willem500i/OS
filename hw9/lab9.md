# Lab 9 Assignment Answers

## B) Questions Answers (3 points)

### 1) In both processes, print the start address of the shared buffer.

The programs print the shared buffer address using:

```c
printf("Producer/Consumer: Shared buffer address = %p (logical/virtual address)\n", (void *)shm);
```

### 2) Was the address printed Logical (virtual) or physical address?

The address printed is a **logical (virtual) address**. This is because the programs access memory through the virtual memory system. The `mmap()` function returns a virtual address that the process can use to access the shared memory region. The actual physical address is managed by the operating system's memory management unit (MMU) and is not directly accessible to user-space programs.

### 3) Print the address of n from your running program and also...

The programs print the address of n using:

```c
printf("Producer/Consumer: Address of n = %p\n", (void *)&n);
```

### 4) Find out where it's stored in the .elf file (executable).

To find where variables are stored in the executable, we can use tools like `objdump` or `readelf`:

```bash
objdump --syms lab9_producer | grep n
```

Or:

```bash
readelf -s lab9_producer
```

The variable `n` is likely stored in the `.bss` section if uninitialized or in the `.data` section if initialized. Since `n` is initialized when read from command line arguments, it would be stored in the stack segment during program execution.

### 5) Did the addresses match (printed from the running program vs the one in the program's elf file)? Why?

No, the addresses will not match. The address printed from the running program is the runtime virtual address in memory after the program has been loaded and memory has been allocated by the operating system. The address in the ELF file is the address relative to the program's load address as specified in the executable.

The difference exists because:

1. Modern operating systems use Address Space Layout Randomization (ASLR) which randomizes where programs and their data are loaded in memory for security purposes.
2. The variable `n` is a local variable stored on the stack, while the ELF file would contain information about global or static variables.
3. The actual memory layout is determined at runtime by the loader and memory allocator.

### 6) What is the virtual address of the entry point in your producer and consumer programs?

The entry point (main function) virtual address can be determined using:

```bash
readelf -h lab9_producer | grep "Entry point"
```

Or:

```bash
objdump -f lab9_producer | grep "start address"
```

This would show the entry point address as defined in the ELF header. Note that due to ASLR, the actual runtime address would be different from this value unless ASLR is disabled.

## Compilation and Execution

To compile:

```bash
gcc -o lab9_producer lab9_producer.c -lrt
gcc -o lab9_consumer lab9_consumer.c -lrt
```

To run:

```bash
./lab9_producer 5 2 &
./lab9_consumer 5 2
```

Note: The `-lrt` flag is needed to link with the real-time library which contains the shared memory functions.
