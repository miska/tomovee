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
select imdb_id from movies order by imdb_id;
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
export test1="$RUN_DIR"/files/test1
export test2="$RUN_DIR"/files/test2
mkdir -p "$test1"/inside
mkdir -p "$test2"
cp fixtures/movies/breakdance.avi "$test1"/a.avi
cp fixtures/movies/breakdance.avi "$test1"/inside/whatever.avi
cp fixtures/movies/breakdance.avi "$test2"/c.avi
cp fixtures/movies/breakdance.avi "$test2"/d.avi
echo 'a' >> "$test2"/d.avi

export test3="$RUN_DIR"/nfo/test3
export test3_inside="$test3"/first/overwrite/file
mkdir -p "$test3_inside"
cp fixtures/movies/breakdance.avi "$test3_inside"/whatever.avi
cat > "$test3_inside"/whatever.NFO << EOF
URL: http://www.imdb.com/title/tt0086999
__Audio___: _Abkhazian_
sUbTitles.... : -= Zaza & Zulu =-
EOF
cat > "$test3"/first/overwrite/whatever.nFo.force << EOF
URL: http://www.imdb.com/title/tt0086998
__Audio___: _English_
sUbTitles.... : -= Empty =-
EOF
cat > "$test3"/first/whatever.nfo << EOF
URL: http://www.imdb.com/title/tt0086997
__Audio___: _Abkhazian_
sUbTitles.... : -= Zaza & Zulu =-
EOF
find "$RUN_DIR"/{files,nfo} -exec touch -d "2014-09-26 11:00:00 CEST" \{\} \;

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
    expr match $SCRIPT .\*\\.sh > /dev/null || SKIP=true
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
