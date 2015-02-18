package com.minisql.util;

import com.minisql.IndexManager;

import java.util.Scanner;

public class BTree {
  private int m;
  private long mRoot;
  private int mDataType;

  static { System.loadLibrary("BTree"); }

  public long getRoot() { return mRoot; }

  public BTree(int dataType, int m) {
    mDataType = dataType;
    this.m = m;
    mRoot = nativeCreate(dataType, m);
  }

  public String toString() {
    return new String();
  }

  public void print() { nativePrint(mRoot, mDataType); }
  public void release() { nativeDestroy(mRoot, mDataType); }

  public boolean insert(int value, long blockOffset) { return nativeInsert(mRoot, value, blockOffset); }
  public boolean insert(double value, long blockOffset) { return nativeInsert(mRoot, value, blockOffset); }
  public boolean insert(String value, long blockOffset) { return nativeInsert(mRoot, value, blockOffset); }

  public boolean delete(int value) { return nativeDelete(mRoot, value); }
  public boolean delete(double value) { return nativeDelete(mRoot, value); }
  public boolean delete(String value) { return nativeDelete(mRoot, value); }

  public long search(int value) { return nativeSearch(mRoot, value); }
  public long search(double value) { return nativeSearch(mRoot, value); }
  public long search(String value) { return nativeSearch(mRoot, value); }

  public long retrieve(int value) { return nativeRetrieve(mRoot, value); }
  public long retrieve(double value) { return nativeRetrieve(mRoot, value); }
  public long retrieve(String value) { return nativeRetrieve(mRoot, value); }

  public boolean update(int oldValue, int newValue) { return nativeUpdate(mRoot, oldValue, newValue); }
  public boolean update(double oldValue, double newValue) { return nativeUpdate(mRoot, oldValue, newValue); }
  public boolean update(String oldValue, String newValue) { return nativeUpdate(mRoot, oldValue, newValue); }

  private native long nativeCreate(int dataType, int m);
  private native void nativeDestroy(long root, int dataType);
  private native void nativePrint(long root, int dataType);

  private native boolean nativeInsert(long root, int value, long blockOffset);
  private native boolean nativeInsert(long root, double value, long blockOffset);
  private native boolean nativeInsert(long root, String value, long blockOffset);

  private native boolean nativeDelete(long root, int value);
  private native boolean nativeDelete(long root, double value);
  private native boolean nativeDelete(long root, String value);

  private native long nativeSearch(long root, int value);
  private native long nativeSearch(long root, double value);
  private native long nativeSearch(long root, String value);

  private native long nativeRetrieve(long root, int value);
  private native long nativeRetrieve(long root, double value);
  private native long nativeRetrieve(long root, String value);

  private native boolean nativeUpdate(long root, int oldValue, int newValue);
  private native boolean nativeUpdate(long root, double oldValue, double newValue);
  private native boolean nativeUpdate(long root, String oldValue, String newValue);

  public static void main(String args[]) {
    Scanner scanner = new Scanner(System.in);
    BTree btree = new BTree(IndexManager.CHAR, 4);
    /*
    double[] keys = new double[]{4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 6.5f, 18.5f, 17.5f, 12, 6.6f, 6.7f, 6.8f, 6.9f};
    System.out.println("\nInsert: {4, 7, 3, 6, 1, 2, 8, 5, 17, 18, 11, 23, 19, 6.5, 18.5, 17.5, 12, 6.6, 6.7, 6.8, 6.9}");
    for (double value: keys)
      btree.Insert(value);
    btree.Print();
    */
    while (true) {
      System.out.print("> ");
      String command;
      String value;
      command = scanner.next();
      if (true == command.equals("insert")) {
        value = scanner.next();
        btree.insert(value, 0);
        System.out.println("\nThe key \"" + value + "\" inserted");
        btree.print();
      } else if (true == command.equals("delete")) {
        value = scanner.next();
        if (true == btree.delete(value)) {
          System.out.println("\nThe key \"" + value + "\" deleted");
          btree.print();
        } else {
          System.out.println("\nThe key \"" + value + "\" not found");
        }
      } else if (true == command.equals("search")) {
        value = scanner.next();
        System.out.println(btree.search(value));
        if (-1 != btree.search(value)) {
          System.out.println("\nThe key \"" + value + "\" found");
          btree.print();
        } else {
          System.out.println("\nThe key \"" + value + "\" not found");
        }
      } else if (true == command.equals("retrieve")) {
        value = scanner.next();
        System.out.println();
        System.out.println("\nThe index \"" + btree.retrieve(value) + "\" found");
        btree.print();
      } else if (true == command.equals("quit")) {
        System.out.println("Bye");
        break;
      } else if (true == command.equals("print")) {
        btree.print();
      } else {
        System.out.println("I beg your parden?");
      }
    }
    scanner.close();
  }
}
