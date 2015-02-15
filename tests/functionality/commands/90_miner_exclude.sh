rm -f "$DB"
tomovee_miner -s test -m "`dirname $test1`" -e test2 > /dev/null
dump_db
