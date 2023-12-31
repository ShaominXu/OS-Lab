#include "oslabs.h"
#include <stdlib.h>
#include <limits.h>

struct RCB NULLRCB = {0,0,0,0};


struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.request_id == 0) {
        // Disk is free, service the new request immediately
        return new_request;
    } else {
        // Add the new request to the queue and return the current_request
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;
        return current_request;
    }
}

struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt) {
    if (*queue_cnt == 0) {
        // No request in the queue
        return NULLRCB;
    } else {
        // Get the request with the earliest arrival time
        struct RCB next_request = request_queue[0];

        // Shift the queue to remove the serviced request
        for (int i = 0; i < *queue_cnt - 1; i++) {
            request_queue[i] = request_queue[i + 1];
        }

        (*queue_cnt)--;
        return next_request;
    }
}

struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.request_id == 0) {
        // Disk is free, service the new request immediately
        return new_request;
    } else {
        // Add the new request to the queue and return the current_request
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;
        return current_request;
    }
}

struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder) {
    if (*queue_cnt == 0) {
        // No request in the queue
        return NULLRCB;
    } else {
        // Find the request with the closest cylinder and earliest arrival time
        int min_distance = abs(request_queue[0].cylinder - current_cylinder);
        int min_index = 0;

        for (int i = 1; i < *queue_cnt; i++) {
            int distance = abs(request_queue[i].cylinder - current_cylinder);
            if (distance < min_distance || (distance == min_distance && request_queue[i].arrival_timestamp < request_queue[min_index].arrival_timestamp)) {
                min_distance = distance;
                min_index = i;
            }
        }

        // Get the next request
        struct RCB next_request = request_queue[min_index];

        // Shift the queue to remove the serviced request
        for (int i = min_index; i < *queue_cnt - 1; i++) {
            request_queue[i] = request_queue[i + 1];
        }

        (*queue_cnt)--;
        return next_request;
    }
}

struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.request_id == 0) {
        // Disk is free, service the new request immediately
        return new_request;
    } else {
        // Add the new request to the queue and return the current_request
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;
        return current_request;
    }
}


struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder, int scan_direction) {
    if (*queue_cnt == 0) {
        // No request in the queue
        return NULLRCB;
    }

    // Initialize variables to track the selected RCB
    int selected_index = -1;
    int min_arrival_time = INT_MAX;
    int min_cylinder_diff = INT_MAX;

    // Iterate through the request queue
    for (int i = 0; i < *queue_cnt; i++) {
        struct RCB current_request = request_queue[i];

        // Check if the request has the same cylinder as the current cylinder
        if (current_request.cylinder == current_cylinder) {
            // Pick the one with the earliest arrival time
            if (current_request.arrival_timestamp < min_arrival_time) {
                selected_index = i;
                min_arrival_time = current_request.arrival_timestamp;
                //min_cylinder_diff = abs(current_request.cylinder - current_cylinder);
            }
        } else if (scan_direction == 1 && current_request.cylinder > current_cylinder) {
            // Scan direction is 1 and there are cylinders larger than the current cylinder
            // Pick the one with the closest cylinder
            int current_cylinder_diff = current_request.cylinder - current_cylinder;
            if (current_cylinder_diff < min_cylinder_diff) {
                selected_index = i;
                min_cylinder_diff = current_cylinder_diff;
            }       
        } else if (scan_direction == 0 && current_request.cylinder < current_cylinder) {
            // Scan direction is 0 and there are cylinders smaller than the current cylinder
            // Pick the one with the closest cylinder
            int current_cylinder_diff = current_cylinder - current_request.cylinder;
            if (current_cylinder_diff < min_cylinder_diff) {
                selected_index = i;
                min_cylinder_diff = current_cylinder_diff;
            }
        } 
    }

    if (selected_index == -1) {
        for (int i = 0; i < *queue_cnt; i++) {
            struct RCB current_request = request_queue[i];

            if (scan_direction == 1 && current_request.cylinder < current_cylinder) {
                // Scan direction is 1 and there are cylinders smaller than the current cylinder
                // Pick the one with the closest cylinder
                int current_cylinder_diff = current_cylinder - current_request.cylinder;
                if (current_cylinder_diff < min_cylinder_diff) {
                    selected_index = i;
                    min_cylinder_diff = current_cylinder_diff;
                }       
            } else if (scan_direction == 0 && current_request.cylinder > current_cylinder) {
                // Scan direction is 0 and there are cylinders lager than the current cylinder
                // Pick the one with the closest cylinder
                int current_cylinder_diff = current_request.cylinder - current_cylinder;
                if (current_cylinder_diff < min_cylinder_diff) {
                    selected_index = i;
                    min_cylinder_diff = current_cylinder_diff;
                }
            } 
        }
    }

    // Check if a request is selected
    if (selected_index != -1) {
        struct RCB selected_request = request_queue[selected_index];

        // Remove the selected RCB from the queue
        for (int j = selected_index; j < *queue_cnt - 1; j++) {
            request_queue[j] = request_queue[j + 1];
        }
        (*queue_cnt)--;

        return selected_request;
    } else {
        return NULLRCB;
    }
}

