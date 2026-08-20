#pragma once 

#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>

class RingBuffer {
    private:
    size_t head = 0;
    size_t tail = 0;
    const size_t max_size;
    std::vector<uint8_t> buffer;
    bool full = false;

    public:
    RingBuffer(size_t size);
    std::pair<uint8_t*, size_t> writeableSpan();
    void commitWrite(size_t bytesRead);
    std::pair<const uint8_t*, size_t> peek() const;
    void consume(size_t bytes);
    void reset();
    bool isFull() const;
    bool isEmpty() const;
    size_t size() const;
};
