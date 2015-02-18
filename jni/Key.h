#ifndef MINISQL_SRC_KEY_H_
#define MINISQL_SRC_KEY_H_

#include <iostream>

namespace minisql {

template<typename T>
class Key {
 public:
  Key(): offset(-1) {}
  Key(T value, long offset = -1): value(value), offset(offset) {}

  Key(const Key& other) {
    this->value = other.value;
    this->offset = other.offset;
  }

 public:
  bool operator<(const Key<T>& other) const { return (this->value < other.value); }
  bool operator<=(const Key<T>& other) const { return (this->value <= other.value); }
  bool operator>(const Key<T>& other) const { return (this->value > other.value); }
  bool operator>=(const Key<T>& other) const { return (this->value >= other.value); }
  bool operator==(const Key<T>& other) const { return (this->value == other.value); }

  Key& operator=(const Key<T>& other) {
    this->value = other.value;
    this->offset = other.offset;
  }

  template<typename S>
  friend std::ostream& operator<<(std::ostream& os, const Key<S>& key);

 public:
  T value;
  long offset;
};

template<typename S>
std::ostream& operator<<(std::ostream& os, const Key<S>& key) {
  return std::cout << key.value;
}

}

#endif
