#!/bin/bash

source "$2/accept-setup.sh"

fixtures="$TESTFIX/svg"

# default 

for base in Gradients_01 
do
    svg="$base.svg"
    cpt="$base.cpt"
    assert_raises "./svgx -t cpt -o $cpt $fixtures/$svg" 0
    assert "equal-cpt $cpt accept/$cpt" true
    rm -f "$cpt"
done

# list

for base in eyes 
do
    svg="$base.svg"
    txt="$base.txt"
    assert_raises "./svgx -l $fixtures/$svg > $txt" 0
    assert "diff $txt accept/$txt | wc -l" 0
    rm -f "$txt"
done

# named

for base in eyes 
do
    svgsrc="$base.svg"
    for name in $(./svgx -l "$fixtures/$svgsrc")
    do
	cpt="$base-$name.cpt"
	assert_raises "./svgx -t cpt -s $name -o $cpt $fixtures/$svgsrc" 0
	assert "equal-cpt $cpt accept/$cpt" 'true'
	rm -f "$cpt"

	ggr="$base-$name.ggr"
	assert_raises "./svgx -t ggr -s $name -o $ggr $fixtures/$svgsrc" 0
	assert "equal-ggr $ggr accept/$ggr" 'true'
	rm -f "$ggr"

	inc="$base-$name.inc"
	assert_raises "./svgx -t pov -s $name -o $inc $fixtures/$svgsrc" 0
	assert "equal-inc $inc accept/$inc" 'true'
	rm -f "$inc"

	gpf="$base-$name.gpf"
	assert_raises "./svgx -t gnuplot -s $name -o $gpf $fixtures/$svgsrc" 0
	assert "equal-gpf $gpf accept/$gpf" 'true'
	rm -f "$gpf"
       
	c3g="$base-$name.c3g"
	assert_raises "./svgx -t css3 -s $name -o $c3g $fixtures/$svgsrc" 0
	assert "equal-c3g $c3g accept/$c3g" 'true'
	rm -f "$c3g"

	psp="$base-$name.PspGradient"
	assert_raises "./svgx -t psp -s $name -o $psp $fixtures/$svgsrc" 0
	assert "equal-psp $psp accept/$psp" 'true'
	rm -f "$psp"

	sao="$base-$name.sao"
	assert_raises "./svgx -t sao -s $name -T255/0/0 -o $sao $fixtures/$svgsrc" 0
	assert "equal-sao $sao accept/$sao" 'true'
	rm -f "$sao"

	png="$base-$name.png"
	assert_raises "./svgx -t png -s $name -o $png $fixtures/$svgsrc" 0
	assert "equal-png $png accept/$png" 'true'
	rm -f "$png"

	svg="$base-$name.svg"
	assert_raises "./svgx -t svg -s $name -o $svg $fixtures/$svgsrc" 0
	assert "equal-svg $svg accept/$svg" 'true'
	rm -f "$svg"
    done
done

# mad ids :

testdir='mad-id-filenames'
txt='mad-id-filenames.txt'
mkdir -p "$testdir"
assert_raises "./svgx -a -o $testdir $fixtures/mad-ids.svg" 0
assert_raises "ls -1 $testdir > $txt" 0
assert "equal-txt $txt accept/$txt" 'true'
rm -f $txt $testdir/*
rmdir $testdir

# create a backtrace file
bt="backtrace.json"
malformed="$fixtures/malformed.svg"
btargs="--backtrace-file $bt --backtrace-format json"
assert_raises "./svgx $btargs -a -o nope.svg $malformed" 1
assert "equal-json-backtrace $bt accept/$bt" 'true'
rm -f $bt

source accept-teardown.sh
