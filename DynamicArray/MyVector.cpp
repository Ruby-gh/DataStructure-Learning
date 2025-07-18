#include <iostream>
#include <stdexcept> // 用于 std::out_of_range
#include <utility>   // 用于 std::move
#include <algorithm> // 用于 std::copy

template <typename T>
class MyVector {
private:
    T* data_;
    size_t size_;
    size_t capacity_;
public:
    // 1. 构造函数和析构函数
    
    // 默认构造函数
    MyVector() : data_(nullptr), size_(0), capacity_(0) {}

    // 带初始容量的构造函数
    explicit MyVector(size_t initial_capacity) {
        if (initial_capacity == 0) {
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        } else {
            data_ = new T[initial_capacity];
            size_ = 0;
            capacity_ = initial_capacity;
        }
    }

    // 析构函数：释放内存
    ~MyVector() {
        delete[] data_;
        // C++中，良好的习惯是将指针置空，尽管对象即将销毁
        data_ = nullptr;
    }

    // 2. 拷贝控制 (Rule of Five)

    // 拷贝构造函数 (深拷贝)
    MyVector(const MyVector& other) {
        std::cout << "Copy constructor called.\n";
        // 分配自己的内存
        data_ = new T[other.capacity_];
        // 复制数据
        std::copy(other.data_, other.data_ + other.size_, data_);
        size_ = other.size_;
        capacity_ = other.capacity_;
    }

    // 拷贝赋值运算符
    MyVector& operator=(const MyVector& other) {
        std::cout << "Copy assignment operator called.\n";
        if (this == &other) { // 防止自赋值
            return *this;
        }
        
        // 释放旧资源
        delete[] data_;

        // 分配新资源并复制
        data_ = new T[other.capacity_];
        std::copy(other.data_, other.data_ + other.size_, data_);
        size_ = other.size_;
        capacity_ = other.capacity_;

        return *this;
    }

    // 移动构造函数 (C++11)
    MyVector(MyVector&& other) noexcept { // noexcept很重要，能让标准库进行更多优化
        std::cout << "Move constructor called.\n";
        // "偷"走 other 的资源
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        // 将 other 置于一个有效的、可析构的状态
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // 移动赋值运算符 (C++11)
    MyVector& operator=(MyVector&& other) noexcept {
        std::cout << "Move assignment operator called.\n";
        if (this == &other) { // 防止自移动
            return *this;
        }
        
        // 释放自己的旧资源
        delete[] data_;

        // "偷"走 other 的资源
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        
        // 将 other 置于有效状态
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;

        return *this;
    }


    // 3. 核心功能

    // 在末尾添加元素
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            resize(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    // 在末尾添加元素 (C++11 右值引用版本，用于移动)
    void push_back(T&& value) {
        if (size_ >= capacity_) {
            resize(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_++] = std::move(value);
    }
    
    // 删除末尾元素
    void pop_back() {
        if (size_ > 0) {
            size_--;
            // 注意：对于非POD类型，可能需要调用析构函数
            // data_[size_].~T(); 
            // 但对于基本类型和简单对象，这通常是不必要的。
            if (capacity_ > 0 && size_ == capacity_ /4) {
                resize(capacity_/2 == 0 ? 1 :capacity_ /2)
            }
        }
    }

    // 通过索引访问元素 (返回引用，可修改)
    T& operator[](size_t index) {
        // 为了方便，不进行边界检查，行为类似原生数组
        return data_[index];
    }
    
    // 通过索引访问元素 (const版本，不可修改)
    const T& operator[](size_t index) const {
        return data_[index];
    }

    // 带边界检查的访问
    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // 4. 容量相关方法
    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    bool empty() const {
        return size_ == 0;
    }

private:
    // 私有辅助方法
    void resize(size_t new_capacity) {
        if (new_capacity <= size_) {
            return;
        }
        
        std::cout << "Resizing from " << capacity_ << " to " << new_capacity << "\n";

        T* new_data = new T[new_capacity];
        
        // 关键点：移动元素而非拷贝，效率更高 (需要T支持移动)
        for(size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }
        
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
    
};

// 重载 << 运算符，方便打印 MyVector
template <typename T>
std::ostream& operator<<(std::ostream& os, const MyVector<T>& vec) {
    os << "Vector(size=" << vec.size() << ", capacity=" << vec.capacity() << ") [";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i < vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}