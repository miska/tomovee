#!/bin/sh

PASS=0
TOTAL=0

print_result() {
    _ret=0
    if [ "$1" -eq 0 ]; then
        echo " * PASSED"
        PASS="`expr $PASS + 1`"
    else
        echo " * FAILED"
        _ret=1
        FAILED="$FAILED $NAME"
    fi
    TOTAL="`expr $TOTAL + 1`"
    echo
    return $_ret
}

test_it() {
    if [ "$1" ]; then
        NAME="$1"
    fi
    echo "Running test $NAME:"
}

run_it() {
    echo "$@"
    "$@"
}

export TZ="/usr/share/zoneinfo/Europe/Prague"

dump_db() {
sqlite3 $DB <<EOF
.mode csv 
.header on 
select size,mhash,osdbhash,added,audios,subtitles,width,height,length,movie_assigned_by,movie_id from files order by osdbhash;
select storage,path,file_id from paths order by storage,path;
EOF
}

cd "`dirname $0`"

export PATH="`cd ..; pwd`:$PATH"

[ "$LOG_DIR" ] || LOG_DIR="`pwd`/functionality/log"
[ "$RUN_DIR" ] || RUN_DIR="`pwd`/functionality/run"
mkdir -p "$LOG_DIR" || exit 4

export TOMOVEE_DB=sqlite:"$LOG_DIR"/tomovee.sqlite
export DB="$LOG_DIR"/tomovee.sqlite
rm -f "$DB"

# Setup testing structure
mkdir -p "$RUN_DIR"/test1/inside
mkdir -p "$RUN_DIR"/test2
cp fixtures/movies/breakdance.avi "$RUN_DIR"/test1/a.avi
cp fixtures/movies/breakdance.avi "$RUN_DIR"/test1/inside/whatever.avi
cp fixtures/movies/breakdance.avi "$RUN_DIR"/test2/c.avi
cp fixtures/movies/breakdance.avi "$RUN_DIR"/test2/d.avi
echo 'a' >> "$RUN_DIR"/test2/d.avi
find "$RUN_DIR"/test* -exec touch -d "2014-09-26 11:00:00 CEST" \{\} \;

export test1="$RUN_DIR"/test1
export test2="$RUN_DIR"/test2

cd functionality/commands

POSITIVE=""
NEGATIVE=""
while [ "$1" ]; do
    if [ -z "`echo "x$1" | grep "^x-"`" ]; then
        POSITIVE="$POSITIVE $1"
    else
        NEGATIVE="$NEGATIVE `echo "x$1" | sed 's|^x-||'`"
    fi
    shift
done
[ -n "$POSITIVE" ] || POSITIVE="*"

for i in $POSITIVE; do
    for SCRIPT in *$i*; do
    SKIP=""
    for n in $NEGATIVE; do
        if expr match $SCRIPT .\*"$n".\* > /dev/null; then
            SKIP="true"
        fi
    done
    [ -z "$SKIP" ] || continue
    NAME="`echo "$SCRIPT" | sed 's|^[0-9]*_\(.*\)\.sh$|\1|'`"
    EXPECT="../results/$NAME".res
    RESULT="$LOG_DIR/$NAME".log
    test_it
    . ./"$SCRIPT" > "$RESULT"
    [ -r "$EXPECT" ] || touch "$EXPECT"
    if [ -r "$RESULT" ]; then
        diff -Naruw "$EXPECT" "$RESULT"
        print_result $?
    fi
    done
done

echo "Testing completed, $PASS/$TOTAL tests passed"

if [ -n "$FAILED" ]; then
    echo "Following tests failed:"
    for i in $FAILED; do
        echo " * $i"
    done
    exit 1
fi
rm -f "$DB"
exit 0
