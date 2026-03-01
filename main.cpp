#include "allocator.h"

#include <cstddef>
#include <iostream>

using custom_allocator::my_free;
using custom_allocator::my_malloc;
using custom_allocator::print_memory_layout;

int main() {
    std::cout << "Custom Memory Allocator Demo\n";

    // Initial empty heap.
    print_memory_layout();

    // Multiple allocations.
    void* p1 = my_malloc(120);
    void* p2 = my_malloc(256);
    void* p3 = my_malloc(64);

    std::cout << "After allocating p1(120), p2(256), p3(64):\n";
    print_memory_layout();

    // Free one middle block to create a hole.
    my_free(p2);
    std::cout << "After freeing p2:\n";
    print_memory_layout();

    // Re-allocation should reuse first fitting free block (first-fit).
    void* p4 = my_malloc(200);
    std::cout << "After reallocating p4(200) (should fit where p2 was):\n";
    print_memory_layout();

    // Free neighboring blocks to demonstrate coalescing.
    my_free(p3);
    my_free(p4);
    std::cout << "After freeing p3 and p4 (coalescing expected):\n";
    print_memory_layout();

    // Allocate again after coalescing.
    void* p5 = my_malloc(400);
    std::cout << "After allocating p5(400) from coalesced free space:\n";
    print_memory_layout();

    // Cleanup.
    my_free(p1);
    my_free(p5);
    std::cout << "After freeing all allocations:\n";
    print_memory_layout();

    return 0;
}
