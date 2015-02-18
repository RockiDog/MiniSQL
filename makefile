libBtree.so: com_minisql_util_BTree.o
	g++ -fPIC -shared bin/com_minisql_util_BTree.o -o bin/libBTree.so
com_minisql_util_BTree.o: com_minisql_util_BTree.h
	g++ -fPIC -c -std=c++11 jni/com_minisql_util_BTree.cc \
		-o bin/com_minisql_util_BTree.o \
		-I/usr/lib/jvm/jdk7/include \
		-I/usr/lib/jvm/jdk7/include/linux
com_minisql_util_BTree.h:
	javac -cp src/ src/com/minisql/util/BTree.java -d bin/;
	javah -jni -d jni/ -cp src/ com.minisql.util.BTree
run: libBtree.so
	java -Djava.library.path=./bin -cp ./bin com.minisql.util.BTree
clean:
	rm bin/com_minisql_util_BTree.o;
	rm -rf bin/com
truncate:
	rm bin/com_minisql_util_BTree.o;
	rm -rf bin/com;
	rm bin/libBTree.so
