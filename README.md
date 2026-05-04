# Trace-Stride Hybrid Prefetcher for L2 Cache

## Motivation

Data-intensive applications may include regular, consistent stride access pattern, consecutively inconsistent and irregular access pattern, random or unrecognizable access patterns, or even mixture from these types of access patterns.

## Overview

Trace-Stride Hybrid Prefetcher performs prefetching for L2 Cache and effectively supports programs with mixture of above patterns. It can dynamically select prefetching mode for each access instruction in the program based on pattern detection over time without causing cache pollution.

Trace-Stride Hybrid Prefetcher has 3 prefetching modes: (1) dynamic trace-based prefetching, (2) dynamic stride-based prefetching, (3) no prefetching, along with history memory buffer that stores necessary data and drives the decision of prefetching behaviors.

## Architecture Design

### History Memory Buffer

<p align="center">
    <img src="img/history_mem_buffer.png">
</p>

### Per-entry FSM Counter

<p align="center">
    <img src="img/FSM.png">
</p>

### Dynamic Trace-based Prefetcher

<p align="center">
    <img src="img/trace_pref.png">
</p>

### Dynamic Stride-based Prefetcher

## Performance Benchmarking

### Configuration setup for all scenarios
* Simulation nvironment: ChampSim CRC2 simulator.
* Branch predictor: bimodal.
* Cache replacement: lru_crc2.
* Core: 1.
* Trace: mcf_10M.trace.gz (irregular-access, cache-miss-likely, low-IPC workload with baseline IPC of just 0.0864461).
* Number of warmup instructions: 1,000,000.
* Number of simulation instructions: 10,000,000.
* Number of signatures for Trace-Stride: 64.

### Trace-Stride Hybrid Prefetcher's Performance

<p align="center">
    <img src="img/hybrid_perf.png">
</p>

## Comparison of different L2C Prefetchers' Performance

<p align="center">
    <img src="img/comparison.png">
</p>