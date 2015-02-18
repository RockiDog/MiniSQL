#include "com_minisql_util_BTree.h"
#include "BTree.h"
#include "Key.h"

using namespace std;
using namespace minisql;

namespace minisql {

enum DATATYPE {
  INTEGER = 0,
  DOUBLE = 1,
  STRING = 2
};

}

JNIEXPORT jlong JNICALL Java_com_minisql_util_BTree_nativeCreate
(JNIEnv*, jobject, jint data_type, jint m) {
  switch (data_type) {
    case DATATYPE::INTEGER: {
      BTree<Key<jint>>* tree = new BTree<Key<jint>>(m);
      return (jlong)tree;
    }
    case DATATYPE::DOUBLE: {
      BTree<Key<jdouble>>* tree = new BTree<Key<jdouble>>(m);
      return (jlong)tree;
    }
    case DATATYPE::STRING: {
      BTree<Key<string>>* tree = new BTree<Key<string>>(m);
      return (jlong)tree;
    }
  }
  return 0;
}

JNIEXPORT void JNICALL Java_com_minisql_BTree_nativeDestroy
(JNIEnv*, jobject, jlong tree, jint data_type) {
  switch (data_type) {
    case DATATYPE::INTEGER: {
      delete (BTree<Key<jint>>*)tree;
      break;
    }
    case DATATYPE::DOUBLE: {
      delete (BTree<Key<jdouble>>*)tree;
      break;
    }
    case DATATYPE::STRING: {
      delete (BTree<Key<string>>*)tree;
      break;
    }
  }
  tree = 0;
  return;
}

JNIEXPORT void JNICALL Java_com_minisql_util_BTree_nativePrint
(JNIEnv*, jobject, jlong tree, jint data_type) {
  switch (data_type) {
    case DATATYPE::INTEGER:
      ((BTree<Key<jint>>*)tree)->Print(&TreeNode<Key<jint>>::Print);
      break;
    case DATATYPE::DOUBLE:
      ((BTree<Key<jdouble>>*)tree)->Print(&TreeNode<Key<jdouble>>::Print);
      break;
    case DATATYPE::STRING:
      ((BTree<Key<string>>*)tree)->Print(&TreeNode<Key<string>>::Print);
      break;
  }
  return;
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeInsert__JIJ
(JNIEnv*, jobject, jlong tree, jint value, jlong offset) {
  return ((BTree<Key<jint>>*)tree)->Insert(Key<jint>(value, offset));
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeInsert__JDJ
(JNIEnv*, jobject, jlong tree, jdouble value, jlong offset) {
  return ((BTree<Key<jdouble>>*)tree)->Insert(Key<jdouble>(value, offset));
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeInsert__JLjava_lang_String_2J
(JNIEnv* env, jobject, jlong tree, jstring value, jlong offset) {
  string str = env->GetStringUTFChars(value, 0);
  return ((BTree<Key<string>>*)tree)->Insert(Key<string>(str, offset));
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeDelete__JI
(JNIEnv*, jobject, jlong tree, jint value) {
  return ((BTree<Key<jint>>*)tree)->Delete(Key<jint>(value));
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeDelete__JD
(JNIEnv*, jobject, jlong tree, jdouble value) {
  return ((BTree<Key<jdouble>>*)tree)->Delete(Key<jdouble>(value));
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeDelete__JLjava_lang_String_2
(JNIEnv* env, jobject, jlong tree, jstring value) {
  string str = env->GetStringUTFChars(value, 0);
  return ((BTree<Key<string>>*)tree)->Delete(Key<string>(str));
}

JNIEXPORT jlong JNICALL Java_com_minisql_util_BTree_nativeSearch__JI
(JNIEnv*, jobject, jlong tree, jint value) {
  const Key<jint>* result_key = ((BTree<Key<jint>>*)tree)->Search(Key<jint>(value));
  if (0 == result_key)
    return -1;
  return result_key->offset;
}

JNIEXPORT jlong JNICALL Java_com_minisql_util_BTree_nativeSearch__JD
(JNIEnv*, jobject, jlong tree, jdouble value) {
  const Key<jdouble>* result_key = ((BTree<Key<jdouble>>*)tree)->Search(Key<jdouble>(value));
  if (0 == result_key)
    return -1;
  return result_key->offset;
}

JNIEXPORT jlong JNICALL Java_com_minisql_util_BTree_nativeSearch__JLjava_lang_String_2
(JNIEnv* env, jobject, jlong tree, jstring value) {
  string str = env->GetStringUTFChars(value, 0);
  const Key<string>* result_key = ((BTree<Key<string>>*)tree)->Search(Key<string>(str));
  if (0 == result_key)
    return -1;
  return result_key->offset;
}

JNIEXPORT jlong JNICALL Java_com_minisql_util_BTree_nativeRetrieve__JI
(JNIEnv*, jobject, jlong tree, jint value) {
  const Key<jint>* result_key = ((BTree<Key<jint>>*)tree)->Retrieve(Key<jint>(value));
  if (0 == result_key)
    return -1;
  return result_key->offset;
}

JNIEXPORT jlong JNICALL Java_com_minisql_util_BTree_nativeRetrieve__JD
(JNIEnv*, jobject, jlong tree, jdouble value) {
  const Key<jdouble>* result_key = ((BTree<Key<jdouble>>*)tree)->Retrieve(Key<jdouble>(value));
  if (0 == result_key)
    return -1;
  return result_key->offset;
}

JNIEXPORT jlong JNICALL Java_com_minisql_util_BTree_nativeRetrieve__JLjava_lang_String_2
(JNIEnv* env, jobject, jlong tree, jstring value) {
  string str = env->GetStringUTFChars(value, 0);
  const Key<string>* result_key = ((BTree<Key<string>>*)tree)->Retrieve(Key<string>(str));
  if (0 == result_key)
    return -1;
  return result_key->offset;
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeUpdate__JII
(JNIEnv*, jobject, jlong tree, jint old_value, jint new_value) {
  return ((BTree<Key<jint>>*)tree)->Update(Key<jint>(old_value), Key<jint>(new_value));
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeUpdate__JDD
(JNIEnv*, jobject, jlong tree, jdouble old_value, jdouble new_value) {
  return ((BTree<Key<jdouble>>*)tree)->Update(Key<jdouble>(old_value), Key<jdouble>(new_value));
}

JNIEXPORT jboolean JNICALL Java_com_minisql_util_BTree_nativeUpdate__JLjava_lang_String_2Ljava_lang_String_2
(JNIEnv* env, jobject, jlong tree, jstring old_value, jstring new_value) {
  string old_str = env->GetStringUTFChars(old_value, 0);
  string new_str = env->GetStringUTFChars(new_value, 0);
  return ((BTree<Key<string>>*)tree)->Update(Key<string>(old_str), Key<string>(new_str));
}
