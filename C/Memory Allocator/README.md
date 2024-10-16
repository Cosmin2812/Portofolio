# Memory Allocator in C
Build a minimalistic memory allocator that can be used to manually manage virtual memory. 
- "Reliable" library that accounts for explicit allocation, reallocation, and initialization of memory.
- **Low level implementation** of minimal versions of **malloc()**, **calloc()**, **realloc()**, **free()**
- Used **brk()** and **mmap()** syscalls to allocate memory in user-space processes.