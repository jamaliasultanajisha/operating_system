#!/bin/bash

cd 1505095
mv SubmissionsAll.zip ../
mv CSE_322.csv ../
cd ..
unzip -q SubmissionsAll.zip
mv SubmissionsAll.zip 1505095	
mv CSE_322.csv SubmissionsAll
cd SubmissionsAll						
localfile=$(ls)

touch submit.txt
touch all.txt
touch absents.txt
cat CSE_322.csv | while read line
do
		flag="0"
		line2=$(echo $line | cut -d'"' -f2)    # roll pailam
		line2=$(echo $line2 | cut -c1-7)		
		(IFS='_.'; for stuword in $localfile;
					do
					if [[ ("$line2" = "$stuword") ]]; then 			
					echo $stuword>>submit.txt
					
					flag="1"
					fi;
					done)
		if [[ ("$flag" = "0") ]];then
			echo $line2>>all.txt		
		fi;

done

grep -v -F -x -f submit.txt all.txt > absents.txt

#mv CSE_322.csv ../
mkdir output 
cd output
mkdir extra
cd ..

IFS=$'\n'       
for i in  $localfile  
do
    if [[ ($i =~ ^[^_]+_[0-9]+_assignsubmission_file_[0-9]{7}.*$) ]]; then
		if [[ ($i  =~ ".zip") ]]; then
			cd output
			#mkdir tempdir
			if [ ! -d "$tempdir" ]; then
  				# Control will enter here if $DIRECTORY doesn't exist.
  				mkdir tempdir
  				ech "hello 3333333333333"
			fi
			if [ -d "$tempdir" ]; then
  				# Control will enter here if $DIRECTORY exists.
  				rm -r tempdir; mkdir tempdir
  				echo "hellooooo"
			fi
			cd ..
	
			unzip -q $i -d "output/tempdir/"
			#echo $i
			cd output/tempdir
	
			filename=$(ls)
			fileNum=$(ls -1 | wc -l)
		
			rollNo="$(echo $i | cut -d'_' -f5)"
			rollNo="$(echo $rollNo | cut -d'.' -f1)"
			#echo roll $rollNo
			if [[ ($fileNum = "1") && ($filename = $rollNo) ]]; then
					#echo "full 10"
					cd ..
					(echo $rollNo "  10")>>marks.txt
					cd tempdir	
					mv $filename ../
					cd ..

			elif [[ ($fileNum = "1") && ($filename =~ $rollNo) ]];then
					cd ..
					#echo "half 5"
					(echo $rollNo "  5")>>marks.txt				
					cd tempdir
					mv $filename $rollNo
					mv $filename ../
					cd ..

			elif [[ ($fileNum = "1") ]];then
					#echo " 0"
			#		echo $rollNo
					mv $filename $rollNo
					mv $filename ../
					cd ..
					(echo $rollNo "  0")>>marks.txt
								
			else
					#echo " 0"
			#		echo $rollNo
					cd ..
					mv tempdir $rollNo
					mkdir tempdir
					(echo $rollNo "  0")>>marks.txt				
			fi
			cd ..
			rm $i
			cd output	
		#rm -r tempdir	
		fi
	
	elif [[ ($i  =~ "*.zip") ]]; then
		cd ..		
		echo $i		
		zipName="$(echo $i | cut -d'_' -f1)"
#		echo zipName:  $zipName
		numOfnames=$(grep $zipName CSE_322.csv | wc -l)
		if [[ $numOfnames = "1" ]];then
			roll=$(grep $zipName CSE_322.csv)		
			
			roll=$(echo $roll | cut -c3-9)	#"  0405035"
#			echo rolllllll $roll
			unzip -q $i -d "output/tempdir/"
			cd output
			(echo $roll "  0")>>marks.txt
			mv tempdir $roll
			mkdir tempdir
			cd ..
			(echo $roll )>>submit.txt
			cd output
		
		elif [[ $numOfnames = "0" ]];then
			unzip -q $i -d "output/tempdir/"
			cd output
#			echo zipname $zipName
			mv tempdir $zipName
			mv $zipName extra
			mkdir tempdir
		else
			echo "more than 2 names"	
		fi
		
	fi

#	echo $localfile
done

rm -r tempdir
cd ..
pwd
mv absents.txt output
cd output
cd ..
mv CSE_322.csv ../
cd ..
pwd
ls -lS
mv CSE_322.csv 1505095