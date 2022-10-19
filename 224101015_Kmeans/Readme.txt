*Input Files*
	-For Kmeans and LBG
		--In the same working directory there is a .csv file Universe.csv which is the input training data

*Software *
	-Windows 11 OS on Mac M1 machine using Virtual Machine(Parallels)
*Instructions to run the code*
	-Expected to run in Visual Studio 2010
	-Code requires one command line argument the input file name as input Universe.csv
*Logic Used in the Code*
    -For Kmeans
		initialize codebook with random k vectors from Universe
		m = 0
		do 
			Classification using Nearest Neighbour selection Rule
			Centroid Update 
			compute distortion
			if(m>0)
				if( |prev-distortion - distortion| <= delta) 
					break
			prev-distortion = distortion
			m++;
		while(1)
		return the final codebook
	