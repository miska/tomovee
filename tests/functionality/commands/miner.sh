test_it "miner"
tomovee_miner -s test1 -m $test1 >&2 || echo failed
tomovee_miner -s test2 -m $test2 >&2 || echo failed
dump_db
