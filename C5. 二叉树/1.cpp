#include <iostream>
#include "bintree.hpp"
using std::cin;
using std::cout;
using std::endl;



int main(void)
{
  BinNode<int> skt(1),drx(2),dwg(3);

  BinTree<int> lck,lck2,lck3;
  BinTree<int> *p1,*p2;
  
  lck.setRoot(&drx);
  lck2.setRoot(&skt);
  lck3.setRoot(&dwg);

  p1 = &lck2;
  p2 = &lck3;

/*  lck.insert(1);
  lck2.insert(2);
  lck3.insert(3);
*/
  lck.attach(p2, lck.root());
  lck.attach(lck.root(), p1);

  cout << lck.root()->data() <<endl;
  cout << lck.root()->lChild()->data() <<endl;
  cout << lck.root()->rChild()->data() <<endl;

  cout << lck.remove(lck.root()->lChild()) <<endl;
  
  cout << lck.size() <<endl;

  return 0;
  
}
