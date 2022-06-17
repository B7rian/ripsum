#!/bin/env bash

set -e

TMPDIR=`mktemp -d`
RIPSUM=$PWD/ripsum
SRC=$PWD

#
# generate_and_compare: Generate checksums using both find+sha256sum and 
# ripsum and compare the output (stderr, stdout, and return value)
#
function generate_and_compare() {
	time find $1 -type f -exec sha256sum $2 {} + 2> ref_gen.err | sort -k 2 > ref_gen.out
	RET1=$?
	time $RIPSUM $1 $2 2> ripsum_gen.err | sort -k 2 > ripsum_gen.out
	RET2=$?

	diff ref_gen.out ripsum_gen.out
	diff ref_gen.err ripsum_gen.err
	if [ $RET1 != $RET2 ]; then
		echo RET1=$RET1 RET2=$RET2
	fi
}


#
# check_and_compare: Check checksums in a file using both find+sha256sum 
# and ripsum and compare the output (stderr, stdout, and return value)
#
function check_and_compare() {
	time sha256sum $1 2> ref_check.err | sort -k 2 > ref_check.out 
	RET1=$?
	time $RIPSUM $1 2> ripsum_check.err | sort -k 2 > ripsum_check.out
	RET2=$?

	diff ref_check.out ripsum_check.out
	diff ref_check.err ripsum_check.err
	if [ $RET1 != $RET2 ]; then
		echo RET1=$RET1 RET2=$RET2
	fi

}

set -v -x 
cd $TMPDIR

# Generate checksums on $SRC and compare the outputs
generate_and_compare "$SRC" ""

# Check the checksums just generated and compare the outputs
cp ref_gen.out checksums.txt
check_and_compare "-c checksums.txt"

# Inject a bad checksum (most of the time) on line 2 of the checksum file
sed '2s/^......../ffffffff/' < checksums.txt > badsum.txt
check_and_compare "-c badsum.txt"

# Inject another bad checksum on line 5
sed '5s/^......../ffffffff/' < badsum.txt > badsums.txt
check_and_compare "-c badsums.txt"

# Inject a bad character into line 3 of the checksum file
sed '3s/^./x/' < checksums.txt > badchar.txt
check_and_compare "-c badchar.txt"

# Inject another bad character on line 4
sed '4s/^./x/' < badchar.txt > badchars.txt
check_and_compare "-c badchars.txt"

# Make the checksum on line 4 too long
sed '4s/ /f /' < checksums.txt > toolong.txt
check_and_compare "-c toolong.txt"

# Make the checksum on line 9 too short
sed '9s/. / /' < checksums.txt > tooshort.txt
check_and_compare "-c tooshort.txt"

# Spaces at the beginning of the line
sed '9s/^/     /' < checksums.txt > spaces.txt
check_and_compare "-c spaces.txt"

cd -
rm -rf $TMPDIR 	# If TMPDIR is not set for some reason, this won't do anything
