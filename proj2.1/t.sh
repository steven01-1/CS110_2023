filename=$1
cd tests
echo $filename
python3 binary_to_hex_regfile.py reference_output/regfile-${filename}-ref.out > regfile-${filename}-ref.out
python3 binary_to_hex_regfile.py student_output/regfile-${filename}-student.out > regfile-${filename}-student.out
echo "done"
