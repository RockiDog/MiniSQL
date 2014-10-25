#ifndef MINISQL_SRC_B_TREE_H_
#define MINISQL_SRC_B_TREE_H_

#include "TreeNode.h"
#include <cstddef>
using namespace std;

namespace minisql {
  // The B-Tree data structure
  template<typename T>
  class BTree {
   public:
    BTree(int m): m_(m), height_(0), root_(0) {
      root_ = new TreeNode<T>(m_);
    }
    ~BTree() {
      delete root_;
    }

   public:
    int m() const { return m_; }
    const int height() const { return height_; }
    const TreeNode<T>* root() const { return root; }
    TreeNode<T>* root() { return root; }

   public:
    // TODO Implementing B-Tree operations
    bool Insert(T key) {
      return root_->Insert(key);
    }
    bool Delete(T key) {}
    T Search(T key) {}
    void Print() {}

   private:
    void Split() {}
    void Traverse() {}
    
   private:
    const int m_;
    int height_;
    TreeNode<T>* root_;

   private:
    BTree(const BTree& other);
    void operator=(const BTree& other);
  };
}

#endif
