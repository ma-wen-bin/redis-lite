# Perfomance Testing

## Supported Commands

| Command | Description | Example |
|---|---|---|
| `GET` | Get the string value of a key | `redis-benchmark -p 6380 GET <key>` |
| `SET` | Set a key to a string value | `redis-benchmark -p 6380 SET <key> <value>` |
| `DEL` | Delete one or more keys | `redis-benchmark -p 6380 DEL <key>` |
| `STRLEN` | Get the length of a string value | `redis-benchmark -p 6380 STRLEN <key>` |
| `APPEND` | Append a value to an existing string | `redis-benchmark -p 6380 APPEND <key> <value>` |
| `LPUSH` | Push one or more values onto the front of a list | `redis-benchmark -p 6380 LPUSH <key> <value>` |
| `RPUSH` | Push one or more values onto the back of a list | `redis-benchmark -p 6380 RPUSH <key> <value>` |
| `LRANGE` | Get a range of elements from a list | `redis-benchmark -p 6380 LRANGE <key> <start> <stop>` |
| `HSET` | Set a field in a hash | `redis-benchmark -p 6380 HSET <key> <field> <value>` |
| `HGET` | Get a field's value from a hash | `redis-benchmark -p 6380 HGET <key> <field>` |
| `HDEL` | Delete one or more fields from a hash | `redis-benchmark -p 6380 HDEL <key> <field>` |
| `SADD` | Add one or more members to a set | `redis-benchmark -p 6380 SADD <key> <value>` |
| `SREM` | Remove one or more members from a set | `redis-benchmark -p 6380 SREM <key> <value>` |
| `SMEMBERS` | Get all members of a set | `redis-benchmark -p 6380 SMEMBERS <key>` |
| `SCARD` | Get the number of members in a set | `redis-benchmark -p 6380 SCARD <key>` |
| `SISMEMBER` | Check whether a value is a member of a set | `redis-benchmark -p 6380 SISMEMBER <key> <value>` |

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


## Commands Supported 


## Benchmark Results  

### Single Client Blocking I/O

Command ran (5 times back-to-back, same server process): `redis-benchmark -h 127.0.0.1 -p 6380 -c 1 -n 50000 -d 100 -t set,get`

Note : `-c 1` represents a single client 

**SET, GET operations | 50k requests per operation, per run | 100 byte payloads**

### SET

| Metric | Run 1 | Run 2 | Run 3 | Run 4 | Run 5 | **Average** |
|---|---|---|---|---|---|---|
| Throughput (requests/sec) | 11742.60 | 7664.01 | 8021.82 | 7757.95 | 7862.87 | **8609.85** |
| Avg latency (ms) | 0.134 | 0.123 | 0.118 | 0.122 | 0.120 | **0.123** |
| Min latency (ms) | 0.064 | 0.064 | 0.056 | 0.064 | 0.064 | **0.062** |
| p50 latency (ms) | 0.127 | 0.119 | 0.111 | 0.111 | 0.111 | **0.116** |
| p95 latency (ms) | 0.215 | 0.191 | 0.175 | 0.191 | 0.191 | **0.193** |
| p99 latency (ms) | 0.311 | 0.263 | 0.247 | 0.263 | 0.255 | **0.268** |
| Max latency (ms) | 3.735 | 7.631 | 3.023 | 3.671 | 1.303 | **3.873** |

### GET

| Metric | Run 1 | Run 2 | Run 3 | Run 4 | Run 5 | **Average** |
|---|---|---|---|---|---|---|
| Throughput (requests/sec) | 7568.88 | 7541.48 | 13524.48 | 7994.88 | 8199.41 | **8965.83** |
| Avg latency (ms) | 0.124 | 0.125 | 0.123 | 0.118 | 0.115 | **0.121** |
| Min latency (ms) | 0.064 | 0.064 | 0.056 | 0.064 | 0.056 | **0.061** |
| p50 latency (ms) | 0.119 | 0.119 | 0.111 | 0.111 | 0.103 | **0.113** |
| p95 latency (ms) | 0.199 | 0.191 | 0.191 | 0.175 | 0.167 | **0.185** |
| p99 latency (ms) | 0.279 | 0.263 | 0.263 | 0.255 | 0.239 | **0.260** |
| Max latency (ms) | 1.327 | 2.015 | 3.151 | 4.359 | 1.415 | **2.453** |


### Single Threaded Non-Blocking I/O using epoll

Command ran (5 times back-to-back, same server process): `redis-benchmark -h 127.0.0.1 -p 6380 -c 50 -n 50000 -d 100 -t set,get`

Note : `-c50` represents 50 clients (redis-benchmark default)

**SET, GET operations | 50k requests per operation, per run | 100 byte payloads**

### SET

| Metric | Run 1 | Run 2 | Run 3 | Run 4 | Run 5 | **Average** |
|---|---|---|---|---|---|---|
| Throughput (requests/sec) | 25406.50 | 25654.18 | 23607.18 | 24319.07 | 23786.87 | **24554.76** |
| Avg latency (ms) | 1.961 | 1.941 | 2.111 | 2.046 | 2.094 | **2.031** |
| Min latency (ms) | 0.504 | 0.504 | 0.520 | 0.176 | 0.448 | **0.430** |
| p50 latency (ms) | 1.951 | 1.895 | 2.079 | 1.983 | 2.071 | **1.996** |
| p95 latency (ms) | 2.319 | 2.391 | 2.551 | 2.559 | 2.559 | **2.476** |
| p99 latency (ms) | 5.367 | 5.319 | 5.887 | 5.647 | 5.567 | **5.557** |
| Max latency (ms) | 7.047 | 7.575 | 7.831 | 9.407 | 9.447 | **8.261** |

### GET

| Metric | Run 1 | Run 2 | Run 3 | Run 4 | Run 5 | **Average** |
|---|---|---|---|---|---|---|
| Throughput (requests/sec) | 27964.21 | 26896.18 | 26413.10 | 26001.04 | 26399.15 | **26734.74** |
| Avg latency (ms) | 1.556 | 1.811 | 1.803 | 1.748 | 1.621 | **1.708** |
| Min latency (ms) | 0.208 | 0.392 | 0.288 | 0.256 | 0.184 | **0.266** |
| p50 latency (ms) | 1.575 | 1.815 | 1.823 | 1.703 | 1.583 | **1.700** |
| p95 latency (ms) | 2.183 | 2.399 | 2.287 | 2.447 | 2.335 | **2.330** |
| p99 latency (ms) | 2.551 | 2.639 | 2.967 | 3.583 | 2.927 | **2.933** |
| Max latency (ms) | 6.447 | 8.111 | 8.079 | 7.319 | 7.525 | **7.496** |

