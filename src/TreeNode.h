#ifndef MINISQL_SRC_TREENODE_H_
#define MINISQL_SRC_TREENODE_H_

#include <cstddef>
#include <iostream>
using namespace std;

namespace minisql {

// The B-Tree node data structure
template<typename T>
class TreeNode {
 public:
  // Constructors&Destructors
  TreeNode(int m): m_(m), size_(0), subnodes_(0), parent_(0), next_sibling_(0) {
    keys_ = new T[m_ - 1]{0};
  }

  ~TreeNode() {
    if (0 != subnodes_) {
      for (int i = 0; i != m_; ++i) {
        delete subnodes_[i];
        subnodes_[i] = 0;
      }
      delete [] subnodes_;
      subnodes_ = 0;
    }
    delete [] keys_;
    keys_ = 0;
  }

 public:
  // Getting&Setting functions
  int m() const { return m_; }
  int size() const {return size_; }
  const T keys(int i) const { return keys_[i]; }
  const TreeNode<T>* const* subnodes() const { return subnodes_; }
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

 public:
  // Fundamental operations
  TreeNode<T>* Insert(T key);
  void Print() const {
    cout << "|* ";
    for (int i = 0; i != size_; ++i)
      cout << keys_[i] << " ";
    cout << "*| ";
    return;
  }

 private:
  // Inline tool functions
  TreeNode<T>* split(T key);
  TreeNode<T>* split(int index, TreeNode<T>* subnode);

 private:
  // Private variables
  const int m_;
  int size_;
  T* keys_;
  TreeNode<T>** subnodes_;
  TreeNode<T>* parent_;
  TreeNode<T>* next_sibling_;

 private:
  // Banned operations
  void operator=(const TreeNode<T>&);
  TreeNode<T>(const TreeNode<T>&);
};

template<typename T>
TreeNode<T>* TreeNode<T>::Insert(T key) {
  // Single root node
  if (0 == subnodes_ && 0 == parent_) {
    if (size_ != m_ - 1) {  // Single root node not full, simply insert
      ++size_;
      keys_[size_ - 1] = key;
      for (int i = 0; i != size_; ++i)
        if (key < keys_[i] || i == size_ - 1) {
          for (int j = size_ - 1; j > i; --j)
            keys_[j] = keys_[j - 1];
          keys_[i] = key;
          break;
        }
      return this;
    }
    else {  // Single root node full, split
      TreeNode<T>* temp_node = split(key);
      TreeNode<T>* parent = new TreeNode<T>(m_);
      parent->subnodes_ = new TreeNode<T>*[m_]{0};
      parent->subnodes_[0] = this;
      parent->subnodes_[1] = temp_node;
      parent->keys_[0] = temp_node->keys_[0];
      ++(parent->size_);
      parent_ = parent;
      temp_node->parent_ = parent;
      return parent;
    }
  }  // (END) Single root node

  // Leaf, non-root node
  if (0 == subnodes_ && 0 != parent_) {
    if (size_ != m_ - 1) {  // Leaf node not full, simply insert
      ++size_;
      keys_[size_ - 1] = key;
      for (int i = 0; i != size_; ++i)
        if (key < keys_[i] || i == size_ - 1) {
          for (int j = size_ - 1; j != i; --j)
            keys_[j] = keys_[j - 1];
          keys_[i] = key;
          break;
        }
      return this;
    }
    else {                                                    // Leaf node full, check next sibling
      if (0 != next_sibling_) {                               // Has next sibling
        if (next_sibling_->size_ != next_sibling_->m_ - 1) {  // Next sibling not full, insert to it
          if (key > keys_[size_ - 1]) {
            next_sibling_->Insert(key);
          }
          else {
            next_sibling_->Insert(keys_[size_ - 1]);
            keys_[size_ - 1] = key;
            for (int i = 0; i != size_ - 1; ++i)
              if (key < keys_[i] || i == size_ - 1) {
                for (int j = size_ - 1; j != i; --j)
                  keys_[j] = keys_[j - 1];
                keys_[i] = key;
                break;
              }
          }
          return this;
        }       // (END) Next sibling not full
        else {  // Next sibling full, split
          TreeNode<T>* temp_node = split(key);
          return next_sibling_;
        }
      }       // (END) Has next sibling
      else {  // Does not have a next sibling, split
        TreeNode<T>* temp_node = split(key);
        return next_sibling_;
      }
    }
  }  // (END) Leaf, non-root node

  // Non-leaf, non-root node
  if (0 != subnodes_ && 0 != parent_) {
    for (int i = 0; i != size_ + 1; ++i)
      if (i == size_ || key < keys_[i]) {
        TreeNode<T>* temp_node = subnodes_[i]->Insert(key);
        if (temp_node == subnodes_[i]) {     // No splitting happened
          if (0 == subnodes_[i]->subnodes_)  // Subnodes are leaves
            if (i != size_ && subnodes_[i + 1]->keys_[0] < keys_[i])
              keys_[i] = subnodes_[i + 1]->keys_[0];
          return this;
        }
        else {                    // Subnode splited, update
          if (size_ == m_ - 1) {  // Node full, split
            temp_node = split(i, temp_node);
            return temp_node;
          }
          else {  // Node not full, update
            for (int j = size_; j != i; --j) {
              keys_[j] = keys_[j - 1];
              subnodes_[j + 1] = subnodes_[j];
            }
            keys_[i] = (0 == temp_node->subnodes_) ? temp_node->keys_[0] : subnodes_[i]->keys_[subnodes_[i]->size_];
            subnodes_[i + 1] = temp_node;
            ++size_;
            return this;
          }
        }
      }
  }  // (END) Non-leaf, non-root node

  // Non-leaf, root node
  if (0 != subnodes_ && 0 == parent_) {
    for (int i = 0; i != size_ + 1; ++i)
      if (i == size_ || key < keys_[i]) {
        TreeNode<T>* temp_node = subnodes_[i]->Insert(key);
        if (temp_node == subnodes_[i]) {     // No splitting happened
          if (0 == subnodes_[i]->subnodes_)  // Subnodes are leaves
            if (i != size_ && subnodes_[i + 1]->keys_[0] < keys_[i])
              keys_[i] = subnodes_[i + 1]->keys_[0];
          return this;
        }
        else {                    // Subnode splited, update
          if (size_ == m_ - 1) {  // Node full, split
            temp_node = split(i, temp_node);
            TreeNode<T>* parent = new TreeNode<T>(m_);
            parent->subnodes_ = new TreeNode<T>*[m_]{0};
            parent->subnodes_[0] = this;
            parent->subnodes_[1] = temp_node;
            parent->keys_[0] = keys_[size_];
            ++(parent->size_);
            parent_ = parent;
            temp_node->parent_ = parent;
            return parent;
          }
          else {  // Node not full, update
            for (int j = size_; j != i; --j) {
              keys_[j] = keys_[j - 1];
              subnodes_[j + 1] = subnodes_[j];
            }
            keys_[i] = (0 == temp_node->subnodes_) ? temp_node->keys_[0] : subnodes_[i]->keys_[subnodes_[i]->size_];
            subnodes_[i + 1] = temp_node;
            ++size_;
            return this;
          }
        }
      }
  }  // (END) Non-leaf, root node
}

template<typename T>
TreeNode<T>* TreeNode<T>::split(T key) {
  TreeNode<T>* temp_node = new TreeNode<T>(m_);
  temp_node->parent_ = this->parent_;
  temp_node->next_sibling_ = this->next_sibling_;
  next_sibling_ = temp_node;
  if (key < keys_[size_ / 2]) {  // Move the last round_up(size_/2) keys, key left in the origin node
    for (int i = size_ / 2, j = 0; i != size_; ++i, ++j) {
      temp_node->keys_[j] = this->keys_[i];
      ++(temp_node->size_);
    }
    for (int i = 0; i != size_ / 2; ++i) {
      if (key < keys_[i] || i == size_ / 2 - 1) {
        for (int j = size_ / 2 - 1; j > i; --j)
          keys_[j] = keys_[j - 1];
        keys_[i] = key;
        break;
      }
    }
    size_ = size_ / 2 + 1;
  }       // (END) Move the last round_up(size_/2) keys, key left in the origin node
  else {  // Move the last round_down(size/2) keys, key inserted into the sibling
    /*
    int i = (size_ + 1) / 2;
    int j = 0;
    while (i != size_) {
      if (key < keys_[i]) {
        temp_node->keys_[j] = key;
        ++(temp_node->size_);
        ++j;
        break;
      }
      temp_node->keys_[j] = keys_[i];
      ++(temp_node->size_);
      ++j;
      ++i;
    }
    while (i != size_) {
      temp_node->keys_[j] = keys_[i];
      ++(temp_node->size_);
      ++j;
      ++i;
    }
    size_ = (size_ + 1) / 2;
    */
    int i = size_;
    int j = (size_ + 1) / 2;
    while (0 != j) {
      if (key >= keys_[i - 1]) {
        temp_node->keys_[j - 1] = key;
        ++(temp_node->size_);
        --j;
        break;
      }
      temp_node->keys_[j - 1] = keys_[i - 1];
      ++(temp_node->size_);
      --size_;
      --i;
      --j;
    }
    while (0 != j) {
      temp_node->keys_[j - 1] = keys_[i - 1];
      ++(temp_node->size_);
      --size_;
      --i;
      --j;
    }
  }  // (END) Move the last round_down(size/2) keys, key inserted into the sibling
  return temp_node;
}

template<typename T>
TreeNode<T>* TreeNode<T>::split(int index, TreeNode<T>* subnode) {
  TreeNode<T>* temp_node = new TreeNode<T>(m_);
  temp_node->subnodes_ = new TreeNode<T>*[m_]{0};
  temp_node->next_sibling_ = this->next_sibling_;
  next_sibling_ = temp_node;
  if (index < m_ / 2) {  // The subnode should be left in the origin node
    for (int i = m_ / 2, j = 0; i != m_; ++i, ++j) {
      temp_node->subnodes_[j] = this->subnodes_[i];
      temp_node->subnodes_[j]->parent_ = temp_node;
      if (i != m_ - 1) {
        temp_node->keys_[j] = this->keys_[i];
        ++temp_node->size_;
        --size_;
      }
    }
    for (int i = size_; i != index; --i) {
      keys_[i] = keys_[i - 1];
      if (i != index + 1)
        subnodes_[i] = subnodes_[i - 1];
    }
    keys_[index] = (0 == subnode->subnodes_) ? subnode->keys_[0] : subnodes_[index]->keys_[size_];  // XXX
    subnodes_[index + 1] = subnode;
    subnodes_[index + 1]->parent_ = subnodes_[index]->parent_;
    subnodes_[index + 1]->next_sibling_ = subnodes_[index]->next_sibling_;
    subnodes_[index]->next_sibling_ = subnodes_[index + 1];
    subnodes_[size_]->next_sibling_ = 0;
  }       // (END) The subnode left in the origin node
  else {  // The subnode should be moved to the sibling
    int j = (m_ + 1) / 2 - 1;
    int i = m_ - 1;
    while (0 != j) {
      if (i == index) {
        temp_node->subnodes_[j] = subnode;
        subnode->parent_ = temp_node;
        temp_node->keys_[j - 1] = (0 == subnode->subnodes_) ? subnode->keys_[0] : subnodes_[index]->keys_[size_];  // XXX
        ++(temp_node->size_);
        --j;
        break;
      }
      temp_node->subnodes_[j] = this->subnodes_[i];
      temp_node->subnodes_[j]->parent_ = temp_node;
      temp_node->keys_[j - 1] = this->keys_[i - 1];
      ++(temp_node->size_);
      --size_;
      --i;
      --j;
    }
    while (0 != j) {
      temp_node->subnodes_[j] = this->subnodes_[i];
      temp_node->subnodes_[j]->parent_ = temp_node;
      temp_node->keys_[j - 1] = this->keys_[i - 1];
      ++(temp_node->size_);
      --size_;
      --i;
      --j;
    }
    if (i != index) {  // j == 0
      temp_node->subnodes_[j] = subnode;
      subnode->parent_ = temp_node;
      keys_[size_] = (0 == subnode->subnodes_) ? subnode->keys_[0] : subnodes_[index]->keys_[size_];  // XXX
    }
    else {
      temp_node->subnodes_[j] = this->subnodes_[i];
      temp_node->subnodes_[j]->parent_ = temp_node;
      --size_;
    }
  }  // (END) The subnode moved to the sibling
  subnodes_[size_]->next_sibling_ = 0;
  return temp_node;
}

}  // (END) Namespace minisql


#endif
