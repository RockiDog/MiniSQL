package com.minisql;

import com.minisql.BlockManager;
import com.minisql.BlockName;
import com.minisql.catalog.CManager;
import com.minisql.catalog.TableInfo;
import com.minisql.record.*;
import com.minisql.util.BTree;
import com.minisql.MiniApi;

public class Index {
  private BTree mBTree;
  private int mBTreeDegree;
  private String mTableName;
  private String mAttrName;
  private int mDataType;
  private int mIndexType;
  private int mTableID;

  public Index(String tableName, String attrName, int dataType, int indexType, int bTreeDegree) {
    mTableName = tableName;
    mAttrName = attrName;
    mDataType = dataType;
    mIndexType = indexType;
    mTableID = MiniApi.getTableIDByName(tableName);
    mBTreeDegree = bTreeDegree;
    build();
  }

  public String getTableName() { return mTableName; }
  public int getTableID() { return mTableID; }
  public String getAttrName() { return mAttrName; }
  public int getDataType() { return mDataType; }
  public int getIndexType() { return mIndexType; }

  public long search(int value) { return mBTree.search(value); }
  public long search(double value) { return mBTree.search(value); }
  public long search(String value) { return mBTree.search(value); }

  public long retrieve(int value) { return mBTree.retrieve(value); }
  public long retrieve(double value) { return mBTree.retrieve(value); }
  public long retrieve(String value) { return mBTree.retrieve(value); }

  public void drop() { mBTree.release(); }

  // Update after insertion
  public void update(long dirtyBlockOffset, int dirtyTupleOffset) {
    try {
      BlockName blockHeaderInfo = new BlockName(mTableID, dirtyBlockOffset);
      long blockAmount = BlockManager.getBlockAmountInTable(blockHeaderInfo, BlockManager.RECORD);
      TableInfo tableInfo = CManager.getTableInfo(mTableID);
      int attrID = tableInfo.getAttributeOffset(mAttrName);
      if (blockAmount < 1)
        return;
      if (dirtyBlockOffset == blockAmount - 1) {
        long resultBlockOffset = -1;
        TupleAddress addr0 = new TupleAddress(mTableName, dirtyBlockOffset, 0);
        TupleValue tuple0 = Block.getTupleValue(addr0);
        ConstantValue value0 = tuple0.values.get(attrID).getConstant();
        
        int lastBlockSizeInTuple = tableInfo.numberOfTupleOfLastBlock();
        switch (mDataType) {
          case IndexManager.INTEGER:
            resultBlockOffset = mBTree.search((int)(value0.getValue()));
            if (1 == lastBlockSizeInTuple) {  // New block
              if (-1 == resultBlockOffset)    // No index to this block, insert
                mBTree.insert((int)(value0.getValue()), blockAmount - 1);
            } else {                          // No new block
              if (-1 == resultBlockOffset) {  // No index to this block, update
                TupleAddress addr1 = new TupleAddress(mTableName, dirtyBlockOffset, 1);
                TupleValue tuple1 = Block.getTupleValue(addr1);
                ConstantValue value1 = tuple1.values.get(attrID).getConstant();
                mBTree.update((int)(value1.getValue()), (int)(value0.getValue()));
              }
            }
            break;
          case IndexManager.DOUBLE:
            resultBlockOffset = mBTree.search((double)(value0.getValue()));
            if (1 == lastBlockSizeInTuple) {      // New block
              if (-1 == resultBlockOffset)    // No index to this block, insert
                mBTree.insert((double)(value0.getValue()), blockAmount - 1);
            } else {                          // No new block
              if (-1 == resultBlockOffset) {  // No index to this block, update
                TupleAddress addr1 = new TupleAddress(mTableName, dirtyBlockOffset, 1);
                TupleValue tuple1 = Block.getTupleValue(addr1);
                ConstantValue value1 = tuple1.values.get(attrID).getConstant();
                mBTree.update((double)(value1.getValue()), (double)(value0.getValue()));
              }
            }
            break;
          case IndexManager.CHAR:
            resultBlockOffset = mBTree.search((String)(value0.getValue()));
            if (1 == lastBlockSizeInTuple) {      // New block
              if (-1 == resultBlockOffset)    // No index to this block, insert
                mBTree.insert((String)(value0.getValue()), blockAmount - 1);
            } else {                          // No new block
              if (-1 == resultBlockOffset) {  // No index to this block, update
                TupleAddress addr1 = new TupleAddress(mTableName, dirtyBlockOffset, 1);
                TupleValue tuple1 = Block.getTupleValue(addr1);
                ConstantValue value1 = tuple1.values.get(attrID).getConstant();
                mBTree.update((String)(value1.getValue()), (String)(value0.getValue()));
              }
            }
            break;
        }
        
      }
      
      if (0 == dirtyTupleOffset) {  // Fisrt tuple in block modified
        long resultBlockOffset = -1;
        TupleAddress addr0 = new TupleAddress(mTableName, dirtyBlockOffset, 0);
        TupleAddress addr1 = new TupleAddress(mTableName, dirtyBlockOffset, 1);
        TupleValue tuple0 = Block.getTupleValue(addr0);
        TupleValue tuple1 = Block.getTupleValue(addr1);
        ConstantValue value0 = tuple0.values.get(attrID).getConstant();  // New value
        ConstantValue value1 = tuple1.values.get(attrID).getConstant();  // Old value
        
        switch (mDataType) {
          case IndexManager.INTEGER:
            resultBlockOffset = mBTree.search((int)(value1.getValue()));
            break;
          case IndexManager.DOUBLE:
            resultBlockOffset = mBTree.search((double)(value1.getValue()));
            break;
          case IndexManager.CHAR:
            resultBlockOffset = mBTree.search((String)(value1.getValue()));
            break;
        }
        
        if (resultBlockOffset == dirtyBlockOffset)           // Has index to this block
          if (0 != ConstantValue.compare(value0, value1)) {  // Update index to this block
            switch (mDataType) {
              case IndexManager.INTEGER:
                mBTree.update((int)(value1.getValue()), (int)(value0.getValue()));
                break;
              case IndexManager.DOUBLE:
                mBTree.update((double)(value1.getValue()), (double)(value0.getValue()));
                break;
              case IndexManager.CHAR:
                mBTree.update((String)(value1.getValue()), (String)(value0.getValue()));
                break;
            }
          }
      }
      update(dirtyBlockOffset + 1, 0);
    } catch (Exception e) {
      System.out.println("Excepiton while updating index on \"" + mTableName + "." + mAttrName + "\"");
      System.out.println("Updating index failed");
      e.printStackTrace();
    }
  }

  // Update after deletion
  public void update(long dirtyBlockOffset, int dirtyTupleOffset, ConstantValue oldValue) {
    try {
      BlockName blockHeaderInfo = new BlockName(mTableID, dirtyBlockOffset);
      long blockAmount = BlockManager.getBlockAmountInTable(blockHeaderInfo, BlockManager.RECORD);
      TableInfo tableInfo = CManager.getTableInfo(mTableID);
      int attrID = tableInfo.getAttributeOffset(mAttrName);
      if (blockAmount < 1) {
        switch (mDataType) {
          case IndexManager.INTEGER:
            mBTree.delete((int)(oldValue.getValue()));
            break;
          case IndexManager.DOUBLE:
            mBTree.delete((double)(oldValue.getValue()));
            break;
          case IndexManager.CHAR:
            mBTree.delete((String)(oldValue.getValue()));
            break;
        }
        return;
      }
      
      int lastBlockSizeInTuple = tableInfo.numberOfTupleOfLastBlock();
      int fullBlockSizeInTuple = tableInfo.numberOfTupleOfFullBlock();
      if (lastBlockSizeInTuple == fullBlockSizeInTuple && dirtyBlockOffset == blockAmount) {
        switch (mDataType) {
          case IndexManager.INTEGER:
            mBTree.delete((int)(oldValue.getValue()));
            break;
          case IndexManager.DOUBLE:
            mBTree.delete((double)(oldValue.getValue()));
            break;
          case IndexManager.CHAR:
            mBTree.delete((String)(oldValue.getValue()));
            break;
        }
      }
      
      if (0 == dirtyTupleOffset) {
        long resultBlockOffset = -1;
        TupleAddress addr0 = new TupleAddress(mTableName, dirtyBlockOffset, 0);
        TupleValue tuple0 = Block.getTupleValue(addr0);
        ConstantValue value0 = tuple0.values.get(attrID).getConstant();
        if (ConstantValue.compare(value0, oldValue) > 0) {
          switch (mDataType) {
            case IndexManager.INTEGER:
              resultBlockOffset = mBTree.search((int)(value0.getValue()));
              if (-1 == resultBlockOffset) {  // No index for this new value
                mBTree.insert((int)(value0.getValue()), dirtyBlockOffset);
              } else {
                mBTree.delete((int)(value0.getValue()));
                mBTree.insert((int)(value0.getValue()), dirtyBlockOffset);
              }
              break;
            case IndexManager.DOUBLE:
              if (-1 == resultBlockOffset) {  // No index for this new value
                mBTree.insert((double)(value0.getValue()), dirtyBlockOffset);
              } else {
                mBTree.delete((double)(value0.getValue()));
                mBTree.insert((double)(value0.getValue()), dirtyBlockOffset);
              }
              break;
            case IndexManager.CHAR:
              if (-1 == resultBlockOffset) {  // No index for this new value
                mBTree.insert((String)(value0.getValue()), dirtyBlockOffset);
              } else {
                mBTree.delete((String)(value0.getValue()));
                mBTree.insert((String)(value0.getValue()), dirtyBlockOffset);
              }
              break;
          }
        }
      }
      
      TupleAddress addr1 = new TupleAddress(mTableName, dirtyBlockOffset, fullBlockSizeInTuple - 1);
      TupleValue tuple1 = Block.getTupleValue(addr1);
      ConstantValue value1 = tuple1.values.get(attrID).getConstant();
      update(dirtyBlockOffset + 1, 0, value1);
    } catch (Exception e) {
      System.out.println("Excepiton while updating index on \"" + mTableName + "." + mAttrName + "\"");
      System.out.println("Updating index failed");
      e.printStackTrace();
    }
  }

  private boolean build() {
    try {
      BlockName blockHeaderInfo = new BlockName(mTableID, 0);
      long blockAmount = BlockManager.getBlockAmountInTable(blockHeaderInfo, BlockManager.RECORD);
      if (blockAmount < 1)
        return false;
      
      mBTree = new BTree(mDataType, mBTreeDegree);
      TableInfo tableInfo = CManager.getTableInfo(mTableID);
      int attrID = tableInfo.getAttributeOffset(mAttrName);
      
      TupleAddress addr = new TupleAddress(mTableName, 0, 0);
      TupleValue tuple = Block.getTupleValue(addr);
      ConstantValue lastValue = tuple.values.get(attrID).getConstant();
      switch (mDataType) {
        case IndexManager.INTEGER:
          mBTree.insert((int)(lastValue.getValue()), 0);
          break;
        case IndexManager.DOUBLE:
          mBTree.insert((double)(lastValue.getValue()), 0);
          break;
        case IndexManager.CHAR:
          mBTree.insert((String)(lastValue.getValue()), 0);
          break;
      }
      
      for (long i = 1; i != blockAmount; ++i) {
        addr = new TupleAddress(mTableName, i, 0);
        tuple = Block.getTupleValue(addr);
        ConstantValue value = tuple.values.get(attrID).getConstant();
        if (ConstantValue.compare(value, lastValue) != 0) {
          switch (mDataType) {
            case IndexManager.INTEGER:
              mBTree.insert((int)(lastValue.getValue()), i);
              break;
            case IndexManager.DOUBLE:
              mBTree.insert((double)(lastValue.getValue()), i);
              break;
            case IndexManager.CHAR:
              mBTree.insert((String)(lastValue.getValue()), i);
              break;
          }
        }
      }
    } catch (Exception e) {
      System.out.println("Excepiton while building index on \"" + mTableName + "." + mAttrName + "\"");
      System.out.println("Building index failed");
      e.printStackTrace();
      drop();
      return false;
    }
    return true;
  }

  // TODO
  private byte[] encodeToBinary() {
    String buffer = new String();
    /*
    Integer.toBinaryString(mTableID)
    Integer.toBinaryString(mAttrID)
    Integer.toBinaryString(mDataType)
    Integer.toBinaryString(mIndexType)
    */
    return buffer.getBytes();
  }

  // TODO
  private void rebuildFromBinary(byte[] buffer) {
  }
}
