#ifndef BINNODE_H
#define BINNODE_H


enum RBColor { RB_RED,
               RB_BLACK }; //红黑树节点颜色

template <typename T> class BinNode;                    //预先声明类
template <typename T> using BinNodePosi = BinNode<T> *; //节点位置
template <typename T> class BinTree;                    //预先声明友邻类，便于后续访问

//辅助函数
template <typename T> int getNodeSize(BinNodePosi<T> node); //计算节点后代数


template <typename T>
class BinNode { //二叉树节点模板类

 public: //protected:                 //成员------使用public便于实现，少写许多接口，留待未来封装优化****

   BinNodePosi<T> pt;       //父节点
   BinNodePosi<T> lc, rc;   //左、右节点
   T elem;                  //数值
   int ht;                  //高度(通用)
   int npl;                 // Null Path Length(左氏堆，也可以用height直接替代)
   RBColor color;           //颜色(红黑树)
  
 public:
  //构造函数
   BinNode() : 
    pt(nullptr), lc(nullptr), rc(nullptr), ht(0), npl(1), color(RB_RED) {}   //默认构造函数
   BinNode(T e, BinNodePosi<T> p = nullptr, BinNodePosi<T> l = nullptr, BinNodePosi<T> r = nullptr, int h = 0, int n = 1, RBColor c = RB_RED) : 
    elem(e), pt(p), lc(l), rc(r), ht(h), npl(n), color(c) {} //带参data构造函数

   //操作接口
   //int size();                         //统计当前节点后代数，即其根子树规模（非空节点方能调用，空节点默认size为0）
   BinNodePosi<T> insertAsLC(const T &e); //将元素e当作当前节点的左孩子插入为新节点
   BinNodePosi<T> insertAsRC(const T &e); //将元素e当作当前节点的右孩子插入为新节点
   BinNodePosi<T> succ();                 //（中序遍历意义下）当前节点的直接后继节点

   //只读接口
   T data() const { return elem; }              //获得当前节点所存数据对象
   BinNodePosi<T> parent() const { return pt; } //获得当前节点的父节点位置
   BinNodePosi<T> lChild() const { return lc; } //获得当前节点的左子节点位置
   BinNodePosi<T> rChild() const { return rc; } //获得当前节点的右子节点位置
   int height() const { return ht; }            //获得当前节点高度
   int getNpl() const { return npl; }           //获得当前节点左氏堆值
   RBColor getColor() const { return color; }   //获得当前节点的颜色

   template <typename VST> void travLevel(VST &); //子树层次遍历
   template <typename VST> void travPre(VST &);   //子树先序遍历
   template <typename VST> void travIn(VST &);    //子树中序遍历
   template <typename VST> void travPost(VST &);  //子树后序遍历

   //比较器、判等器（操作符重载）
   bool operator<(const BinNode &bn) { return elem < bn.elem; }   //重载小于号
   bool operator>(const BinNode &bn) { return elem > bn.elem; }   //重载大于号
   bool operator==(const BinNode &bn) { return elem == bn.elem; } //判等重载
   bool operator!=(const BinNode &bn) { return elem != bn.elem; } //不等重载
   bool operator>=(const BinNode &bn) { return elem >= bn.elem; } //重载大于等于号
   bool operator<=(const BinNode &bn) { return elem <= bn.elem; } //重载小于等于号

   //内联BinNode节点状态函数
   bool IsRoot(BinNodePosi<T> x) { return !(x->pt); }                           //是否根节点
   bool IsLChild(BinNodePosi<T> x) { return (!IsRoot(x) && (x == x->pt->lc)); } //是否左子节点
   bool IsRChild(BinNodePosi<T> x) { return (!IsRoot(x) && (x == x->pt->rc)); } //是否右子节点
   bool HasParent(BinNodePosi<T> x) { return x->pt; }                           //有父节点
   bool HasLChild(BinNodePosi<T> x) { return x->lc; }                           //有左子节点
   bool HasRChild(BinNodePosi<T> x) { return x->rc; }                           //有右子节点
   bool HasChild(BinNodePosi<T> x) { return HasLChild(x) || HasRChild(x); }     //是否有孩子
   bool HasBothChild(BinNodePosi<T> x) { return HasLChild(x) && HasRChild(x); } //是否同时拥有两个孩子
   bool IsLeaf(BinNodePosi<T> x) { return !HasChild(x); }                       //是否叶节点

   //内联BinNode关系函数获取
   BinNodePosi<T> &FromParentTo(BinNodePosi<T> x) { return (IsRoot(x) ? x : (IsLChild(x) ? x->pt->lc : x->pt->rc)); } //获取对父节点的相对位置
   BinNodePosi<T> &sibling(BinNodePosi<T> x) { return (IsLChild(x) ? x->pt->rc : x->pt->lc); }                        //获取兄弟节点的指针
   BinNodePosi<T> &uncle(BinNodePosi<T> x) { return sibling(x->pt); }                                                 //获取叔叔节点的指针
};


//****************************************************************************************************************************
//****************************************************************************************************************************
//********************************************************* 实 ***************************************************************
//********************************************************* 现 ***************************************************************
//********************************************************* 部 ***************************************************************
//********************************************************* 分 ***************************************************************
//****************************************************************************************************************************
//****************************************************************************************************************************



////辅助函数
//递归实现后代节点总数求解
template <typename T>
int getNodeSize(BinNodePosi<T> node) { // O(n) = O(|size|)
  if (node == nullptr) return 0; //考虑边界奇异情况：空节点
  return getNodeSize(node->lChild()) + getNodeSize(node->rChild()) + 1;
}


////成员函数
template <typename T>
BinNodePosi<T> BinNode<T>::insertAsLC(const T &e) {
  return lc = new BinNode(e, this); //调用构造函数
} //O(1)

template <typename T>
BinNodePosi<T> BinNode<T>::insertAsRC(const T &e) {
  return rc = new BinNode(e, this); //调用构造函数实现子链接父
} //O(1)

/*
//递归实现后代节点总数求解（较慢）
template <typename T> 
int BinNode<T>:: size() { //O(n) = O(|size|)

  int s = 1; //计算自身
  if (lc != nullptr)  s += lc->size(); //递归计入左子树规模
  if (rc != nullptr)  s += rc->size(); //递归计入右子树规模

  return s;
}
*/

#endif