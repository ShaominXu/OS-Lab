#include "oslabs.h"
#include <stdio.h>
#include <limits.h>  // for INT_MAX



int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    int frame_number = -1;

    // Check if the page is already in memory
    if (page_table[page_number].is_valid) {
        // Page found in memory
        frame_number = page_table[page_number].frame_number;
        // Update timestamps and reference count
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return frame_number;
    }

    // Page is not in memory

    // Check if there are free frames
    if (*frame_cnt > 0) {
        // Allocate a free frame
        frame_number = frame_pool[(*frame_cnt) - 1];
        (*frame_cnt)--;
    } else {
        // Page replacement is needed

        // Find the page with the smallest arrival_timestamp
        int min_arrival_index = -1;
        for (int j = 0; j < *table_cnt; j++) {
            if (page_table[j].is_valid) {
                if (min_arrival_index == -1 || page_table[j].arrival_timestamp <= page_table[min_arrival_index].arrival_timestamp) {
                    min_arrival_index = j;
                }
            }
        }

        // Free the frame occupied by the page with the smallest arrival_timestamp
        frame_number = page_table[min_arrival_index].frame_number;


        // Replace the page in the page table
        page_table[min_arrival_index].is_valid = 0;
        page_table[min_arrival_index].frame_number = -1;
        page_table[min_arrival_index].arrival_timestamp = -1;
        page_table[min_arrival_index].last_access_timestamp = -1;
        page_table[min_arrival_index].reference_count = -1;
    }

    // Update the page table entry for the newly referenced page
    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame_number;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame_number;
}


int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int faults = 0;
    int timestamp = 1;

    for (int i = 0; i < reference_cnt; i++) {
        int page_number = reference_string[i];

        // Check if the page is already in memory
        if (page_table[page_number].is_valid) {
            // Page found in memory, update timestamps and reference count
            page_table[page_number].last_access_timestamp = timestamp;
            page_table[page_number].reference_count++;
        } else {
            // Page is not in memory

            // Check if there are free frames
            if (frame_cnt > 0) {
                // Allocate a free frame
                int frame_number = frame_pool[frame_cnt - 1];
                frame_cnt--;

                // Update the page table entry for the newly referenced page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = timestamp;
                page_table[page_number].last_access_timestamp = timestamp;
                page_table[page_number].reference_count = 1;

                // Count this page access as a page fault
                faults++;
            } else {
                // Page replacement is needed

                // Find the page with the smallest arrival_timestamp
                int min_arrival_index = -1;
                for (int j = 0; j < table_cnt; j++) {
                    if (page_table[j].is_valid) {
                        if (min_arrival_index == -1 || page_table[j].arrival_timestamp <= page_table[min_arrival_index].arrival_timestamp) {
                            min_arrival_index = j;
                        }
                    }
                }

                // Free the frame occupied by the page with the smallest arrival_timestamp
                int frame_number = page_table[min_arrival_index].frame_number;

                // Replace the page in the page table
                page_table[min_arrival_index].is_valid = 0;
                page_table[min_arrival_index].frame_number = -1;
                page_table[min_arrival_index].arrival_timestamp = -1;
                page_table[min_arrival_index].last_access_timestamp = -1;
                page_table[min_arrival_index].reference_count = -1;

                // Update the page table entry for the newly referenced page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = timestamp;
                page_table[page_number].last_access_timestamp = timestamp;
                page_table[page_number].reference_count = 1;

                // Count this page access as a page fault
                faults++;
            }
        }

        // Increment timestamp for the next page access
        timestamp++;
    }

    return faults;
}


int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    int frame_number = -1;

    // Check if the page is already in memory
    if (page_table[page_number].is_valid) {
        // Page found in memory, update timestamps and reference count
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // Page is not in memory

    // Check if there are free frames
    if (*frame_cnt > 0) {
        // Allocate a free frame
        frame_number = frame_pool[(*frame_cnt) - 1];
        (*frame_cnt)--;

        // Update the page table entry for the newly referenced page
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
    } else {
        // Page replacement is needed

        // Find the page with the smallest last_access_timestamp
        int min_access_index = -1;
        for (int i = 0; i < *table_cnt; i++) {
            if (page_table[i].is_valid) {
                if (min_access_index == -1 || page_table[i].last_access_timestamp <= page_table[min_access_index].last_access_timestamp) {
                    min_access_index = i;
                }
            }
        }

        // Free the frame occupied by the page with the smallest last_access_timestamp
        frame_number = page_table[min_access_index].frame_number;

        // Replace the page in the page table
        page_table[min_access_index].is_valid = 0;
        page_table[min_access_index].frame_number = -1;
        page_table[min_access_index].arrival_timestamp = -1;
        page_table[min_access_index].last_access_timestamp = -1;
        page_table[min_access_index].reference_count = -1;

        // Update the page table entry for the newly referenced page
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
    }

    return frame_number;
}


int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int faults = 0;
    int current_timestamp = 1;

    for (int i = 0; i < reference_cnt; i++) {
        int page_number = reference_string[i];
        int frame_number = -1;

        // Check if the page is already in memory
        if (page_table[page_number].is_valid) {
            // Page found in memory, update timestamps and reference count
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count++;
        } else {
            // Page is not in memory

            // Check if there are free frames
            if (frame_cnt > 0) {
                // Allocate a free frame
                frame_number = frame_pool[frame_cnt - 1];
                frame_cnt--;

                // Update the page table entry for the newly referenced page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;

                faults++;
            } else {
                // Page replacement is needed

                // Find the page with the smallest last_access_timestamp
                int min_access_index = -1;
                int min_access_timestamp = INT_MAX;
                for (int j = 0; j < table_cnt; j++) {
                    if (page_table[j].is_valid) {
                        if (page_table[j].last_access_timestamp <= min_access_timestamp) {
                            min_access_index = j;
                            min_access_timestamp = page_table[j].last_access_timestamp;
                        }
                    }
                }

                // Free the frame occupied by the page with the smallest last_access_timestamp
                frame_number = page_table[min_access_index].frame_number;

                // Replace the page in the page table
                page_table[min_access_index].is_valid = 0;
                page_table[min_access_index].frame_number = -1;
                page_table[min_access_index].arrival_timestamp = -1;
                page_table[min_access_index].last_access_timestamp = -1;
                page_table[min_access_index].reference_count = -1;

                // Update the page table entry for the newly referenced page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;

                faults++;
            }
        }

        current_timestamp++;
    }

    return faults;
}




int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    int frame_number = -1;

    // Check if the page is already in memory
    if (page_table[page_number].is_valid) {
        // Page found in memory, update timestamps and reference count
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // Page is not in memory

    // Check if there are free frames
    if (*frame_cnt > 0) {
        // Allocate a free frame
        frame_number = frame_pool[(*frame_cnt) - 1];
        (*frame_cnt)--;

        // Update the page table entry for the newly referenced page
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;

        return frame_number;
    }

    // Page replacement is needed

    // Find the page with the smallest reference_count
    int min_reference_index = -1;
    int min_reference_count = INT_MAX;
    int min_arrival_timestamp = INT_MAX;

    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (page_table[i].reference_count < min_reference_count ||
                (page_table[i].reference_count == min_reference_count && page_table[i].arrival_timestamp < min_arrival_timestamp)) {
                min_reference_index = i;
                min_reference_count = page_table[i].reference_count;
                min_arrival_timestamp = page_table[i].arrival_timestamp;
            }
        }
    }

    // Free the frame occupied by the page with the smallest reference_count
    frame_number = page_table[min_reference_index].frame_number;

    // Replace the page in the page table
    page_table[min_reference_index].is_valid = 0;
    page_table[min_reference_index].frame_number = -1;
    page_table[min_reference_index].arrival_timestamp = -1;
    page_table[min_reference_index].last_access_timestamp = -1;
    page_table[min_reference_index].reference_count = -1;

    // Update the page table entry for the newly referenced page
    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = frame_number;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return frame_number;
}


int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int faults = 0;
    int current_timestamp = 1;

    for (int i = 0; i < reference_cnt; i++) {
        int page_number = reference_string[i];

        // Check if the page is already in memory
        if (page_table[page_number].is_valid) {
            // Page found in memory, update timestamps and reference count
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count++;
        } else {
            // Page is not in memory

            // Check if there are free frames
            if (frame_cnt > 0) {
                // Allocate a free frame
                int frame_number = frame_pool[frame_cnt - 1];
                frame_cnt--;

                // Update the page table entry for the newly referenced page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;

                faults++;  // Count this page access as a page fault
            } else {
                // Page replacement is needed

                // Find the page with the smallest reference_count
                int min_reference_index = -1;
                int min_reference_count = INT_MAX;
                int min_arrival_timestamp = INT_MAX;

                for (int j = 0; j < table_cnt; j++) {
                    if (page_table[j].is_valid && (page_table[j].reference_count < min_reference_count ||
                                                   (page_table[j].reference_count == min_reference_count && page_table[j].arrival_timestamp < min_arrival_timestamp))) {
                        min_reference_index = j;
                        min_reference_count = page_table[j].reference_count;
                        min_arrival_timestamp = page_table[j].arrival_timestamp;
                    }
                }

                // Free the frame occupied by the page with the smallest reference_count
                int frame_number = page_table[min_reference_index].frame_number;

                // Replace the page in the page table
                page_table[min_reference_index].is_valid = 0;
                page_table[min_reference_index].frame_number = -1;
                page_table[min_reference_index].arrival_timestamp = -1;
                page_table[min_reference_index].last_access_timestamp = -1;
                page_table[min_reference_index].reference_count = -1;

                // Update the page table entry for the newly referenced page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;

                faults++;  // Count this page access as a page fault
            }
        }

        current_timestamp++;
    }

    return faults;
}


