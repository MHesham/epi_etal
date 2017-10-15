#ifndef __MEMORY_MGMT_TESTS_H__
#define __MEMORY_MGMT_TESTS_H__

#include "memory_mgmt.h"

void memory_pool_test()
{
  auto n3 = new list_node<int>(3);
  auto n2 = new list_node<int>(2);
  auto n1 = new list_node<int>(1);

  n2->next = n3;
  n1->next = n2;

  delete n3;
  delete n2;
  delete n1;
}

void SmartPtrTest()
{
  int *x = new int(5);
  {
    SmartPtr<int> sp(x);
  }
}

#endif
