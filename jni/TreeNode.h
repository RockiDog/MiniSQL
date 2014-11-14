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
  const TreeNode<T>* parent() const { return parent_; }
  const TreeNode<T>* next_sibling() const { return next_sibling_; }
  const TreeNode<T>* const* const subnodes() const { return subnodes_; }
  const TreeNode<T>* subnode(int i) const { return subnodes_[i]; }
  TreeNode<T>* subnode(int i) { return subnodes_[i]; }

  bool set_size(int size) {
    if (size < 0 || size >= m_) {
      size_ = size;
      return true;
    } else {
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
  TreeNode<T>* Delete(T key);
  bool Search(const T& key);
  void Destroy() {
    if (0 == subnodes_)
      return;
    for (int i = 0; i != size_ + 1; ++i) {
      subnodes_[i]->Destroy();
      delete subnodes_[i];
    }
  }

  void Print() const {
    cout << "|* ";
    for (int i = 0; i != size_ - 1; ++i)
      cout << keys_[i] << ", ";
    cout << keys_[size_ - 1] << " ";
    cout << "*|";
    if (0 != next_sibling_)
      cout << "-->";
    else
      cout << "   ";
    return;
  }

 private:
  // Inline tool functions
  TreeNode<T>* split(T key);
  TreeNode<T>* split(int index, TreeNode<T>* subnode);
  const T* search(const T& key);

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
    } else {  // Single root node full, split
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
    } else {                                                  // Leaf node full, check next sibling
      if (0 != next_sibling_) {                               // Has next sibling
        if (next_sibling_->size_ != next_sibling_->m_ - 1) {  // Next sibling not full, insert to it
          if (key > keys_[size_ - 1]) {
            next_sibling_->Insert(key);
          } else {
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
        } else {  // Next sibling full, split
          TreeNode<T>* temp_node = split(key);
          return next_sibling_;
        }
      } else {  // Does not have a next sibling, split
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
        } else {                  // Subnode splited, update
          if (size_ == m_ - 1) {  // Node full, split
            temp_node = split(i, temp_node);
            return temp_node;
          } else {  // Node not full, update
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
        } else {                  // Subnode splited, update
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
          } else {  // Node not full, update
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

/*************************************************************
 * Specification:                                            *
 *                                                           *
 *                  Delete From a Leaf Node                  *
 * Simply delete key and return itself, let parent handle    *
 * the adjustment. Return NULL if key not found.             *
 *                                                           *
 *                Delete From an Internal Node               *
 * First invoke deletion recursively. If NULL is returned,   *
 * then deletion failed, return the NULL recursively. If the *
 * origin node is returned, then the deletion succeeded, and *
 * nothing is to be updated recursively. Otherwise if a leaf *
 * node is returned, then there exist several branches.      *
 * Check the entrance subnode:                               *
 *  + Assume no adjustment is required:                      *
 *  | + If the returned node is from the first subnode then  *
 *  | | return it to the parent, recursively.                *
 *  | |                                                      *
 *  | + Otherwise update the relative key, according to the  *
 *  |   first key of the returned node(always being a leaf)  *
 *  |   then return the current node. No further updatings.  *
 *  |                                                        *
 *  + In case adjustment is required:                        *
 *    + If subnodes are leaves:                              *
 *    | - First apply the adjustment:                        *
 *    |   + If borrowing required, borrow keys from right or *
 *    |   | left sibling, update the relative key.           *
 *    |   + If combination required, combine the adjacent    *
 *    |     two subnodes, update the relative key.           *
 *    | - Next return the previously returned node.          *
 *    |                                                      *
 *    + Otherwise subnodes are non-leaf nodes, the strategy  *
 *      is similar:                                          *
 *      - First apply the adjustment. Rotate relative keys.  *
 *      - Next return the previously returned node.          *
 *                                                           *
 *                  Delete From a Root Node                  *
 * Also exist two possibilities:                             *
 *  + Single root node: implement using the leaf strategy.   *
 *  + Non-leaf root node: implement using the internal node  *
 *    strategy. Note that if there is only one subnode left, *
 *    remove the root node and shallow the tree.             *
 *                                                           *
 *************************************************************/
template<typename T>
TreeNode<T>* TreeNode<T>::Delete(T key) {
  // Leaf or single root node
  if (0 == subnodes_) {
    for (int i = 0; i != size_; ++i) {
      if (key == keys_[i]) {
        //delete keys_[i];
        for (int j = i; j != size_ - 1; ++j)
          keys_[j] = keys_[j + 1];
        --size_;
        return this;                     // Key deleted, return this node
      }
    }
    return 0;                            // No key found, return NULL
  }  // (END) Leaf or single root node

  // Non-leaf node, subnodes being leaves
  if (0 != subnodes_ && 0 == subnodes_[0]->subnodes_) {
    for (int i = 0; i != size_ + 1; ++i)
      if (i == size_ || key < keys_[i]) {                                 // Find an entrance for deletion
        TreeNode<T>* temp_node = subnodes_[i]->Delete(key);
        if (0 != temp_node) {                                             // Key found and deleted
          if (0 != i)                        
            keys_[i - 1] = temp_node->keys_[0];
          
          if (subnodes_[i]->size_ < m_ / 2) {                             // Adjustment required
            if (i != size_) {                                             // Checking for right sibling
              int total = subnodes_[i + 1]->size_ + subnodes_[i]->size_;  // The total key number of two siblings
              if (total >= m_) {                                          // Borrow from right sibling
                int borrow = total / 2 - subnodes_[i]->size_;
                for (int j = 0; j != borrow; ++j) {
                  subnodes_[i]->keys_[subnodes_[i]->size_] = subnodes_[i + 1]->keys_[j];
                  ++subnodes_[i]->size_;
                  --subnodes_[i + 1]->size_;
                }
                for (int j = 0; j != subnodes_[i + 1]->size_; ++j)
                  subnodes_[i + 1]->keys_[j] = subnodes_[i + 1]->keys_[j + borrow];
                keys_[i] = subnodes_[i + 1]->keys_[0];
              } else {                                                    // Combine with right sibling
                int j = 0;
                while (0 != subnodes_[i + 1]->size_) {
                  subnodes_[i]->keys_[subnodes_[i]->size_] = subnodes_[i + 1]->keys_[j];
                  ++subnodes_[i]->size_;
                  --subnodes_[i + 1]->size_;
                  ++j;
                }
                subnodes_[i]->next_sibling_ = subnodes_[i + 1]->next_sibling_;
                delete subnodes_[i + 1];
                subnodes_[i + 1] = 0;
                for (j = i + 1; j != size_; ++j) {                        // Update after deletion
                  subnodes_[j] = subnodes_[j + 1];
                  keys_[j] = keys_[j + 1];
                }
                if (0 != subnodes_[i + 1])
                  keys_[i] = subnodes_[i + 1]->keys_[0];
                --size_;
              }
            } else if (0 != i) {                                          // Checking for left sibling
              int total = subnodes_[i - 1]->size_ + subnodes_[i]->size_;
              if (total >= m_) {                                          // Borrow from left sibling
                int borrow = total / 2 - subnodes_[i]->size_;
                int& left = subnodes_[i - 1]->size_;
                int& right = subnodes_[i]->size_;
                for (int j = 0; j != right; ++j)
                  subnodes_[i]->keys_[j + borrow] = subnodes_[i]->keys_[j];
                while (0 != borrow) {
                  subnodes_[i]->keys_[borrow - 1] = subnodes_[i - 1]->keys_[left - 1];
                  --borrow;
                  ++right;
                  --left;
                }
                keys_[i - 1] = subnodes_[i]->keys_[0];
              } else {                                                    // Combine with left sibling
                int j = 0;
                int& left = subnodes_[i - 1]->size_;
                int& right = subnodes_[i]->size_;
                while (0 != right) {
                  subnodes_[i - 1]->keys_[left] = subnodes_[i]->keys_[j];
                  ++left;
                  --right;
                  ++j;
                }
                subnodes_[i - 1]->next_sibling_ = subnodes_[i]->next_sibling_;
                delete subnodes_[i];
                subnodes_[i] = 0;
                --size_;
              }
            }
          }
            
          if (0 == i && 0 != parent_)
            return temp_node;
          else
            return this;
        } else {                                                          // Key not found, get NULL returned
          return 0;
        }
      }
  }  // (END) Non-leaf node, subnodes being leaves

  // Non-leaf node, subnodes being internal node
  if (0 != subnodes_ && 0 != subnodes_[0]->subnodes_) {
    for (int i = 0; i != size_ + 1; ++i)
      if (i == size_ || key < keys_[i]) {                                 // Find an entrance for deletion
        TreeNode<T>* temp_node = subnodes_[i]->Delete(key);
        if (0 != temp_node) {                                             // Key found and deleted
          if (0 == temp_node->subnodes_ && 0 != i)                        
            keys_[i - 1] = temp_node->keys_[0];
          
          if (subnodes_[i]->size_ + 1 < (m_ + 1) / 2) {                   // Adjustment required
            if (i != size_) {                                             // Checking for right sibling
              int total = subnodes_[i + 1]->size_ + subnodes_[i]->size_ + 2;
              if (total > m_) {                                           // Borrow from right sibling
                int borrow = total / 2 - subnodes_[i]->size_;
                for (int j = 0; j != borrow; ++j) {
                  subnodes_[i]->subnodes_[subnodes_[i]->size_ + 1] = subnodes_[i + 1]->subnodes_[j];
                  //subnodes_[i + 1]->subnodes_[j] = subnodes_[i + 1]->subnodes_[j + borrow];
                  if (0 == j) {                                           // First borrow, rotate
                    subnodes_[i]->keys_[subnodes_[i]->size_] = keys_[i];
                    ++(subnodes_[i]->size_);
                  } else if (j == borrow - 1) {                           // Last borrow, rotate
                    keys_[i] = subnodes_[i + 1]->keys_[j - 1];
                    --(subnodes_[i + 1]->size_);
                  } else {                                                // Otherwise, borrow the key
                    subnodes_[i]->keys_[subnodes_[i]->size_] = subnodes_[i + 1]->keys_[j - 1];
                    //subnodes_[i + 1]->keys_[j - 1] = subnodes_[i + 1]->keys_[j - 1 + borrow];
                    ++(subnodes_[i]->size_);
                    --(subnodes_[i + 1]->size_);
                  }
                }
                for (int j = 0; j != subnodes_[i + 1]->size_ + 1; ++j) {
                  subnodes_[i + 1]->subnodes_[j] = subnodes_[i + 1]->subnodes_[j + borrow];
                  if (j != subnodes_[i + 1]->size_)
                    subnodes_[i + 1]->keys_[j] = subnodes_[i + 1]->keys_[j + borrow];
                }
              } else {                                                    // Combine with right sibling
                int j = 0;
                int& left = subnodes_[i]->size_;
                int& right = subnodes_[i + 1]->size_;
                while (0 != right) {
                  subnodes_[i]->subnodes_[left + 1] = subnodes_[i + 1]->subnodes_[j];
                  if (0 == j) {
                    subnodes_[i]->keys_[left] = keys_[i];                 // First move, rotate
                    subnodes_[i]->subnodes_[left]->next_sibling_ = subnodes_[i]->subnodes_[left + 1];
                  } else {                                                // Otherwise, move
                    subnodes_[i]->keys_[left] = subnodes_[i + 1]->keys_[j - 1];
                    --right;
                  }
                  ++left;
                  ++j;
                }
                subnodes_[i]->next_sibling_ = subnodes_[i + 1]->next_sibling_;
                delete subnodes_[i + 1];
                subnodes_[i + 1] = 0;
                for (int j = i + 1; j != size_; ++j) {                    // Update after deletion
                  subnodes_[j] = subnodes_[j + 1];
                  keys_[j - 1] = keys_[j];
                }
                --size_;
              }
            }
            else if (0 != i) {                                            // Checking for left sibling
              int total = subnodes_[i - 1]->size_  + subnodes_[i]->size_ + 2;
              if (total > m_) {                                           // Borrow from left sibling
                int borrow = total / 2 - subnodes_[i]->size_;
                int& left = subnodes_[i - 1]->size_;
                int& right = subnodes_[i]->size_;
                for (int j = 0; j != right + 1; ++j) {
                  subnodes_[i]->subnodes_[j + borrow] = subnodes_[i]->subnodes_[j];
                  if (0 != j)
                    subnodes_[i]->keys_[j - 1 + borrow] = subnodes_[i]->keys_[j - 1];
                  else
                    subnodes_[i]->keys_[j - 1 + borrow] = keys_[i - 1];
                }
                while (0 != borrow) {
                  subnodes_[i]->subnodes_[borrow - 1] = subnodes_[i - 1]->subnodes_[left];
                  if (1 != borrow)
                    subnodes_[i]->keys_[borrow - 2] = subnodes_[i - 1]->keys_[left - 1];
                  else
                    keys_[i - 1] = subnodes_[i - 1]->keys_[left - 1];     // Rotate
                  --borrow;
                  ++right;
                  --left;
                }
              } else {                                                    // Combine with left sibling
                int j = 0;
                int& left = subnodes_[i - 1]->size_;
                int& right = subnodes_[i]->size_;
                while (0 != right) {
                  subnodes_[i - 1]->subnodes_[left + 1] = subnodes_[i]->subnodes_[j];
                  if (0 == j) {
                    subnodes_[i - 1]->keys_[left] = keys_[i - 1];         // First move, rotate
                  } else {                                                // Otherwise, move
                    subnodes_[i - 1]->keys_[left] = subnodes_[i]->keys_[j - 1];
                    --right;
                  }
                  ++left;
                  ++j;
                }
                subnodes_[i - 1]->next_sibling_ = subnodes_[i]->next_sibling_;
                delete subnodes_[i];
                subnodes_[i] = 0;
                --size_;
              }
            }
          }
          
          if (0 == temp_node->subnodes_ && 0 == i && 0 != parent_)
            return temp_node;
          else
            return this;
        } else {                                                          // Key not found, get NULL returned
          return 0;
        }
      }
  }  // (END) Non-leaf node, subnodes being internal node
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
        for (int j = size_ / 2; j > i; --j)
          keys_[j] = keys_[j - 1];
        keys_[i] = key;
        break;
      }
    }
    size_ = size_ / 2 + 1;
  } else {  // Move the last round_down(size_/2) keys, key inserted into the sibling
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
    keys_[index] = (0 == subnode->subnodes_) ? subnode->keys_[0] : subnodes_[index]->keys_[subnodes_[index]->size_];  // XXX
    subnodes_[index + 1] = subnode;
    subnodes_[index + 1]->parent_ = subnodes_[index]->parent_;
    subnodes_[index + 1]->next_sibling_ = subnodes_[index]->next_sibling_;
    subnodes_[index]->next_sibling_ = subnodes_[index + 1];
    subnodes_[size_]->next_sibling_ = 0;
  } else {  // The subnode should be moved to the sibling
    int j = (m_ + 1) / 2 - 1;
    int i = m_ - 1;
    while (0 != j) {
      if (i == index) {
        temp_node->subnodes_[j] = subnode;
        subnode->parent_ = temp_node;
        temp_node->keys_[j - 1] = (0 == subnode->subnodes_) ? subnode->keys_[0] : subnodes_[index]->keys_[subnodes_[index]->size_];  // XXX
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
      keys_[size_] = (0 == subnode->subnodes_) ? subnode->keys_[0] : subnodes_[index]->keys_[subnodes_[index]->size_];  // XXX
    } else {
      temp_node->subnodes_[j] = this->subnodes_[i];
      temp_node->subnodes_[j]->parent_ = temp_node;
      --size_;
    }
  }  // (END) The subnode should be moved to the sibling
  subnodes_[size_]->next_sibling_ = 0;
  return temp_node;
}

template<typename T>
bool TreeNode<T>::Search(const T& key) {
  const T* temp_key = search(key);
  if (0 == temp_key)
    return false;
  else
    return true;
}


template<typename T>
const T* TreeNode<T>::search(const T& key) {
  // Leaf or single root node
  if (0 == subnodes_) {
    for (int i = 0; i != size_; ++i)
      if (key == keys_[i])
        return &keys_[i];
    return 0;  // No key found, return NULL
  }

  // Non-leaf node
  if (0 != subnodes_)
    for (int i = 0; i != size_ + 1; ++i)
      if (i == size_ || key < keys_[i])  // Find an entrance for searching
        return subnodes_[i]->search(key);
}

}  // (END) Namespace minisql

#endif
