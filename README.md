# Custom C++ Memory Allocator

This project implements a simplified custom allocator that simulates core behavior of `malloc()` and `free()` using a fixed-size heap.

## Project Files

- `allocator.h` - public API and block metadata definition.
- `allocator.cpp` - allocator implementation (first-fit, splitting, coalescing, fragmentation tracking).
- `main.cpp` - demonstration program with allocations, frees, and memory layout printing.

Compile with:

```bash
g++ main.cpp allocator.cpp -o allocator
```

Run with:

```bash
./allocator
```

---

## How `malloc` Works Internally in This Project

### 1. Fixed-size heap
- A large static byte array (`64 KB`) is used as the heap.
- The first block initially spans the whole heap (minus metadata overhead).

### 2. Block metadata
Each memory block contains:
- `size` - payload bytes available in this block.
- `is_free` - whether block is free or allocated.
- `requested` - exact bytes requested by user (for internal fragmentation metrics).
- `next` - pointer to the next block.

### 3. First-Fit allocation (`my_malloc`)
- Traverse blocks from heap start.
- Pick the first free block with enough space.
- If block is larger than needed, split it into:
  - allocated block of requested size
  - remaining free block
- Return pointer to payload (just after metadata header).

### 4. Free and coalesce (`my_free`)
- Convert payload pointer back to block header.
- Mark block as free.
- Merge adjacent free blocks to create larger contiguous regions and reduce external fragmentation.

---

## Time Complexity

Let `n` be number of blocks currently in the heap list.

- `my_malloc(size)`:
  - First-fit search scans blocks linearly.
  - **Worst case:** `O(n)`.
- `my_free(ptr)`:
  - Marking free is `O(1)`.
  - Coalescing pass scans list to merge neighbors.
  - **Worst case:** `O(n)`.
- Layout/fragmentation reporting:
  - Requires scanning all blocks.
  - **O(n)**.

---

## Fragmentation Explained

### Internal fragmentation
- Wasted space inside allocated blocks.
- In this implementation:
  - `internal = sum(block.size - block.requested)` over allocated blocks.
- Happens primarily due to split constraints and allocation granularity.

### External fragmentation
- Free memory split into multiple non-contiguous pieces.
- In this implementation:
  - `external = total_free_bytes - largest_free_block`.
- Even with enough total free bytes, large allocation can fail if free space is too scattered.

---

## Demonstration in `main.cpp`

The demo shows:
1. Multiple allocations.
2. Freeing blocks to create gaps.
3. Re-allocation using first-fit strategy.
4. Coalescing after freeing adjacent blocks.
5. Memory layout and fragmentation output after each step.

