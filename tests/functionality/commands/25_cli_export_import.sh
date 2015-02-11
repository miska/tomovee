tomovee_cli export > "$LOG_DIR"/tomovee.json
rm -f "$DB"
tomovee_cli import < "$LOG_DIR"/tomovee.json
rm -f "$LOG_DIR"/tomovee.json
dump_db
