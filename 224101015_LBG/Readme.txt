*Input Files*
	-For Kmeans and LBG
		--In the same working directory there is a .csv file Universe.csv which is the input training data
*Software *
	-Windows 11 OS on Mac M1 machine using Virtual Machine(Parallels)
*Instructions to run the code*
	-Expected to run in Visual Studio 2010
	-Code requires one command line argument the input file name as input Universe.csv
*Logic Used in the Code*
    LBG --

1. centroid is found from the universe 

2. split each centroid and Distortion = 999999.0 (large value)
   size is multiplied with 2 

3. vectors are classified into corresponding regions 

4. Again centroids are found for each cluster 

5. compute current distortion 

6. if the change in two consequtive distortion is lesser than delta(0.0001) 
   then we found the stable codevector now go to step 2 
7. else olddistortion is updated as current distortion and it goes to step 2

when we get our desired codebook then the algorithm is terminated 