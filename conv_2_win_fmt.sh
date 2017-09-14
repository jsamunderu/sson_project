#!/bin/bash
find . -type f -name *.h -o -name *.cpp -exec unix2dos {} \; > /dev/null 2>&1
