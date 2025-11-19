#ifndef PMR_STACK_H
#define PMR_STACK_H

#include <memory_resource>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <new> 

class FixedBlockVectorResource : public std::pmr::memory_resource {

private:
    struct MemoryBlock {
        void* ptr;
        size_t size;
    };

    void* global_buffer;       
    size_t total_size;         
    size_t current_offset;     
    
    std::vector<MemoryBlock> used_blocks; 
    std::vector<MemoryBlock> free_blocks; 

public:
    explicit FixedBlockVectorResource(size_t size) 
        : total_size(size), current_offset(0) {
        global_buffer = ::operator new(size);
    }

    ~FixedBlockVectorResource() override {
        if (!used_blocks.empty()) {
             std::cerr << "RES-LEAK DETECTED" << used_blocks.size() << "blocks lost" << std::endl;
        }
        ::operator delete(global_buffer);
    }

    FixedBlockVectorResource(const FixedBlockVectorResource&) = delete;
    FixedBlockVectorResource& operator=(const FixedBlockVectorResource&) = delete;

protected:
    void* do_allocate(size_t bytes, size_t /*alignment*/) override {
        for (auto it = free_blocks.begin(); it != free_blocks.end(); ++it) {
            if (it->size >= bytes) {
                void* ptr = it->ptr;
                used_blocks.push_back(*it);
                free_blocks.erase(it);
                std::cout << "ALLOC - reuse:" << ptr << std::endl;
                return ptr;
            }
        }
        if (current_offset + bytes > total_size) {
            throw std::bad_alloc();
        }

        void* ptr = static_cast<char*>(global_buffer) + current_offset;
        current_offset += bytes;
        
        used_blocks.push_back({ptr, bytes});
        std::cout << "ALLOC - new:" << ptr << std::endl;
        
        return ptr;
    }

    void do_deallocate(void* p, size_t /*bytes*/, size_t /*alignment*/) override {
        auto it = std::find_if(used_blocks.begin(), used_blocks.end(),
            [p](const MemoryBlock& block) { return block.ptr == p; });

        if (it != used_blocks.end()) {
            free_blocks.push_back(*it);
            used_blocks.erase(it);
            std::cout << "DEALLOC - free:" << p << std::endl;
        }
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

template<typename T>

class PmrStack {

private:
    struct Node {

        T value;
        Node* next;
        template<typename... Args>
        Node(Node* n, Args&&... args) 
            : value(std::forward<Args>(args)...), next(n) {}

    };

    Node* head;
    std::pmr::polymorphic_allocator<Node> allocator;

public:
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    explicit PmrStack(std::pmr::memory_resource* res = std::pmr::get_default_resource()) 
        : head(nullptr), allocator(res) 
    {}

    ~PmrStack() {
        clear();
    }

    PmrStack(const PmrStack&) = delete;
    PmrStack& operator=(const PmrStack&) = delete;

    template<typename... Args>
    void push(Args&&... args) {
        Node* node_mem = allocator.allocate(1);
        try {
            allocator.construct(node_mem, head, std::forward<Args>(args)...);
        } catch (...) {
            allocator.deallocate(node_mem, 1);
            throw;
        }
        head = node_mem;
    }

    void pop() {

        if (!head){ 
            return;
        }
        Node* temp = head;
        head = head->next;
        allocator.destroy(temp);
        allocator.deallocate(temp, 1);

    }

    reference top() {
        if (!head) {
            throw std::logic_error("stack is empty");
        }
        return head->value;
    }
    
    const_reference top() const {
        if (!head) {
            throw std::logic_error("stack is empty");
        }
        return head->value;
    }

    bool empty() const { 
        return head == nullptr; 
    }

    void clear() {
        while (head){ 
            pop();
        }
    }

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* node) : current(node) {}

        reference operator*() const { return current->value; }
        pointer operator->() const { return &current->value; }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }

    private:

        Node* current;
    };

    Iterator begin() { 
        return Iterator(head); 
    }
    Iterator end() { 
        return Iterator(nullptr); 
    }
    Iterator begin() const { 
        return Iterator(head); 
    }
    Iterator end() const { 
        return Iterator(nullptr); 
    }
};

#endif