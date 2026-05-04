#include "cache.h"

#define NUM_SIGNATURES 64
#define THRESHOLD 0
#define MAX 3
#define MIN (0 - MAX)
#define N 3
#define MAX_NUM_OFFSETS 4

struct HistoryEntry {
    uint64_t associated_ip;
    int8_t prefetch_counter;

    // Parameters for stride-based prefetching
    uint64_t prev_block;
    int64_t prev_stride;

    // Parameters for trace-based prefetching
    int64_t prev_page;
    uint64_t first_access_block;
    int8_t access_block_offsets[MAX_NUM_OFFSETS];
    int8_t predicted_block_offsets[MAX_NUM_OFFSETS];
    int offset_idx;
    int num_offsets_matches;
};

HistoryEntry history_buffer[NUM_SIGNATURES];


void CACHE::l2c_prefetcher_initialize() 
{
    cout << "CPU " << cpu << " Hybrid prefetcher" << endl;
    
    for (auto &entry : history_buffer) {
        entry.associated_ip = 0;
        entry.prefetch_counter = THRESHOLD;
        entry.prev_block = 0;
        entry.prev_stride = 0;
        entry.prev_page = -1;
        entry.first_access_block = 0;
        for (int i = 0; i < MAX_NUM_OFFSETS; i++) {
            entry.access_block_offsets[i] = 0;
            entry.predicted_block_offsets[i] = 0;
        }
        entry.offset_idx = 0;
        entry.num_offsets_matches = 0;
    }
}

void CACHE::l2c_prefetcher_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type)
{
    uint64_t curr_block = addr >> LOG2_BLOCK_SIZE;
    uint64_t curr_page = curr_block >> LOG2_PAGE_SIZE;

    HistoryEntry &curr_entry = history_buffer[ip % NUM_SIGNATURES];

    // If reach new ip with same signature, reset
    if (ip != curr_entry.associated_ip) {
        curr_entry.associated_ip = ip;
        curr_entry.prefetch_counter = THRESHOLD;
        curr_entry.prev_block = 0;
        curr_entry.prev_stride = 0;
        curr_entry.prev_page = -1;
        curr_entry.first_access_block = 0;
        curr_entry.offset_idx = 0;
        for (int i = 0; i < MAX_NUM_OFFSETS; i++) {
            curr_entry.access_block_offsets[i] = 0;
            curr_entry.predicted_block_offsets[i] = 0;
        }
        curr_entry.num_offsets_matches = 0;
    }


    // Configure for stride-based prefetching side
    int64_t curr_stride = curr_block - curr_entry.prev_block;
    if (curr_entry.prefetch_counter <= THRESHOLD) {
        if (curr_stride == 0) {
            return;
        } else if (curr_stride != curr_entry.prev_stride) {
            curr_entry.prefetch_counter = (curr_entry.prefetch_counter < THRESHOLD) ? curr_entry.prefetch_counter + 1 : THRESHOLD;
            curr_entry.prev_stride = curr_stride;
        } else {
            curr_entry.prefetch_counter = (curr_entry.prefetch_counter > MIN) ? curr_entry.prefetch_counter - 1 : MIN;
        }
    }

    // Configure for trace-based prefetching side
    if (curr_entry.prefetch_counter >= THRESHOLD) {
        if (curr_page != curr_entry.prev_page) { // Reach new page
            // Configure counter and trace-based prefetching prediction based on matches
            if (curr_entry.num_offsets_matches >= curr_entry.offset_idx) {
                curr_entry.prefetch_counter = (curr_entry.prefetch_counter < MAX) ? curr_entry.prefetch_counter + 1 : MAX;
            } else {
                curr_entry.prefetch_counter = (curr_entry.prefetch_counter > THRESHOLD) ? curr_entry.prefetch_counter - 1 : THRESHOLD;
                // Update with new prediction
                for (int i = 0; i < MAX_NUM_OFFSETS; i++) {
                    curr_entry.predicted_block_offsets[i] = curr_entry.access_block_offsets[i];
                }
            }

            // Setup for new page
            curr_entry.prev_page = curr_page;
            curr_entry.first_access_block = curr_block;
            curr_entry.offset_idx = 0;
            curr_entry.num_offsets_matches = 0;

        } else { // In current page
            // Capture current offset from first-access block in current page
            int8_t curr_access_block_offset = curr_block - curr_entry.first_access_block;
            if (curr_entry.offset_idx < MAX_NUM_OFFSETS) {
                curr_entry.access_block_offsets[curr_entry.offset_idx] = curr_access_block_offset;
                curr_entry.offset_idx++;
            }

            // Check for prediction matching
            for (int i = 0; i < MAX_NUM_OFFSETS; i++) {
                if (curr_access_block_offset != 0 && curr_access_block_offset == curr_entry.predicted_block_offsets[i]) {
                    curr_entry.num_offsets_matches++;
                    break;
                }
            }
            return;
        }
    }


    if (curr_entry.prefetch_counter < THRESHOLD) { // Perform stride-based prefetching
        for (int i = 1; i <= N; i++) {
            uint64_t stride_block_addr_to_prefetch = (curr_block + i * curr_stride) << LOG2_BLOCK_SIZE;
            if ((stride_block_addr_to_prefetch >> LOG2_PAGE_SIZE) == (addr >> LOG2_PAGE_SIZE)) {
                prefetch_line(ip, addr, stride_block_addr_to_prefetch, FILL_L2);
            }
        }
    } else if (curr_entry.prefetch_counter != THRESHOLD) { // Perform trace-based prefetching
        for (int i = 0; i < MAX_NUM_OFFSETS; i++) {
            int8_t predicted_block_offset = curr_entry.predicted_block_offsets[i];
            if (predicted_block_offset != 0) {
                uint64_t trace_block_addr_to_prefetch = (curr_entry.first_access_block + predicted_block_offset) << LOG2_BLOCK_SIZE;
                prefetch_line(ip, addr, trace_block_addr_to_prefetch, FILL_L2);
            }
        }
    }
    
    curr_entry.prev_block = curr_block;
}

void CACHE::l2c_prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr)
{
    
}

void CACHE::l2c_prefetcher_final_stats()
{
    cout << "CPU " << cpu << " Hybrid prefetcher final stats" << endl;
}