#include "oslabs.h"
#include <limits.h>


struct MEMORY_BLOCK myblock(int start_address, int end_address, int segment_size, int process_id)
{
    struct MEMORY_BLOCK block;
    block.start_address = start_address;
    block.end_address = end_address;
    block.segment_size = segment_size;
    block.process_id = process_id;

    return block;
}

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id)
{
    struct MEMORY_BLOCK allocated_block = myblock(0,0,0,0);
    int best_fit_index = -1;
    int best_fit_difference = INT_MAX;

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            int size_difference = memory_map[i].segment_size - request_size;

            if (size_difference == 0) {
                // Exact fit, allocate the entire block
                allocated_block = memory_map[i];
                allocated_block.process_id = process_id;
                memory_map[i] = allocated_block;
                return allocated_block;
            }

            if (size_difference < best_fit_difference && size_difference > 0){
                // Found a better fit
                best_fit_difference = size_difference;
                best_fit_index = i;
            }
        }
    }

    if (best_fit_index != -1) {
        // Split the block into allocated and free parts
        allocated_block.start_address = memory_map[best_fit_index].start_address;
        allocated_block.end_address = allocated_block.start_address + request_size - 1;
        allocated_block.segment_size = request_size;
        allocated_block.process_id = process_id;

        struct MEMORY_BLOCK free_block = {
            .start_address = allocated_block.end_address + 1,
            .end_address = memory_map[best_fit_index].end_address,
            .segment_size = memory_map[best_fit_index].segment_size - request_size,
            .process_id = 0
        };

        // Shift the memory_map array to make space for the new free block
        for (int i = *map_cnt; i > best_fit_index + 1; i--) {
            memory_map[i] = memory_map[i-1];
        }

        // Update memory_map
        memory_map[best_fit_index + 1] = free_block;
        memory_map[best_fit_index] = allocated_block;

        (*map_cnt) += 1;
    }

    return allocated_block;
}


struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    struct MEMORY_BLOCK allocated_block = myblock(0,0,0,0);

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            if (memory_map[i].segment_size == request_size) {
                // Exact fit, allocate the entire block
                allocated_block = memory_map[i];
                allocated_block.process_id = process_id;
                memory_map[i] = allocated_block;
                return allocated_block;
            } else {
                // Split the block into allocated and free parts
                allocated_block = memory_map[i];
                allocated_block.segment_size = request_size;
                allocated_block.process_id = process_id;
                allocated_block.end_address = allocated_block.start_address + request_size -1;

                struct MEMORY_BLOCK free_block = {
                    .start_address = allocated_block.end_address + 1,
                    .end_address = memory_map[i].end_address,
                    .segment_size = memory_map[i].segment_size - request_size,
                    .process_id = 0
                };

                // Shift the memory_map array to make space for the new free block
                for (int j = *map_cnt; j > i + 1; j--) {
                    memory_map[j] = memory_map[j - 1];
                }

                // Update memory_map
                memory_map[i + 1] = free_block;
                memory_map[i] = allocated_block;

                (*map_cnt) += 1;
                return allocated_block;
            }
        }
    }

    return allocated_block;
}


struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    struct MEMORY_BLOCK allocated_block = myblock(0,0,0,0);
    int worst_fit_index = -1;
    int worst_fit_size = -1;

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            if (memory_map[i].segment_size == request_size) {
                // Exact fit, allocate the entire block
                allocated_block = memory_map[i];
                allocated_block.process_id = process_id;
                memory_map[i] = allocated_block;
                return allocated_block;
            } else if (memory_map[i].segment_size > worst_fit_size) {
                // Found a larger fit
                worst_fit_size = memory_map[i].segment_size;
                worst_fit_index = i;
            }
        }
    }

    if (worst_fit_index != -1) {
        // Split the block into allocated and free parts
        allocated_block = memory_map[worst_fit_index];
        allocated_block.segment_size = request_size;
        allocated_block.process_id = process_id;
        allocated_block.end_address = allocated_block.start_address + request_size -1;

        struct MEMORY_BLOCK free_block = {
            .start_address = allocated_block.end_address + 1,
            .end_address = memory_map[worst_fit_index].end_address,
            .segment_size = memory_map[worst_fit_index].segment_size - request_size,
            .process_id = 0
        };

        // Shift the memory_map array to make space for the new free block
        for (int i = *map_cnt; i > worst_fit_index + 1; i--) {
            memory_map[i] = memory_map[i - 1];
        }

        // Update memory_map
        memory_map[worst_fit_index + 1] = free_block;
        memory_map[worst_fit_index] = allocated_block;

        (*map_cnt) += 1;
    }

    return allocated_block;
}



struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id, int last_address) {
    struct MEMORY_BLOCK allocated_block = myblock(0,0,0,0);

    // Start searching from the last allocated block's next position
    int start_index = -1;
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].start_address == last_address) {
            start_index = i + 1;
            break;
        }
    }

    // If last_address is not found or is the last block, start from the beginning
    if (start_index == -1 || start_index == *map_cnt) {
        start_index = 0;
    }

    for (int i = start_index; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            if (memory_map[i].segment_size == request_size) {
                // Exact fit, allocate the entire block
                allocated_block = memory_map[i];
                allocated_block.process_id = process_id;
                memory_map[i] = allocated_block;
                return allocated_block;
            } else {
                // Split the block into allocated and free parts
                allocated_block = memory_map[i];
                allocated_block.segment_size = request_size;
                allocated_block.process_id = process_id;
                allocated_block.end_address = allocated_block.start_address + request_size - 1;

                struct MEMORY_BLOCK free_block = {
                    .start_address = allocated_block.end_address + 1,
                    .end_address = memory_map[i].end_address,
                    .segment_size = memory_map[i].segment_size - request_size,
                    .process_id = 0
                };

                // Shift the memory_map array to make space for the new free block
                for (int j = *map_cnt; j > i + 1; j--) {
                    memory_map[j] = memory_map[j - 1];
                }

                // Update memory_map
                memory_map[i + 1] = free_block;
                memory_map[i] = allocated_block;

                (*map_cnt) += 1;
                return allocated_block;
            }
        }
    }

    // If not found in the remaining blocks, search from the beginning
    for (int i = 0; i < start_index; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            if (memory_map[i].segment_size == request_size) {
                // Exact fit, allocate the entire block
                allocated_block = memory_map[i];
                allocated_block.process_id = process_id;
                return allocated_block;
            } else {
                // Split the block into allocated and free parts
                allocated_block = memory_map[i];
                allocated_block.segment_size = request_size;
                allocated_block.process_id = process_id;

                struct MEMORY_BLOCK free_block = {
                    .start_address = allocated_block.start_address + request_size,
                    .end_address = allocated_block.end_address,
                    .segment_size = allocated_block.segment_size - request_size,
                    .process_id = 0
                };

                // Shift the memory_map array to make space for the new free block
                for (int j = *map_cnt; j > i + 1; j--) {
                    memory_map[j] = memory_map[j - 1];
                }

                // Update memory_map
                memory_map[i + 1] = free_block;
                memory_map[i] = allocated_block;

                (*map_cnt) += 1;
                return allocated_block;
            }
        }
    }

    return allocated_block;
}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt) {
    // Find the index of the released block in the memory map
    int released_index = -1;
    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].start_address == freed_block.start_address &&
            memory_map[i].end_address == freed_block.end_address &&
            memory_map[i].process_id == freed_block.process_id) {
            released_index = i;
            break;
        }
    }

    if (released_index == -1) {
        // The released block was not found in the memory map
        return;
    }

    // Mark the released block as free
    memory_map[released_index].process_id = 0;

    // Merge with the previous free block if available
    if (released_index > 0 && memory_map[released_index - 1].process_id == 0) {
        // Merge with the previous block
        memory_map[released_index - 1].end_address = memory_map[released_index].end_address;
        memory_map[released_index - 1].segment_size += memory_map[released_index].segment_size;

        // Shift the memory_map array to remove the released block
        for (int i = released_index; i < *map_cnt - 1; i++) {
            memory_map[i] = memory_map[i + 1];
        }

        (*map_cnt)--;
        released_index--;
    }

    // Merge with the next free block if available
    if (released_index < *map_cnt - 1 && memory_map[released_index + 1].process_id == 0) {
        // Merge with the next block
        memory_map[released_index].end_address = memory_map[released_index + 1].end_address;
        memory_map[released_index].segment_size += memory_map[released_index + 1].segment_size;

        // Shift the memory_map array to remove the next free block
        for (int i = released_index + 1; i < *map_cnt - 1; i++) {
            memory_map[i] = memory_map[i + 1];
        }

        (*map_cnt)--;
    }
}

