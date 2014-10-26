#ifndef MINISQL_SRC_TREENODE_H_
#define MINISQL_SRC_TREENODE_H_

#include <cstddef>
using namespace std;

namespace minisql {
  // The B-Tree node data structure
template<typename T>
class TreeNode {
 // Constructors&Destructors
 public:
  TreeNode(int m): m_(m), size_(0), parent_(0), next_sibling_(0) {
    keys_ = new T[m - 1]{0};
    subnodes_ = new TreeNode<T>*[m]{0};
  }
  
  ~TreeNode() {
    delete [] subnodes_;
    delete [] keys_;
    subnodes_ = 0;
    keys_ = 0;
  }
  
 // Getting&Setting functions
 public:
  int m() const { return m_; }
  int size() const {return size_; }
  const T* keys(int i) const { return keys_[i]; }
  const TreeNode<T>* subnode(int i) const { return subnodes_[i]; }
  const TreeNode<T>* parent() const { return parent_; }
  const TreeNode<T>* next_sibling() const { return next_sibling_; }

  bool set_size(int size) {
    if (size < 0 || size >= m_) {
      size_ = size;
      return true;
    }
    else {
      return false;
    }
  }

  bool set_key(int i, T* key) {
    if (i < 0 || i > size_ - 1)
      return false;
    else
      keys_[i] = key;
    return true;
  }

  bool set_subnode(int i, TreeNode<T>* subnode) {
    if (i < 0 || i > size_)
      return false;
    else
      subnodes_[i] = subnode;
    return true;
  }

  bool set_parent(TreeNode<T>* parent) {
    parent_ = parent;
    return true;
  }

  bool set_next_sibling(TreeNode<T>* next_sibling) {
    next_sibling_ = next_sibling;
    return true;
  }

 // Fundamental operations
 public:
  // TODO Update father node
  T Insert(T key) {
    if (0 == parent_) { // Leaf node
      if (size_ != m_ - 1) { // Leaf node not full, simply insert
        ++size_;
        for (int i = 0; i != size_ - 1; ++i) {
          if (key < keys_[i] || i == size_ - 1) {
            for (int j = size_ - 1; j > i; --j)
              keys_[j] = keys_[j - 1];
            keys_[i] = key;
            break;
          }
        }
        return key;
      }
      else { // Leaf node full, check next sibling
        if (0 != next_sibling_) { // Has next sibling
          if (next_sibling_->size_ != next_sibling_->m_ - 1) { // Next sibling not full
            if (key > keys_[size_ - 1]) {
              next_sibling_->Insert(key);
              return key;
            }
            else {
              next_sibling_->Insert(keys_[size_ - 1]);
              keys_[size_ - 1] = key;
              for (int i = 0; i != size_ - 1; ++i) {
                if (key < keys_[i] || i == size_ - 1) {
                  for (int j = size_ - 1; j > i; --j)
                    keys_[j] = keys_[j - 1];
                  keys_[i] = key;
                  break;
                }
              }
              return next_sibling_->keys_[0];
            }
          }
          else { // Next sibling full, split
            ; // TODO Implementing split of TreeNode
          }
        }
        else { // Does not have a next sibling, split
          next_sibling_ = new TreeNode<T>(m_); // TODO Implementing split of TreeNode
        }
      }
    }
    else { // Non-leaf node
      for (int i = 0; i != size_; ++i) {
        if (0 == keys_[i])
          break;
        if (key < keys_[i])
          return subnodes_[i]->Insert(key);
      }
    }
  }

 // Private variables
 private:
  const int m_;
  int size_;
  T keys_[];
  TreeNode<T>* subnodes_[];
  TreeNode<T>* parent_;
  TreeNode<T>* next_sibling_;

 // Banned operations
 private:
  void operator=(const TreeNode<T>&);
  TreeNode<T>(const TreeNode<T>&);
};
}

#endif
