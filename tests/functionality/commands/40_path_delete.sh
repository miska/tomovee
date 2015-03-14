rm -f "$DB"
tomovee_miner -s test -m "$test2"  > /dev/null || echo fail
dump_db
rm "$test2"/c.avi
tomovee_miner -s test -md "$test2" > /dev/null || echo fail
dump_db
cp "$movie" "$test2"/c.avi
touch -r "$test2"/d.avi "$test2"/c.avi
