# Perfomance Testing

## Hardware Details 
| Component | Value |
|---|---|
| CPU | Intel Core i7-11700K @ 3.60GHz (8C/16T) | 
| RAM | 23 GiB (as allocated to WSL2) |
| OS  | Ubuntu 24.04.1 LTS |
| Kernel | 6.18.33.2-microsoft-standard-WSL2 |
| Compiler | g++ 13.3.0 |
| Network | loopback (127.0.0.1) |
| Benchmark Client | [redis-benchmark 7.0.15](https://redis.io/docs/latest/operate/oss_and_stack/management/optimization/benchmarks/) | 

## Benchmark Results  

Command Ran: `redis-benchmark -h 127.0.0.1 -p 6380 -c 1 -n 50000 -d 100 -t set,get`
Note : `-c 1` is required for now since the server only accepts one connection at a time

**SET, GET operations | 50k request PER operation | 100 byte payloads**

| Metric | SET | GET |
|---|---|---|
| Throughput (requests/sec) | 7503.00 | 1586.04 |
| Avg latency (ms) | 0.106 | 0.120 |
| Min latency (ms) | 0.056 | 0.056 |
| p50 latency (ms) | 0.095 | 0.119 |
| p95 latency (ms) | 0.151 | 0.175 |
| p99 latency (ms) | 0.215 | 0.215 |
| Max latency (ms) | 1.319 | 1.199 |

