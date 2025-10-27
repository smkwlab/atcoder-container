#! /bin/sh
if [ "$1" -gt 1024 ]; then
    stack_size=1024
else
    stack_size="$1"
fi
work_dir="${2:-/judge}"
java -Xss"$stack_size"M -DONLINE_JUDGE=true -cp "$work_dir"/ac_library.jar:"$work_dir": Main
