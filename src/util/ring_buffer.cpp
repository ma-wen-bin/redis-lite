#include <cstddef>
#include <cstdint>
#include <vector> 
#include "ring_buffer.h"
using namespace std;

    //constructor 
    RingBuffer::RingBuffer(size_t size) : max_size(size), buffer(size, 0) {

    };

    //return (headptr, writeable length)
    pair<uint8_t*, size_t> RingBuffer::writeableSpan() {

        uint8_t* headptr = &buffer[head];
        size_t length;

        if (full) { 
            return {headptr, 0}; 
        }

        if (head < tail ) { // if head behind tail (wrapped around) -> write up to tail 
            length = tail - head;
        } else { 
            length = max_size - head; // write up to the end of the array 
        }

        return {headptr, length};
    }

    void RingBuffer::insert(const std::vector<uint8_t>& bytes, int offset, const size_t length) {
        std::copy(bytes.begin() + offset, bytes.begin() + offset + length, buffer.begin() + head);
        commitWrite(length);
    }

    //update head pointer 
    void RingBuffer::commitWrite(size_t bytesRead) {
        head = (head + bytesRead) % max_size;
        if (head == tail) {
            full = true;
        }
    }

    //peek for consumer 
    pair<uint8_t*, size_t> RingBuffer::peek() {

        uint8_t* tailptr = &buffer[tail];
        size_t length;

        if (full) {  
            return {tailptr, max_size}; 
        }

        if (head == tail) {
            return {tailptr, 0};
        }

        if (head > tail) {
            length = head - tail;
        } else {
            length = max_size - tail; // contiguous run only; caller loops for the rest after wrap
        }
        
        return {tailptr, length};
    }

    //method for consumer 
    void RingBuffer::consume(size_t bytes) {
        tail = (tail + bytes) % max_size;
        full = false;
    }

    //resetting the buffer to empty 
    void RingBuffer::reset() {
        head = tail;
        full = false;
    }
        
    bool RingBuffer::isFull() const {
        return full;
    }

    bool RingBuffer::isEmpty() const {
        return !full && head == tail;
    }

    size_t RingBuffer::size() const {
        if (full) { return max_size; }
        if (head >= tail) { return head - tail;} 
        return (max_size - tail) + head;
    }