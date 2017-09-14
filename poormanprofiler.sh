#!/bin/bash

gdb -ex "set pagination 0" -ex "thread apply all bt" --batch -p $1
