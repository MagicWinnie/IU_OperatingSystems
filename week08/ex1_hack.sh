#!/bin/bash

pid=$(cat /tmp/ex1.pid)

start=$(cat /proc/$pid/maps | grep heap | cut -d ' ' -f 1 | cut -d '-' -f 1)
end=$(cat /proc/$pid/maps | grep heap | cut -d ' ' -f 1 | cut -d '-' -f 2)

pass_raw=$(sudo xxd -s 0x$start -l $((0x$end - 0x$start)) /proc/$pid/mem | grep pass:)

echo $pass_raw | rev | cut -d ' ' -f 1 | rev | head -c 13
echo ""

kill -n SIGKILL $pid

rm /tmp/ex1.pid
