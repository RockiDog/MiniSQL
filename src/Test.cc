#include "BTree.h"
using namespace minisql;
using namespace std;

int main(int argc, char** argv) {
  BTree<float> tree(4);
  float key[] {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 6.5, 18.5, 17.5, 12, 6.6, 6.7, 6.8, 6.9};
  tree.Insert(sizeof(key)/sizeof(float), key);
  tree.Print(&TreeNode<float>::Print);
  return 0;
}
