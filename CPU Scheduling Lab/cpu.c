#include "oslabs.h"


struct PCB NULLPCB = {0, 0, 0, 0, 0, 0, 0};

struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    // If there is no currently running process, set the execution details for the new process
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // If the new process has lower or equal priority, add it to the ready queue
    if (new_process.process_priority >= current_process.process_priority) {
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return current_process;
    }

    // If the new process has higher priority, swap with the current process in the ready queue
    new_process.execution_starttime = timestamp;
    new_process.execution_endtime = timestamp + new_process.total_bursttime;
    new_process.remaining_bursttime = new_process.total_bursttime;
    current_process.remaining_bursttime = current_process.execution_endtime - timestamp;
    current_process.execution_endtime = 0;
    ready_queue[*queue_cnt] = current_process;
    (*queue_cnt)++;

    return new_process;
}

struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    // If the ready queue is empty, return NULLPCB
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    // Find the PCB with the highest priority in the ready queue
    int highest_priority_index = 0;
    for (int i = 1; i < *queue_cnt; ++i) {
        if (ready_queue[i].process_priority < ready_queue[highest_priority_index].process_priority) {
            highest_priority_index = i;
        }
    }

    // Remove the PCB with the highest priority from the ready queue
    struct PCB next_process = ready_queue[highest_priority_index];
    for (int i = highest_priority_index; i < *queue_cnt - 1; ++i) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;

    // Modify the next process to set execution start time and execution end time
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    return next_process;
}


struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int time_stamp) {
    // If there is no currently running process, set the execution details for the new process
    if (current_process.process_id == 0) {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // If the new process has a longer or equal burst time, add it to the ready queue
    if (new_process.total_bursttime >= current_process.remaining_bursttime) {
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return current_process;
    }

    // If the new process has a shorter burst time, swap with the current process in the ready queue
    new_process.execution_starttime = time_stamp;
    new_process.execution_endtime = time_stamp + new_process.total_bursttime;
    new_process.remaining_bursttime = new_process.total_bursttime;
    current_process.remaining_bursttime = current_process.execution_endtime - time_stamp;
    current_process.execution_endtime = 0;
    current_process.execution_starttime = 0;
    ready_queue[*queue_cnt] = current_process;
    (*queue_cnt)++;

    return new_process;
}


struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    // If the ready queue is empty, return NULLPCB
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    // Find the PCB with the smallest remaining burst time
    int smallest_index = 0;
    for (int i = 1; i < *queue_cnt; i++) {
        if (ready_queue[i].remaining_bursttime < ready_queue[smallest_index].remaining_bursttime) {
            smallest_index = i;
        }
    }

    // Remove the PCB from the ready queue and set its execution details
    struct PCB next_process = ready_queue[smallest_index];
    for (int i = smallest_index; i < *queue_cnt - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;
    
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    return next_process;
}

struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum) {
    // If there is no currently running process, set the execution details for the new process
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + (time_quantum < new_process.total_bursttime ? time_quantum : new_process.total_bursttime);
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // Add the new process to the ready queue and return the currently running process
    ready_queue[*queue_cnt] = new_process;
    (*queue_cnt)++;
    new_process.execution_starttime = 0;
    new_process.execution_endtime = 0;
    new_process.remaining_bursttime = new_process.total_bursttime;

    return current_process;
}

struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    // Check if the ready queue is empty
    if (*queue_cnt == 0) {
        return NULLPCB; // No process to execute next
    }

    // Find the process with the earliest arrival time in the ready queue
    int earliest_index = 0;
    for (int i = 1; i < *queue_cnt; i++) {
        if (ready_queue[i].arrival_timestamp < ready_queue[earliest_index].arrival_timestamp) {
            earliest_index = i;
        }
    }

    // Remove the selected process from the ready queue
    struct PCB next_process = ready_queue[earliest_index];
    for (int i = earliest_index; i < *queue_cnt - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;

    // Modify the PCB before returning
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + (time_quantum < next_process.remaining_bursttime ? time_quantum : next_process.remaining_bursttime);

    return next_process;
}
