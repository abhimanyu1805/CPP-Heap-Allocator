#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef>

namespace custom_allocator {

// Fixed heap size used by the custom allocator.
constexpr std::size_t HEAP_SIZE = 1024 * 64; // 64 KB

// Metadata header placed at the beginning of every block.
struct Block {
    std::size_t size;          // Usable payload size in bytes.
    bool is_free;              // True if block is available.
    std::size_t requested;     // Requested bytes (for fragmentation stats).
    Block* next;               // Next block in physical heap order.
};

// Initializes allocator state. Safe to call multiple times.
void init_allocator();

// Allocates memory using first-fit strategy.
void* my_malloc(std::size_t size);

// Frees previously allocated memory.
void my_free(void* ptr);

// Diagnostic helpers.
void print_memory_layout();
std::size_t get_total_internal_fragmentation();
std::size_t get_total_external_fragmentation();

} // namespace custom_allocator

#endif // ALLOCATOR_H
