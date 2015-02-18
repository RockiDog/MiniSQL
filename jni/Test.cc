#include "BTree.h"
using namespace minisql;
using namespace std;

int main(int argc, char** argv) {
  /*
  BTree<float> tree(4);
  float key[] {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 6.5, 18.5, 17.5, 12, 6.6, 6.7, 6.8, 6.9};
  cout << "\nInsert: {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 6.5, 18.5, 17.5, 12, 6.6, 6.7, 6.8, 6.9}" << endl;
  tree.Insert(sizeof(key)/sizeof(float), key);
  tree.Print(&TreeNode<float>::Print);
  */
  /*
  float key[] {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 12};
  cout << "\nInsert: {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 12}" << endl;
  */
  BTree<float> tree(5);
  float key[] {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 12, 24, 25, 26, 27, 28, 29, 30};
  cout << "\nInsert: {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 12, 24, 25, 26, 27, 28, 29, 30}" << endl;
  tree.Insert(sizeof(key) / sizeof(float), key);
  /*
  float d_key[] {11, 12, 8, 7, 5, 4, 3};
  for (int i = 0; i < sizeof(d_key) / sizeof(float); ++i)
    tree.Delete(d_key[i]);
    */
  tree.Print(&TreeNode<float>::Print);
  while (true) {
    cout << "> ";
    string command;
    float value;
    cin >> command;
    if ('i' == command[0]) {
      cin >> value;
      tree.Insert(value);
      cout << "\nThe key \"" << value << "\" inserted" << endl;
      tree.Print(&TreeNode<float>::Print);
    } else if ('d' == command[0]) {
      cin >> value;
      if (true == tree.Delete(value)) {
        cout << "\nThe key \"" << value << "\" deleted" << endl;
        tree.Print(&TreeNode<float>::Print);
      } else {
        cout << "The key \"" << value << "\" not found" << endl;
      }
    } else if ('s' == command[0]) {
      cin >> value;
      if (0 != tree.Search(value)) {
        cout << "\nThe key \"" << value << "\" found" << endl;
        tree.Print(&TreeNode<float>::Print);
      } else {
        cout << "The key \"" << value << "\" not found" << endl;
      }
    } else if ('r' == command[0]) {
      cin >> value;
      const float* temp_value = tree.Retrieve(value);
      if (0 != temp_value) {
        cout << "\nThe index \"" << *temp_value << "\" found" << endl;
        tree.Print(&TreeNode<float>::Print);
      } else {
        cout << "The index not found" << endl;
      }
    } else if (0 == command.compare("quit")) {
      cout << "Bye" << endl;
      break;
    } else if (0 == command.compare("print")) {
      tree.Print(&TreeNode<float>::Print);
    } else {
      cout << "I beg your parden?" << endl;
    }
  }
  return 0;
}
