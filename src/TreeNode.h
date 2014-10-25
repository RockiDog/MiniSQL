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
    TreeNode(int m): m_(m), size_(0), next_sibling_(0) {
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
    bool set_size(int size) {
      if (size < 0 || size >= m_) {
        size_ = size;
        return true;
      }
      else {
        return false;
      }
    }
    const T* keys(int i) const { return keys_[i]; }
    const TreeNode<T>* subnode(int i) const { return subnodes_[i]; }
    const TreeNode<T>* next_sibling() const { return next_sibling_; }
    bool set_next_sibling(TreeNode<T>* next_sibling) {
      next_sibling_ = next_sibling;
      return true;
    }
    bool set_subnode(int i, TreeNode<T>* subnode) {
      if (i < 0 || i > size_)
        return false;
      else
        subnodes_[i] = subnode;
      return true;
    }
    bool set_key(int i, T* key) {
      if (i < 0 || i > size_ - 1)
        return false;
      else
        keys_[i] = key;
      return true;
    }

   // Fundamental operations
   public:
    // TODO Update father node
    bool Insert(T key) {
      if (0 == subnodes_[0]) { // Leaf node
        if (size_ != m_ - 1) { // Leaf node not full, simply insert
          for (int i = 0; i != size_; ++i) {
            if (key < keys_[i] || i == size_ - 1) {
              for (int j = size_; j > i; --j)
                keys_[j] = keys_[j - 1];
              keys_[i] = key;
              break;
            }
          }
          ++size_;
        }
        else { // Leaf node full, check next sibling
          if (0 != next_sibling_) { // Has next sibling
            if (next_sibling_->size_ != next_sibling_->m_ - 1) { // Next sibling not full
              for (int i = 0; i != size_; ++i)
                ;
            }
            else { // Next sibling full, split
              ; // TODO Implementing split of TreeNode
            }
          }
          else { // Does not have a next sibling, split
            ; // TODO Implementing split of TreeNode
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
    TreeNode<T>* next_sibling_;

   // Banned operations
   private:
    void operator=(const TreeNode<T>&);
    TreeNode<T>(const TreeNode<T>&);
  };
}

#endif
