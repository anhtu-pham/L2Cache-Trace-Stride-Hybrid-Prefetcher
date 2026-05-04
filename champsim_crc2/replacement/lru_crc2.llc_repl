////////////////////////////////////////////
//                                        //
//     LRU replacement policy (CRC2 API)  //
//                                        //
//     Example implementation using the   //
//     CRC2 interface for ChampSim        //
//                                        //
////////////////////////////////////////////

#include "crc2_wrapper.h"

// Define cache parameters (must match ChampSim config)
#define LLC_SETS LLC_SET
#define LLC_WAYS LLC_WAY

// LRU state array
uint32_t lru[LLC_SETS][LLC_WAYS];

// Initialize replacement state
void InitReplacementState() {
    cout << "Initialize LRU replacement state (CRC2 API)" << endl;

    for (int i = 0; i < LLC_SETS; i++) {
        for (int j = 0; j < LLC_WAYS; j++) {
            lru[i][j] = j;
        }
    }
}

// Find replacement victim
// Return value should be 0 ~ (LLC_WAYS-1) or LLC_WAYS for bypass
uint32_t GetVictimInSet(uint32_t cpu, uint32_t set, const BLOCK *current_set,
                        uint64_t PC, uint64_t paddr, uint32_t type) {
    // Find the LRU line (the one with highest lru value)
    for (int i = 0; i < LLC_WAYS; i++) {
        if (lru[set][i] == (LLC_WAYS - 1)) {
            return i;
        }
    }

    // Should never reach here
    return 0;
}

// Called on every cache hit and cache fill
void UpdateReplacementState(uint32_t cpu, uint32_t set, uint32_t way,
                            uint64_t paddr, uint64_t PC, uint64_t victim_addr,
                            uint32_t type, uint8_t hit) {
    // Update LRU state
    for (uint32_t i = 0; i < LLC_WAYS; i++) {
        if (lru[set][i] < lru[set][way]) {
            lru[set][i]++;

            if (lru[set][i] == LLC_WAYS) {
                assert(0);
            }
        }
    }
    lru[set][way] = 0;  // Promote to MRU position
}

// Print stats on every heartbeat
void PrintStats_Heartbeat() {
    // Optional: print periodic stats
}

// Print stats at end of simulation
void PrintStats() {
    // Optional: print final stats
}
