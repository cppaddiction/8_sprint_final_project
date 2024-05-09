#pragma once
#include <cassert>
#include "array_ptr.h"

class ReserveProxyObj {
public:    
    explicit ReserveProxyObj (size_t capacity_to_reserve) : capacity_(capacity_to_reserve) {}
    
    size_t ReserveCapacity() {
        return capacity_;
    }  
private:    
    size_t capacity_;
};
   
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
} 


template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit  SimpleVector(ReserveProxyObj capacity_to_reserve) {
        Reserve(capacity_to_reserve.ReserveCapacity());
        size_ = 0;
    }

    explicit SimpleVector(size_t size, const Type& value=Type{}) {
        ArrayPtr <Type> temp (size);
        items_.swap(temp);
        size_ = size;
        capacity_ = size;
        if(value!=Type{})
            std::fill(begin(), end(), value);      
    }

    SimpleVector(std::initializer_list<Type> init) {    
        RethrowItemsFromIterable(init, init.size());
    }

    SimpleVector(const SimpleVector& other) {
        RethrowItemsFromIterable(other, other.GetSize());
    }

    SimpleVector(SimpleVector&& other) {       
        ArrayPtr <Type> temp (other.GetSize());
        std::move(other.begin() , other.end(), &temp[0]);
        items_.swap(temp);
        size_ = other.GetSize();
        capacity_ = other.GetCapacity();
        other.Clear();
    }

    SimpleVector& operator=(const SimpleVector& other) {
        RethrowItemsFromIterable(other, other.GetSize());
        return *this;
    }

    void PushBack(Type&& item) {
        if (size_ < capacity_) {
            items_[size_]=std::move(item);
            size_++;
        }
        else {
            size_t new_capacity = !IsEmpty() ? capacity_ * 2 : 1;
            RethrowItems(new_capacity);
            items_[size_]=std::move(item);
            size_++;
            capacity_ = new_capacity; 
        }
    }

    void PopBack() noexcept {
        if(!IsEmpty()) {
            size_--;
        }
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0 ? true : false;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    Type& At(size_t index) {
        return index < size_ ? items_[index] : throw std::out_of_range("out_of_range");
    }

    const Type& At(size_t index) const {
        return index < size_ ? items_[index] : throw std::out_of_range("out_of_range");
    }

    void Clear() noexcept {
       size_ = 0;
    }

    void Resize (size_t new_size) {
        if (new_size <= size_) {
            size_=new_size;
        }
        else {
            RethrowItems(new_size);
            size_ = new_size;
            capacity_ = new_size;
        }
    }
    
    void swap(SimpleVector& other) noexcept { 
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    Iterator Insert(Iterator pos, Type&& value) {
        assert(pos >= begin() && pos <= end());
        if(IsEmpty()) {
           PushBack(std::move(value));
           return begin();
        }
        ArrayPtr <Type> temp (2*GetSize());
        const size_t delta = pos-begin();
        std::move(begin(), pos, &temp[0]);
        temp[pos-begin()] = std::move(value);
        std::move_backward(pos, end(), &temp[GetSize()+1]); 
        items_.swap(temp);
        size_++;
        capacity_*=2;
        return begin()+delta;
    }
        
    Iterator Erase(Iterator pos) {
        assert(pos >= begin() && pos < end());
        ArrayPtr <Type> temp (GetSize()-1);
        const size_t delta = pos-begin();
        std::move(begin(), pos, &temp[0]);
        std::move_backward(pos+1, end(), &temp[GetSize()-1]);
        items_.swap(temp);
        size_--;
        return begin()+delta;
    }

    Iterator begin() noexcept {
        return  &items_[0];
    }

    Iterator end() noexcept {       
        return  &items_[size_];
    }

    ConstIterator begin() const noexcept {
       return  &items_[0];
    }

    ConstIterator end() const noexcept {
        return  &items_[size_];
    }

    ConstIterator cbegin() const noexcept {
        return  &items_[0];
    }

    ConstIterator cend() const noexcept {
       return  &items_[size_];
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            RethrowItems(new_capacity);
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr <Type> items_;
    size_t size_= 0;
    size_t capacity_= 0;
    
    void RethrowItems(size_t space)
    {
        ArrayPtr <Type> temp (space);
        std::move(begin(), end(), &temp[0]);
        items_.swap(temp);
    }
    
    template <typename Iterable>
    void RethrowItemsFromIterable(const Iterable& other, size_t space)
    {
        ArrayPtr <Type> temp (space);
        std::move(other.begin() , other.end(), &temp[0]);
        items_.swap(temp);   
        size_= space;
        capacity_= space;
    }  
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() != rhs.GetSize()) ? false : ((lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare (lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), 
        [](const Type& a, const Type& b){ return a < b; });
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {  
    return std::lexicographical_compare (lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), 
        [](const Type& a, const Type& b){ return a > b; });
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs || lhs == rhs) ? true : false;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs > rhs || lhs == rhs) ? true : false;
}