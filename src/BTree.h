#ifndef MINISQL_SRC_B_TREE_H_
#define MINISQL_SRC_B_TREE_H_

#include "TreeNode.h"
#include <cstddef>
#include <queue>
using namespace std;

namespace minisql {

// The B-Tree data structure
template<typename T>
class BTree {
 public:
  BTree(int m): m_(m), height_(0), root_(0) {
    root_ = new TreeNode<T>(m_);
  }
  ~BTree() { // TODO Delete the whole tree
  }

 public:
  int m() const { return m_; }
  const int height() const { return height_; }
  const TreeNode<T>* root() const { return root_; }
  TreeNode<T>* root() { return root_; }

 public:
  // TODO Implement B-Tree operations
  bool Insert(T key) {
    root_ = root_->Insert(key);
    return true;
  }

  bool Insert(int num, T* key) {
    for (int i = 0; i != num; ++i)
      root_ = root_->Insert(key[i]);
    return true;
  }

  bool Delete(T key) {}
  T Search(T key) {}

  void Print(void (TreeNode<T>::*printer)() const) {
    queue<const TreeNode<T>*> print_queue;
    print_queue.push(root_);
    const TreeNode<T>* temp_node;
    cout << endl;
    while (0 != print_queue.size()) {
      temp_node = print_queue.front();
      if (0 == temp_node->subnodes())  // A leaf node
        (temp_node->*printer)();
      else
        for (int i = 0 ;i != temp_node->size() + 1; ++i)
          print_queue.push(temp_node->subnode(i));
      print_queue.pop();
    }
    cout << "\n" << endl;
    return;
  }
  
 private:
  const int m_;
  int height_;
  TreeNode<T>* root_;

 private:
  BTree(const BTree& other);
  void operator=(const BTree& other);
};

}  // (END) Namespace minisql

#endif
