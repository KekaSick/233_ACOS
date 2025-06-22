#!/bin/bash

sum() {
    local total=0 tmp
    for arg in "$@"; do
        if expr "$arg" + 0 >/dev/null 2>&1; then
            tmp=$(expr "$total" + "$arg" 2>/dev/null)
            if [ $? -ne 0 ]; then
                echo 0
                return
            fi
            total=$tmp
        else
            echo 0
            return
        fi
    done
    echo "$total"
}

read line1
read line2

sum1=$(sum $line1)
sum2=$(sum $line2)

if [ "$sum1" -eq "$sum2" ]; then
    echo "Equal"
else
    echo "Not equal"
fi 