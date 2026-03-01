#include "allocator.h"

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace custom_allocator {

namespace {

alignas(std::max_align_t) unsigned char heap[HEAP_SIZE];
Block* head = nullptr;
bool initialized = false;

constexpr std::size_t MIN_SPLIT_REMAINDER = sizeof(Block) + 1;

void split_block(Block* block, std::size_t requested_size) {
    // Split only if the remainder can hold a new block header plus at least 1 byte.
    if (block->size < requested_size + MIN_SPLIT_REMAINDER) {
        return;
    }

    unsigned char* block_start = reinterpret_cast<unsigned char*>(block);
    unsigned char* new_block_addr = block_start + sizeof(Block) + requested_size;

    Block* remainder = reinterpret_cast<Block*>(new_block_addr);
    remainder->size = block->size - requested_size - sizeof(Block);
    remainder->is_free = true;
    remainder->requested = 0;
    remainder->next = block->next;

    block->size = requested_size;
    block->next = remainder;
}

void coalesce_free_blocks() {
    Block* current = head;

    // Merge adjacent free blocks in physical order.
    while (current != nullptr && current->next != nullptr) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(Block) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

bool ptr_in_heap(const void* ptr) {
    auto* byte_ptr = reinterpret_cast<const unsigned char*>(ptr);
    return byte_ptr >= heap && byte_ptr < (heap + HEAP_SIZE);
}

} // namespace

void init_allocator() {
    if (initialized) {
        return;
    }

    head = reinterpret_cast<Block*>(heap);
    head->size = HEAP_SIZE - sizeof(Block);
    head->is_free = true;
    head->requested = 0;
    head->next = nullptr;

    initialized = true;
}

void* my_malloc(std::size_t size) {
    if (size == 0) {
        return nullptr;
    }

    init_allocator();

    Block* current = head;

    // First-fit search: first free block large enough for requested size.
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            split_block(current, size);
            current->is_free = false;
            current->requested = size;
            return reinterpret_cast<unsigned char*>(current) + sizeof(Block);
        }
        current = current->next;
    }

    return nullptr; // No suitable block found.
}

void my_free(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    if (!ptr_in_heap(ptr)) {
        std::cerr << "Warning: attempt to free pointer outside custom heap.\n";
        return;
    }

    auto* block = reinterpret_cast<Block*>(reinterpret_cast<unsigned char*>(ptr) - sizeof(Block));

    // Basic guard against double free.
    if (block->is_free) {
        std::cerr << "Warning: double free detected.\n";
        return;
    }

    block->is_free = true;
    block->requested = 0;

    coalesce_free_blocks();
}

void print_memory_layout() {
    init_allocator();

    const Block* current = head;
    std::size_t index = 0;

    std::cout << "\n===== Memory Layout =====\n";
    while (current != nullptr) {
        std::cout << "Block " << index
                  << " | Addr: " << current
                  << " | Size: " << current->size
                  << " | Status: " << (current->is_free ? "FREE" : "USED")
                  << " | Requested: " << current->requested
                  << '\n';
        current = current->next;
        ++index;
    }
    std::cout << "Internal Frag (bytes): " << get_total_internal_fragmentation() << '\n';
    std::cout << "External Frag (bytes): " << get_total_external_fragmentation() << '\n';
    std::cout << "=========================\n\n";
}

std::size_t get_total_internal_fragmentation() {
    init_allocator();

    std::size_t internal = 0;
    const Block* current = head;

    while (current != nullptr) {
        if (!current->is_free && current->size >= current->requested) {
            internal += (current->size - current->requested);
        }
        current = current->next;
    }

    return internal;
}

std::size_t get_total_external_fragmentation() {
    init_allocator();

    std::size_t total_free = 0;
    std::size_t largest_free = 0;

    const Block* current = head;
    while (current != nullptr) {
        if (current->is_free) {
            total_free += current->size;
            largest_free = std::max(largest_free, current->size);
        }
        current = current->next;
    }

    if (total_free == 0) {
        return 0;
    }

    // Common metric: free bytes not part of the largest contiguous chunk.
    return total_free - largest_free;
}

} // namespace custom_allocator
