#include "BTree.h"
using namespace minisql;
using namespace std;

int main(int argc, char** argv) {
  BTree<float> tree(4);
  float key[] {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 6.5, 18.5, 17.5, 12, 6.6, 6.7, 6.8, 6.9};
  cout << "\nInsert: {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 6.5, 18.5, 17.5, 12, 6.6, 6.7, 6.8, 6.9}" << endl;
  tree.Insert(sizeof(key)/sizeof(float), key);
  tree.Print(&TreeNode<float>::Print);
  while (true) {
    cout << "> ";
    string command;
    float value;
    cin >> command;
    if (0 == command.compare("insert")) {
      cin >> value;
      tree.Insert(value);
      cout << "\nThe key \"" << value << "\" inserted" << endl;
      tree.Print(&TreeNode<float>::Print);
    } else if (0 == command.compare("delete")) {
      cin >> value;
      if (true == tree.Delete(value)) {
        cout << "\nThe key \"" << value << "\" deleted" << endl;
        tree.Print(&TreeNode<float>::Print);
      } else {
        cout << "The key \"" << value << "\" not found" << endl;
      }
    } else if (0 == command.compare("search")) {
      cin >> value;
      if (true == tree.Search(value)) {
        cout << "\nThe key \"" << value << "\" found" << endl;
        tree.Print(&TreeNode<float>::Print);
      } else {
        cout << "The key \"" << value << "\" not found" << endl;
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
