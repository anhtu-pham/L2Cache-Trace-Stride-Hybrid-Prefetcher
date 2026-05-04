#!/bin/bash

PREFETCHER=$1

./build_champsim.sh bimodal no "${PREFETCHER}" lru_crc2 1
"./bin/bimodal-no-${PREFETCHER}-lru_crc2-1core" -warmup_instructions 1000000 -simulation_instructions 10000000 -traces ./trace/mcf_10M.trace.gz