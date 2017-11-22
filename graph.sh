#! /bin/bash

function f(){
    dot -Tpng $1.dot -o $1.png
}

f $1
