outpng () {
    filename=`echo $1 | sed 's/\.[^\.]*$//'`
    filename=$filename".png"
    dot -Tpng $1 -o $filename
    echo "Generate "$filename
}

outpng $1
