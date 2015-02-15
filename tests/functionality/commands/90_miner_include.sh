rm -f "$DB"
tomovee_miner -s test -m "`dirname $test1`" -i test1 > /dev/null
dump_db
