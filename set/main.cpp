#include <bits/stdc++.h>
#include "set.h"

int main(void) {
  set<int> setik;

  setik.insert(3);
  setik.insert(2);
  setik.insert(4);
  setik.insert(1);

  auto s2 = setik;

  for (auto i : s2) {
    std::cout << i << ' ';
  }
  std::cout << std::endl;
  return 0;
}