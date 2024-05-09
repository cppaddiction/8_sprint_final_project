#pragma once
#include <algorithm>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size != 0) {
            Type* temp = new Type [size]{};
            raw_ptr_=temp;
        }
    }

    ArrayPtr(ArrayPtr&& other) {
        std::swap(raw_ptr_, other.raw_ptr_);
        other.raw_ptr_=nullptr;
    }
    
    ArrayPtr& operator=(ArrayPtr&& other) { 
        if(this!=&other)
            std::swap(raw_ptr_, other.raw_ptr_);
        return *this;
    }
    
    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    void swap(ArrayPtr& other) noexcept {          
        std::swap(raw_ptr_, other.raw_ptr_);
    }
    
    ~ArrayPtr() {
        delete [] raw_ptr_;
    }
private:
    Type* raw_ptr_ = nullptr;
};