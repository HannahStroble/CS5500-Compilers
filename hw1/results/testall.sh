#! /bin/bash

# generate output files
cd hw1_sample_input/
for filename in *.txt; do
	python3 ../reinbolth.py "$filename" > ../processing/"$filename".out
	echo "$filename"
done


# compare all files and make folder
cd ../
cd hw1_expected_output/
for goodoutfile in *.out; do
	cd ../
	cd processing/
	for myoutfile in *.out; do
		diff "$myoutfile" "$goodoutfile" --ignore-space-change --side-by-side --ignore-case --ignore-blank-lines > ../results/"$myoutfile".txt
	done
done
cd ../

# let hte user know it is complete
echo "Results generated in results/ folder."
