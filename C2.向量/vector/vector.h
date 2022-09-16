#ifndef VECTOR_H
#define VECTOR_H

using Rank = int; //秩
#define DEFAULT_CAPACITY  3 //默认的初始容量（实际应用中可设置为更大）

template <typename T> 
class Vector { //向量模板类

 protected:
    /*三个成员对象*/
    Rank _size; //规模(逻辑存储空间)
    Rank _capacity; //容量(物理存储空间)
    T* _elem; //数据区

    void copyFrom(T const* A, Rank low, Rank high); //复制数组区间A[low, high)
    void expand(); //空间不足时扩容
    void shrink(); //装填因子过小时压缩
    bool bubble(Rank low, Rank high); //扫描交换
    void bubbleSort(Rank low, Rank high); //起泡排序算法
    Rank maxItem(Rank low, Rank high); //选取最大元素
    void selectionSort(Rank low, Rank high); //选择排序算法
    void merge(Rank low, Rank mi, Rank high); //归并算法
    void mergeSort(Rank low, Rank high); //归并排序算法
    void heapSort(Rank low, Rank high); //堆排序（稍后结合完全堆讲解）
    Rank partition(Rank low, Rank high); //轴点构造算法
    void quickSort(Rank low, Rank high); //快速排序算法
    void shellSort(Rank low, Rank high); //希尔排序算法

 public:
    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, Rank s = 0, T v = 0) //容量为c、规模为s、所有元素初始为v
    {
        _elem = new T[_capacity = c]; for (_size = 0; _size < s; _elem[_size++] = v);
    } //s<=c
    Vector(T const* A, Rank n) { copyFrom(A, 0, n); } //数组整体复制
    Vector(T const* A, Rank low, Rank high) { copyFrom(A, low, high); } //区间
    Vector(Vector<T> const& V) { copyFrom(V._elem, 0, V._size); } //向量整体复制
    Vector(Vector<T> const& V, Rank low, Rank high) { copyFrom(V._elem, low, high); } //区间

 // 析构函数
    ~Vector() { delete[] _elem; } //释放内部空间

// 只读访问接口
    Rank size() const { return _size; } //规模    
    bool empty() const { return !_size; } //判空
    Rank find(T const& e) const { return find(e, 0, _size); } //无序向量整体查找
    Rank find(T const& e, Rank low, Rank high) const; //无序向量区间查找
    Rank search(T const& e) const { return (0 >= _size) ? -1 : search(e, 0, _size); }//有序向量整体查找
    Rank search(T const& e, Rank low, Rank high) const; //有序向量区间查找
    Rank disordered() const; //逆序对数计算

 // 可写访问接口
    T& operator[] (Rank r); //重载下标操作符，可以类似于数组形式引用各元素
    const T& operator[] (Rank r) const; //仅限于做右值的重载版本
    Vector<T>& operator= (Vector<T> const&); //重载赋值操作符，以便直接克隆向量
    T remove(Rank r); //删除秩为r的元素
    int remove(Rank low, Rank high); //删除秩在区间[low, high)之内的元素
    Rank insert(Rank r, T const& e); //插入元素
    Rank insert(T const& e) { return insert(_size, e); } //默认作为末元素插入
    void sort(Rank low, Rank high); //对[low, high)排序
    void sort() { sort(0, _size); } //整体排序
    void unsort(Rank low, Rank high); //对[low, high)置乱
    void unsort() { unsort(0, _size); } //整体置乱
    Rank deduplicate(); //无序去重
    Rank uniquify(); //有序去重

 // 遍历
    void traverse(void (*) (T&)); //遍历（使用函数指针，只读或局部性修改）
    template <typename VST> void traverse(VST); //遍历（使用函数对象，可全局性修改）临时变量是常量引用，无法使用引用初始化
}; //Vector

//辅助类
template <typename T> 
class myPrint { public: virtual void operator()(T &e) { std::cout << e <<" ";}};//类外类函数对象，单个打印：通过重载操作符()实现

template <typename T> 
class CheckOrder { //函数对象：判断一个T类对象是否局部有序
 public:
  T pre; 
  Rank &u; //此处声明了类型成员，定义需要在构造函数生成的过程中进行，并且这里必须有自定义构造函数，而不能依赖生成构造函数
  
  CheckOrder(Rank& unsorted, T& first) : pre(first),u(unsorted) {} //类构造函数，初始化引用型对象u;
  virtual void operator()(T& e) { if (pre > e) ++u; pre = e;} //重载函数：找寻逆序对
};

class Fib { //Fib计算类
 private:
  Rank f, g;

 public:
  Fib(Rank n) { //初始化为不小于n的Fibonacci项
    f = 1, g = 0; //fib(0),fib(-1)
    while (g < n) { //O(log_phi(n))
      next();
    }
  }
  Rank get() { return g; } //获得当前项Fibonacci项 O(1)
  Rank next() { f = f + g; g = f - g; return g;} //转向下一项Fibonacci项 O(1)
  Rank prev() { g = f - g; f = f - g; return g;} //转向前一项Fibonacci项 O(1)
};

//类外辅助函数声明
inline Rank myMax(Rank a, Rank b) { return (a > b) ? a : b; } //内联一个比较函数
template <typename T> void print(Vector<T> &v);//向量遍历打印
template <typename T> void checkOrder(Vector<T> &v);//向量有序性判定
/*二者同架构原理*/
template <typename T> static Rank binSearch(T* S, T const& e, Rank low, Rank high); //二分查找A
template <typename T> static Rank fibSearch(T* S, T const& e, Rank low, Rank high); //Fibonacci查找
/*二者实现了减少转向分支优化*/
template <typename T> static Rank binSearchB(T* S, T const& e, Rank low, Rank high); //二分查找B
template <typename T> static Rank binSearchC(T* S, T const& e, Rank low, Rank high); //二分查找C————语义，性能，功能最强优化
/*实现大规模快速收敛*/
template <typename T> static Rank insertSearch(T* S, T const& e, Rank low, Rank high); //插值查找————小规模局部有病态


/***********************************************************实现部分(模板类不支持分离式编译)*************************************************************/


#endif