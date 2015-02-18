package com.minisql;

import com.minisql.catalog.CManager;
import com.minisql.catalog.TableInfo;
import com.minisql.Index;
import com.minisql.record.TupleAddress;
import com.minisql.record.TupleValue;

import java.util.HashMap;

public class IndexManager {
  public static final int SPARSE = 0;
  public static final int DENSE = 1;

  public static final int INTEGER = 0;
  public static final int DOUBLE = 1;
  public static final int CHAR = 2;

  public static final int DEFAULT_B_TREE_DEGREE = 4;

  private static HashMap<String, Index> mIndexMap;

  static {
    mIndexMap = new HashMap<String, Index>();
  }

  public static boolean createIndex(String indexName, String tableName, String attrName, int dataType, int indexType) {
    Index index = new Index(tableName, attrName, dataType, indexType, DEFAULT_B_TREE_DEGREE);
    mIndexMap.put(indexName, index);
    return true;
  }

  public static boolean createIndex(String indexName, String tableName, String attrName, int dataType, int indexType, int bTreeDegree) {
    Index index = new Index(tableName, attrName, dataType, indexType, bTreeDegree);
    mIndexMap.put(indexName, index);
    return true;
  }

  public static boolean dropIndex(String indexName) {
    Index index = mIndexMap.get(indexName);
    if (null == index) {
      return false;
    } else {
      index.drop();
      mIndexMap.remove(indexName);
      return true;
    }
  }

  public static boolean updateIndexAfterInsertion(String indexName, TupleAddress dirtyTupleAddress) {
    long dirtyBlockOffset = dirtyTupleAddress.blockOffset;
    int dirtyTupleOffset = dirtyTupleAddress.tupleOffset;
    Index index = mIndexMap.get(indexName);
    index.update(dirtyBlockOffset, dirtyTupleOffset);
    return true;
  }

  public static boolean updateIndexAfterDeletion(String indexName, TupleAddress dirtyTupleAddress, TupleValue oldValue) throws Exception {
    long dirtyBlockOffset = dirtyTupleAddress.blockOffset;
    int dirtyTupleOffset = dirtyTupleAddress.tupleOffset;
    Index index = mIndexMap.get(indexName);
    TableInfo tableInfo = CManager.getTableInfo(index.getTableName());
    int attrID = tableInfo.getAttributeOffset(index.getAttrName());
    ConstantValue value = oldValue.values.get(attrID).getConstant();
    index.update(dirtyBlockOffset, dirtyTupleOffset, value);
    return true;
  }

  public static long searchIndex(String indexName, int value) {
    Index index = mIndexMap.get(indexName);
    if (index.getDataType() == IndexManager.INTEGER)
      return index.search(value);
    else
      return -1;
  }

  public static long searchIndex(String indexName, double value) {
    Index index = mIndexMap.get(indexName);
    if (index.getDataType() == IndexManager.DOUBLE)
      return index.search(value);
    else
      return -1;
  }

  public static long searchIndex(String indexName, String value) {
    Index index = mIndexMap.get(indexName);
    if (index.getDataType() == IndexManager.CHAR)
      return index.search(value);
    else
      return -1;
  }

  public static long retrieveIndex(String indexName, int value) {
    Index index = mIndexMap.get(indexName);
    if (index.getDataType() == IndexManager.INTEGER)
      return index.retrieve(value);
    else
      return -1;
  }

  public static long retrieveIndex(String indexName, double value) {
    Index index = mIndexMap.get(indexName);
    if (index.getDataType() == IndexManager.DOUBLE)
      return index.retrieve(value);
    else
      return -1;
  }

  public static long retrieveIndex(String indexName, String value) {
    Index index = mIndexMap.get(indexName);
    if (index.getDataType() == IndexManager.CHAR)
      return index.retrieve(value);
    else
      return -1;
  }

  public static int getTableIDByIndexName(String indexName) {
    return mIndexMap.get(indexName).getTableID();
  }

  public static BlockName searchInsertPosition(String indexName, ConstantValue value) {
    long blockOffset = 0;
    if (value.valueType == ConstantValue.INT_TYPE)
      blockOffset = retrieveIndex(indexName, (int)(value.getValue()));
    if (value.valueType == ConstantValue.DOUBLE_TYPE)
      blockOffset = retrieveIndex(indexName, (double)(value.getValue()));
    if (value.valueType == ConstantValue.STRING_TYPE)
      blockOffset = retrieveIndex(indexName, (String)value.getValue());
    return new BlockName(getTableIDByIndexName(indexName), blockOffset);
  }
}
