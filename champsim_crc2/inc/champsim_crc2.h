////////////////////////////////////////////
//                                        //
//        DO NOT CHANGE THIS FILE         //
//                                        //
//      ChampSim interface for CRC-2      //
//     Adapted for ChampSim source build  //
//                                        //
////////////////////////////////////////////

#ifndef CHAMPSIM_CRC2_H
#define CHAMPSIM_CRC2_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <string>

// Include the ChampSim BLOCK definition
// The ChampSim BLOCK class has the same interface as CRC2 BLOCK
#include "block.h"

// CACHE ACCESS TYPE (these are already defined in block.h, but we include
// them here for CRC2 API compatibility in case students don't include block.h)
#ifndef LOAD
#define LOAD      0
#define RFO       1
#define PREFETCH  2
#define WRITEBACK 3
#define NUM_TYPES 4
#endif

using namespace std;

// CRC2 API function declarations
// Students implement these functions

// Initialize replacement state
void InitReplacementState();

// Find replacement victim
// Return value should be 0 ~ (LLC_WAYS-1) or LLC_WAYS for bypass
uint32_t GetVictimInSet(uint32_t cpu, uint32_t set, const BLOCK *current_set,
                        uint64_t PC, uint64_t paddr, uint32_t type);

// Called on every cache hit and cache fill
void UpdateReplacementState(uint32_t cpu, uint32_t set, uint32_t way,
                            uint64_t paddr, uint64_t PC, uint64_t victim_addr,
                            uint32_t type, uint8_t hit);

// Print stats on every heartbeat (every 1M instructions)
void PrintStats_Heartbeat();

// Print stats at end of simulation
void PrintStats();

// Utility functions available to students
uint64_t get_cycle_count();
uint64_t get_instr_count(uint32_t cpu);
uint64_t get_config_number();

#endif
