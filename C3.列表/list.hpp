#ifndef LIST_H
#define LIST_H

#include "listnode.hpp" //引入列表节点类
using std::cout,std::endl;

template <typename T>
class List { //列表模板类
 private:
   int _size;              //规模
   ListNodePosi<T> header; //头哨兵
   ListNodePosi<T> tailer; //尾哨兵（哨兵均不在列表的可见数据壳内）

 protected:
   void init();                          //列表创建时的初始化，需要统一调用
   int clear();                          //清除所有节点
   void copyNodes(ListNodePosi<T>, int); //复制列表中自位置p起的n项
   ListNodePosi<T> merge(ListNodePosi<T>, int, List<T> &, ListNodePosi<T>, int); //归并
   void mergeSort(ListNodePosi<T> &, int);   //对从p开始连续的n个节点归并排序
   void selectionSort(ListNodePosi<T>, int); //对从p开始连续的n个节点选择排序
   void insertionSort(ListNodePosi<T>, int); //对从p开始连续的n个节点插入排序
   void radixSort(ListNodePosi<T>, int);     //对从p开始连续的n个节点基数排序

 public:
   // 构造函数
   List() { init(); }                                         //默认
   List(List<T> const &L) { copyNodes(L.first(), L.size()); } //整体复制列表L
   List(List<T> const &L, Rank r, int n)                      //复制列表L中自第r项起的n项
   { ListNodePosi<T> p = L.first(); Rank k = 0; while (k++ < r) p = p->succ(); copyNodes(p, n); } //header最好不要在外暴露，仅在特殊情况下使用，考虑清楚边界情况啊                         
   List(ListNodePosi<T> p, int n) { copyNodes(p, n); }        //复制列表中自位置p起的n项

   // 析构函数
   ~List() { clear(); delete header; delete tailer; } //释放（包含头、尾哨兵在内的）所有节点

   // 只读访问接口
   Rank size() const { return _size; }                                         //规模
   bool empty() const { return _size <= 0; }                                   //判空
   T &operator[](Rank r) const;                                                //重载，支持循秩访问（效率低）
   ListNodePosi<T> first() const { return header->succ(); }                    //首节点位置
   ListNodePosi<T> last() const { return tailer->pred(); }                     //末节点位置
   bool valid(ListNodePosi<T> p)                                               //判断位置p是否对外合法
   { return p && (tailer != p) && (header != p); }                             //将头、尾节点等同于nullptr
   ListNodePosi<T> find(T const &e) const { return find(e, _size, tailer); }   //无序列表查找
   ListNodePosi<T> find(T const &e, int n, ListNodePosi<T> p) const;           //无序区间向前查找
   ListNodePosi<T> find(T const &e, ListNodePosi<T> p, int n) const;           //无序区间向后查找
   ListNodePosi<T> search(T const &e) const { return find(e, _size, tailer); } //有序列表查找
   ListNodePosi<T> search(T const &e, int n, ListNodePosi<T> p) const;         //有序区间查找
   ListNodePosi<T> selectMax(ListNodePosi<T> p, int n);                        //在p及其n-1个后继中选出最大者
   ListNodePosi<T> selectMax() { return selectMax(header->succ(), _size); }    //整体最大者

   // 可写访问接口
   ListNodePosi<T> insertAsFirst(T const &e);             //将e当作首节点插入
   ListNodePosi<T> insertAsLast(T const &e);              //将e当作末节点插入
   ListNodePosi<T> insert(ListNodePosi<T> p, T const &e); //将e当作p的后继插入
   ListNodePosi<T> insert(T const &e, ListNodePosi<T> p); //将e当作p的前驱插入
   T remove(ListNodePosi<T> p);                           //删除合法位置p处的节点,返回被删除节点
   void merge(List<T> &L) { merge(header->succ(), _size, L, L.header->succ(), L._size); } //全列表归并
   void sort(ListNodePosi<T> p, int n);  //列表区间排序
   void sort() { sort(first(), _size); } //列表整体排序
   int deduplicate();                    //无序去重
   int uniquify();                       //有序去重
   void reverse();                       //前后倒置（习题）

   // 遍历
   void traverse(void (*)(T &)); //遍历，依次实施visit操作（函数指针，只读或局部性修改）
   template <typename VST>       //操作器
   void traverse(VST);           //遍历，依次实施visit操作（函数对象，可全局性修改,不能使用引用形式，因为会面临temp常量赋值现象
};// List

//辅助函数
template <typename T> void print(ListNodePosi<T> const&);
//辅助类
template <typename T>
class myPrint {
public:
  virtual void operator()(T &e) {
    cout << e << " ";
  }
};


//****************************************************************实现部分*********************************************************************//


/*1.重载下标操作符，循秩访问（低效，仅作为左值访问）*/
template <typename T>
T& List<T>::operator[](Rank r) const { // O(n)
  ListNodePosi<T> temp = first();
  for (Rank i = 0; i < r; ++i) {
    temp = temp->succ();
  }

  return temp->data();
}

/*2.列表初始化*/
template <typename T>
void List<T>::init() { //初始化，创建列表对象时统一调用
  header = new ListNode<T>;
  tailer = new ListNode<T>; //创建头节点哨兵
  _size = 0;                //初始化规模

  header->succ() = tailer;
  header->pred() = nullptr; //互联
  tailer->pred() = header;
  tailer->succ() = nullptr; //封装写法header->succ() = tailer; header->pred() = nullptr;
}

/*3.插入函数*/
//向前插入
template <typename T>
ListNodePosi<T> List<T>::insert(T const &e, ListNodePosi<T> p) { //O(1)
  ++_size;
  return p->insertAsPred(e); //链接已调整好，因为双向链表能向前回溯的缘故，因此不用利用快慢指针来实现了
}
//向后插入————————————插入的时候就已经扩充容量了，因此这里都是在基本操作上封装好的，不用在进行其他调用时扩容了，否则会冗余，导致容量不匹配*********************
template <typename T>
ListNodePosi<T> List<T>::insert( ListNodePosi<T> p, T const &e) { //O(1)
  ++_size;
  return p->insertAsSucc(e); //链接已调整好，因为双向链表能向前回溯的缘故，因此不用利用快慢指针来实现了
}
//头插法
template <typename T>
ListNodePosi<T> List<T>::insertAsFirst(T const &e) { //O(1)
  return insert(header, e);
}
//尾插法
template <typename T>
ListNodePosi<T> List<T>::insertAsLast(T const &e) { //O(1)
  return insert(e, tailer);
}

//列表复制函数（构造调用）
// copy函数的抽象性＋封装性进一步体现
template <typename T>
void List<T>::copyNodes(ListNodePosi<T> p, int n) { // O(n)
  init();                                           //隐性传入了this指针

  while (_size < n) {        // while (_size++ < n) {
    insertAsLast(p->data()); // insertAsLast(p->data()); *****************插入操作已经扩容啦，条件里就别再乱来啦
    p = p->succ();           // p = p->succ();
  }                          //}
}

/*4.列表节点删除*/
//删除函数
template <typename T>
T List<T>::remove(ListNodePosi<T> p) { //O(1)
  T e = p->data();
  p->succ()->pred() = p->pred();
  p->pred()->succ() = p->succ();

  delete p; //写入nullptr操作在规模较大时容易影响效率，一般在申请空间前做判断或赋初值即可
  --_size;  //规模缩减调整
  return e;
}

//列表节点清空（析构调用）
template <typename T>
int List<T>::clear() { // O(n)
  int oldSize = _size;
  while (_size > 0) {    //<=> while(_size > 0)
    remove(header->succ()); //与之前单向链表无_size的思路不同，并且此处的遍历不用主动迭代，链接会自动调整
  }
  return oldSize; //返回被清空的链表规模
}

/*5.无序列表查找*/
//前向查找
template <typename T>
ListNodePosi<T> List<T>::find(T const &e, int n, ListNodePosi<T> p) const { //O(n)
  while (n-- > 0) { 
    if (e == p->pred()->data()) { //找到
      return p->pred();
    } else
      p = p->pred(); //前向递增
  }
  return nullptr; //查找失败
}

//后向查找(若重复则返回秩最小者)
template <typename T>
ListNodePosi<T> List<T>::find(T const &e, ListNodePosi<T> p, int n) const { //O(n)
  while (n-- > 0) { 
    if (e == p->succ()->data) { //找到
      return p->succ();
    } else
      p = p->succ(); //后向递增
  }
  return nullptr; //查找失败
}

/*6.无序列表去重（调用前向查找，采用分治思想）*/
template <typename T>
int List<T>::deduplicate() { // O(n^2)
  int oldSize = _size;
  Rank r = 0;
  for (ListNodePosi<T> p = first(); p != tailer; p = p->succ()) { // O(n)
    ListNodePosi<T> q = find(p->data(), r, p);                      // O(n)
    q ? remove(q) : ++r;
  }
  return oldSize - _size; //返回删除的重复元素数量
}

/*7.链表遍历*/
template <typename T>
template <typename VST>
void List<T>::traverse(VST visit) {
  for (ListNodePosi<T> temp = first(); temp != tailer; temp = temp->succ())
    visit(temp->data());
  return;
}

//函数对象实现遍历打印*****************************************理解如何调用的
template <typename T>
void print(List<T> &L) {
  L.traverse(myPrint<T>()); //相当于传入了一种操作
  cout << endl;
}

#endif
