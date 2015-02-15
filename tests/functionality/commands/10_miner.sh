tomovee_miner -v -s test1 -m $test1 | sed "s|$test1|@TEST1@|g" || echo failed
tomovee_miner -v -s test2 -m $test2 | sed "s|$test2|@TEST2@|g" || echo failed
