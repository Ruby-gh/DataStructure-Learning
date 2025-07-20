import ctypes

class DynamicArray:
    """一个从零实现的动态数组"""

    def __init__(self):
        """初始化动态数组"""
        self._n = 0  # 数组中实际元素的数量 (size)
        self._capacity = 1  # 底层数组的容量 (capacity)
        self._A = self._make_array(self._capacity)  # 底层的静态数组

    def __len__(self):
        """返回数组中元素的数量"""
        return self._n

    def __getitem__(self, k):
        """通过索引 k 获取元素"""
        # 检查索引是否有效
        if not 0 <= k < self._n:
            raise IndexError('invalid index')
        return self._A[k]

    def __setitem__(self, k, value):
        """通过索引 k 设置元素的值"""
        if not 0 <= k < self._n:
            raise IndexError('invalid index')
        self._A[k] = value
        
    def append(self, obj):
        """在数组末尾添加一个元素"""
        # 如果容量不足，进行扩容
        if self._n == self._capacity:
            # 扩容为当前容量的2倍
            self._resize(2 * self._capacity)
        
        # 在末尾添加新元素
        self._A[self._n] = obj
        self._n += 1

    def _resize(self, new_capacity):
        """内部方法：将数组扩容至 new_capacity"""
        print(f"Resizing from {self._capacity} to {new_capacity}...") # 打印日志，方便观察
        B = self._make_array(new_capacity)  # 1. 创建一个更大的新数组
        
        for i in range(self._n):  # 2. 将旧数组的元素复制到新数组
            B[i] = self._A[i]
            
        self._A = B  # 3. 用新数组替换旧数组
        self._capacity = new_capacity  # 4. 更新容量

    def _make_array(self, c):
        """内部方法：创建一个容量为 c 的底层静态数组"""
        # 使用 ctypes 创建一个类似 C 语言的底层数组
        return (c * ctypes.py_object)()

    def insert(self, k, value):
        """在索引 k 处插入一个值"""
        if not 0 <= k <= self._n:
            raise IndexError('invalid index for insertion')

        # 如果容量不足，先扩容
        if self._n == self._capacity:
            self._resize(2 * self._capacity)
            
        # 将索引 k 及之后的所有元素向右移动一位
        for j in range(self._n, k, -1):
            self._A[j] = self._A[j-1]
            
        # 在索引 k 处放置新值
        self._A[k] = value
        self._n += 1

    def pop(self):
        """删除并返回数组末尾的元素"""
        if self._n == 0:
            raise IndexError("pop from empty array")

        # （可选）缩容：当元素数量过少时（如小于容量的1/4），可以缩小数组以节省空间
        # 避免在元素数量为容量的1/2时就缩容，以防止在边界情况下的频繁扩容/缩容（抖动）
        if self._n < self._capacity // 4:
            self._resize(self._capacity // 2)

        obj = self._A[self._n - 1]
        self._A[self._n - 1] = None # 帮助垃圾回收
        self._n -= 1
        return obj

    def __str__(self):
        """方便打印数组内容"""
        return '[' + ', '.join(str(self._A[i]) for i in range(self._n)) + ']'