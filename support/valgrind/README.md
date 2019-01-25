
* Memory Leak Check
user@host ~/Documents/git/free5gc$ \
    sudo valgrind --leak-check=full --show-leak-kinds=all \
    --suppressions=support/valgrind/mongoc.suppressions \
    --suppressions=support/valgrind/freeDiameter.suppressions \
    ./test/testngc -f ./install/etc/free5gc/test/free5gc.testngc.conf

* Generate Suppression File
user@host ~/Documents/git/free5gc$ \
    sudo valgrind --leak-check=full --show-reachable=yes --error-limit=no \
    --gen-suppressions=all --log-file=support/valgrind/test.log \
    --suppressions=support/valgrind/mongoc.suppressions \
    ./test/testngc -f ./install/etc/free5gc/test/free5gc.testngc.conf

user@host ~/Documents/git/free5gc/support/valgrind$ \
    sudo chown user:user test.log

user@host ~/Documents/git/free5gc/support/valgrind$ \
    cat ./test.log | ./parse_suppressions.sh > test.supp
