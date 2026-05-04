////////////////////////////////////////////
//                                        //
//   CRC2 API Wrapper for ChampSim        //
//                                        //
//   Include this file in your .llc_repl  //
//   file to use the CRC2 API             //
//                                        //
////////////////////////////////////////////

#ifndef CRC2_WRAPPER_H
#define CRC2_WRAPPER_H

#include "cache.h"
#include "champsim_crc2.h"
#include "uncore.h"
#include "ooo_cpu.h"

// External declarations needed for utility functions
extern uint64_t current_core_cycle[NUM_CPUS];
extern O3_CPU ooo_cpu[NUM_CPUS];

// Configuration number (set during build, can be overridden)
#ifndef CONFIG_NUMBER
#define CONFIG_NUMBER 1
#endif

// Implementation of CRC2 utility functions
uint64_t get_cycle_count() {
    return current_core_cycle[0];
}

uint64_t get_instr_count(uint32_t cpu) {
    if (cpu < NUM_CPUS) {
        return ooo_cpu[cpu].num_retired;
    }
    return 0;
}

uint64_t get_config_number() {
    return CONFIG_NUMBER;
}

// CACHE member function implementations that call CRC2 API

void CACHE::llc_initialize_replacement() {
    InitReplacementState();
}

uint32_t CACHE::llc_find_victim(uint32_t cpu, uint64_t instr_id, uint32_t set,
                                 const BLOCK *current_set, uint64_t ip,
                                 uint64_t full_addr, uint32_t type) {
    // Call CRC2 API (note: CRC2 uses PC instead of ip, and paddr instead of full_addr)
    return GetVictimInSet(cpu, set, current_set, ip, full_addr, type);
}

void CACHE::llc_update_replacement_state(uint32_t cpu, uint32_t set, uint32_t way,
                                          uint64_t full_addr, uint64_t ip,
                                          uint64_t victim_addr, uint32_t type,
                                          uint8_t hit) {
    // Call CRC2 API (note: CRC2 parameter order is different)
    // CRC2: (cpu, set, way, paddr, PC, victim_addr, type, hit)
    // ChampSim: (cpu, set, way, full_addr, ip, victim_addr, type, hit)
    UpdateReplacementState(cpu, set, way, full_addr, ip, victim_addr, type, hit);
}

void CACHE::llc_replacement_final_stats() {
    PrintStats();
}

#endif
