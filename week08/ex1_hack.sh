#!/bin/bash

pass_regex="pass:........"

pid=$(cat /tmp/ex1.pid)
if [ -z "$pid" ]
then
    echo "PID file is empty. Check if the program is running..."
    exit 1
fi

addresses=$(cat /proc/$pid/maps | grep heap | cut -d ' ' -f 1)

start=$(echo $addresses | cut -d '-' -f 1)
end=$(echo $addresses | cut -d '-' -f 2)

pass_raw=$(sudo xxd -s 0x$start -l $((0x$end - 0x$start)) /proc/$pid/mem | grep $pass_regex)

echo "Line containing the password: \"$pass_raw\""
echo $pass_raw | grep --only-matching -e $pass_regex
echo ""

kill -n SIGKILL $pid

rm /tmp/ex1.pid
