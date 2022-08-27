#include <iostream>

using namespace std;

typedef int Positive;

//数组求和：二分递归法
int mySum(int A[], Positive low, Positive high)
{
  if(high - low < 2) //考虑了奇数和偶数规模情况
    return A[low];//高位high无法返回
  
  int mid = (high + low) >> 1;//位操作更快
  return mySum(A, low, mid) + mySum(A, mid, high);//高位high无法求到，考虑边界条件应取n，这样就能求0——n-1的和;
}

int main(void)
{
  int A[] = {1,2,3,4,5};
  int B[] = {1,2,3,4};

  cout<<mySum(A, 0, sizeof(A)/sizeof(int))<<endl;
  cout<<mySum(B, 0, sizeof(B)/sizeof(int))<<endl;

  return 0;
}