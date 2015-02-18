#ifndef MINISQL_SRC_B_TREE_H_
#define MINISQL_SRC_B_TREE_H_

#include "TreeNode.h"
#include <cassert>
#include <cstddef>
#include <queue>

namespace minisql {

// The B-Tree data structure
template<typename T>
class BTree {
 public:
  BTree(int m): m_(m), root_(0) {
    root_ = new TreeNode<T>(m_);
  }
  ~BTree() {  // Destroy the whole tree
    root_->Destroy();
    delete root_;
    root_ = 0;
  }

 public:
  int m() const { return m_; }
  const TreeNode<T>* root() const { return root_; }
  TreeNode<T>* root() { return root_; }

 public:
  // Implement B-Tree operations
  bool Insert(const T& key) {
    root_ = root_->Insert(key);
    return true;
  }

  bool Insert(int num, T* key) {
    for (int i = 0; i != num; ++i)
      root_ = root_->Insert(key[i]);
    return true;
  }

  bool Delete(const T& key) {
    TreeNode<T>* temp_node = root_->Delete(key);
    if (0 == temp_node)        // Key not found
      return false;
    
    if (0 == root_->size()) {  // Shallow the tree
      if (0 == root_->subnodes()) {
        std::cout << "Empty tree!" << std::endl;
      } else {
        root_ = root_->subnode(0);
        delete root_->parent();
        root_->set_parent(0);
      }
      return true;
    } else {
      return true;
    }
  }

  bool Update(const T& old_key, const T& new_key) {
    if (false == Delete(old_key)) return false;
    if (false == Insert(new_key)) return false;
    return true;
  }

  const T* Search(const T& key) {
    return root_->Search(key);
  }

  const T* Retrieve(const T& key) {
    return root_->Retrieve(key);
  }

  void Print(void (TreeNode<T>::*printer)() const) {
    if (0 == root_->size() && 0 == root_->subnodes()) {
      std::cout << "Empty tree!" << std::endl;
      return;
    }
    
    std::queue<const TreeNode<T>*> print_queue;
    print_queue.push(root_);
    print_queue.push(0);
    const TreeNode<T>* temp_node;
    std::cout << std::endl;
    while (0 != print_queue.size()) {
      temp_node = print_queue.front();
      if (0 == temp_node) {
        std::cout << "\n" << std::endl;
        if (print_queue.size() > 1)
          print_queue.push(0);
      } else {
        (temp_node->*printer)();
        if (0 != temp_node->subnodes())  // Non-leaf node
          for (int i = 0 ;i != temp_node->size() + 1; ++i)
            print_queue.push(temp_node->subnode(i));
      }
      print_queue.pop();
    }
    return;
  }
  
 private:
  const int m_;
  TreeNode<T>* root_;

 private:
  BTree(const BTree& other);
  void operator=(const BTree& other);
};

}  // (END) Namespace minisql

#endif
