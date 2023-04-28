PASS1_IN=test/part1/pass1/in
PASS1_OUT=test/part1/pass1/out
PASS1_REF=test/part1/pass1/ref
PASS2_OUT=test/part1/pass2/out
PASS2_REF=test/part1/pass2/ref
./riscv assembler $PASS1_IN/simple.s -d $PASS1_OUT/simple.data -s $PASS1_OUT/simple.symbol -o $PASS1_OUT/simple.original -b $PASS2_OUT/simple.basic -t $PASS2_OUT/simple.text
diff $PASS1_OUT/simple.data $PASS1_REF/simple.data
diff $PASS1_OUT/simple.symbol $PASS1_REF/simple.symbol
diff $PASS1_OUT/simple.original $PASS1_REF/simple.original
diff $PASS2_OUT/simple.basic $PASS2_REF/simple.basic
diff $PASS2_OUT/simple.text $PASS2_REF/simple.text