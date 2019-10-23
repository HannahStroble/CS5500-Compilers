inputs=`ls sample_input --ignore-backups`

for i in $inputs; do
    ./driver < sample_input/$i > ./actual_output/$i.out

    diff ./actual_output/$i.out ./sample_output/$i.out > ./final_output/$i --left-column --ignore-space-change --ignore-case --side-by-side --ignore-blank-lines
done

display=`ls final_output --ignore-backups`
for file in $display; do
    echo ===============================================================================;
    echo $file;
    echo ===============================================================================;
    cat ./final_output/$file;
done