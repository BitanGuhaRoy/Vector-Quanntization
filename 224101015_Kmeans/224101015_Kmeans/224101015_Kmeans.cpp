// km.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <float.h>
#include "conio.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define codebooksize 8 // codebooksize is defined
#define vectorSize 12  // dimension is defined here
#define delta 0.00001
// This function calculates the absolute value of a number a
long double absolute(long double a)
{
    // if(a>0 ) return a;
    // else
    // return (-1.0)*a;
    return (a > 0) ? a : (-1.0) * a;
}
/*
After calculating distortion this function is called it
updates the codebook by calculating centroid of each region

*/
void codebookupdatenew(long double codebook[codebooksize][vectorSize], int region[])
{
    // univector array is used to temporary store the read array from file
    long double univvector[vectorSize], number;
    int i, j, d = 0;
    for (i = 0; i < vectorSize; i++)
        univvector[i] = 0.0;
    // This below .txt file is created in main function to store data in .txt form
    FILE *fp = fopen("Universe_TXT.txt", "r");
    // handle the case if file isn't opened
    if (fp == NULL)
        printf("File not opened \n");
    int ind = 0;
    // this is the updated codebook that we found by updating current codebook
    long double newcodebook[codebooksize][vectorSize];
    for (i = 0; i < codebooksize; i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            // newcodebook is initialized with 0.0 to avoid garbage value
            newcodebook[i][j] = 0.0;
        }
    }
    int regionsize[codebooksize]; // this array stores the regionsize of each region
    for (i = 0; i < codebooksize; i++)
        regionsize[i] = 0; // regionsize is initialized with 0

    long double distortion = 0.0, totalDistortion = 0.0;
    while (fscanf(fp, "%Lf", &number) != EOF)
    {

        univvector[ind++] = number; // values are read from file and stored in the temporary array univvector
        // if ind >= vectorsize it says we found one array

        if (ind >= vectorSize)
        { // regionsize is incremente by 1 each time we hit that region
            regionsize[region[d]]++;
            for (i = 0; i < vectorSize; i++)
                // summing the vectors of a region and storing that in newcodebook
                newcodebook[region[d]][i] += univvector[i];
            d++; // d says the vector index in the universe file

            ind = 0; // ind is set to 0
        }
    }
    // this below nested for loop is used to update the newcodebook with the centroid vectors of each region
    for (i = 0; i < codebooksize; i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            if (regionsize[i] != 0) // divided by the regionsize to get the centroid
                newcodebook[i][j] = newcodebook[i][j] / regionsize[i];
            else
                newcodebook[i][j] = 0;
        }
    }
    // this below nested loop copies the newcodebook into codebook for further computation
    for (i = 0; i < codebooksize; i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            codebook[i][j] = newcodebook[i][j]; // codebook is updated
        }
    }

    fclose(fp); // file is closed
}
// this function calculates the thokura distance between two vectors
long double calculateThokura(long double a[], long double b[])
{
    long double w[] = {1.0, 3.0, 7.0, 12.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};
    long double sum = 0.0; // distance is initialized
    int i;
    for (i = 0; i < vectorSize; i++)
    {
        sum += w[i] * (a[i] - b[i]) * (a[i] - b[i]);
    }

    return sum; // distance is returned
}

// this function calculates a vector belongs to which region and after mapping minimum distortion is updated
int findcluster(long double univ[], long double codebook[codebooksize][vectorSize], long double *distortionret)
{
    // initialized with region =-1
    int i, j, region = -1;
    long double dist = 0, mindist = 99999.0, distortion = 0;
    for (i = 0; i < codebooksize; i++)
    {
        // it returns the  thokura distance between a vector univ and the ith vector of codebook
        dist = calculateThokura(univ, codebook[i]);
        // if current distance is smaller than min dist found so far, just update the min distance

        if (dist < mindist)
        {
            mindist = dist;
            // region of the vector is also updated
            region = i;
        }
    }
    // distortion of the vector is the min distance and that is updated in distortion
    *distortionret = mindist;
    // return the region in which it is mapped
    return region;
}
/*this function updates the region array
  for each vector present in the universe data this function find its proper cluster
  keeping in mind about the nearest neighbour method
*/
long double classification(long double codebook[codebooksize][vectorSize], int region[])
{
    // univector holds the temporary vector that is read from file
    long double univvector[vectorSize], number;
    int i, j, d = 0; // d denotes the index of the vector read from file
    for (i = 0; i < vectorSize; i++)
        univvector[i] = 0.0; // temporary array is initialized with 0
    FILE *fp = fopen("Universe_TXT.txt", "r");
    // handels the case if file isn't opened
    if (fp == NULL)
        printf("File not opened\n");
    int ind = 0;
    // distortiuon variable holds the distortion value of each vector
    long double distortion = 0.0, totalDistortion = 0.0;
    while (fscanf(fp, "%Lf", &number) != EOF)
    {

        univvector[ind++] = number; // values are stored in the univvector
        // ind >= vectorsize it sayz we found one vector
        if (ind >= vectorSize)
        {
            /*
            below line makes a function call and finds the min distortion of the temporary vector
            and corresponding region is udated with the vector index d
            */
            region[d] = findcluster(univvector, codebook, &distortion);
            // totalDistortion is updated by adding current distortion
            totalDistortion += distortion;

            d++; // d is updated

            ind = 0;
        }
    }

    totalDistortion /= d; // total distortion is divided by the universe size
    codebookupdatenew(codebook, region);

    return totalDistortion; // totalDistortion of the universe with respect to the passed codevector is returned
}
// this function is the driver function of kmeansalgorithm
long double kmeansalgo(long double codebook[codebooksize][vectorSize], int region[])
{

    long double ret = classification(codebook, region);
    // distortion value is retunred from classification function and that is returned from the current function
    return ret;
}
// This function adds the initial vector in the codevector
void addintocodebook(long double univ[], long double codebook[codebooksize][vectorSize])
{
    static int ind = 0; // it keeps on updating the index
    int i;
    for (i = 0; i < vectorSize; i++)
    {
        codebook[ind][i] = univ[i];
    }

    ind++;
}

int _tmain(int argc, _TCHAR *argv[])

{
	
    int region[100000]; // this array stores the region number as region[i] of the universe vector i

    int i;
    for (i = 0; i < 100000; i++)
    {

        region[i] = -1; //region is initialized with -1
        
    }
    FILE *fp = fopen("Universe.csv", "r");
    // If file isn't opened
    if (fp == NULL)
    {
        printf("File Not opened\n");

        return -1;
    }
    char *sp = NULL;
    long double number;
    long double l;
    char line[200];
    int c = 0, ind = 0; // c denotes the row and ind denotes the column of the codevector
                        // this below 2D array stores the codebook
    long double codebook[codebooksize][vectorSize];
    // this below file is created to store the universe vectors in the form of .txt file
    FILE *fpw = fopen("Universe_TXT.txt", "w");
    // in one time one line is read
    while (fscanf(fp, "%s", line) != EOF)
    {

        char *ptr;
        ptr = strtok(line, ",");
        // while ptr isn't null keep on writting the lines
        while (ptr)
        { // write the line in universe.txt file
            fprintf(fpw, "%s ", ptr);
            ptr = strtok(NULL, ",");
        }
        fputs("\n", fpw);
    }
    // writting part is done , file is closed
    fclose(fpw);
    // this vector holds the vectores read from the file
    long double univvector[vectorSize];
    int j;
    for (i = 0; i < vectorSize; i++)
        univvector[i] = 0.0; // univvector is initialized
    FILE *fpr = fopen("Universe_TXT.txt", "r");
    // if file isn't opened
    if (fpr == NULL)
        printf("File not opened \n");
    ind = 0;
    int da = 0;
    // each value is read from the file
    while (fscanf(fpr, "%Lf", &number) != EOF)
    {

        univvector[ind++] = number; // values are stored into univvector
                                    // if ind>= vectorsize then it says we got an array
        if (ind >= vectorSize)
        {
            da++;
            if (c < codebooksize)
            { // vector is added into codebook
                addintocodebook(univvector, codebook);

                c++;
            }

            ind = 0;
        }
    }
    // this variable counts the index of the codebook
    int le = 1;
    // 1st codebook is printed
    printf("----------------%d number codebook is printed --------------\n", le);
    for (i = 0; i < codebooksize; i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            printf("%Lf ", codebook[i][j]); // codebook is printed
        }
        printf("\n");
    }
    // codebook index is updated
    le++;
    // distortion is calculated using the kmeans algorithm
    long double distortion = kmeansalgo(codebook, region);

    printf("\n----------------%d number codebook is printed --------------\n", le);
    // codebook is printed
    for (i = 0; i < codebooksize; i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            printf("%Lf ", codebook[i][j]); // codebook is printed
        }
        printf("\n");
    }

    long double olddistortion = distortion; // olddistortion is updated

    // newdistortion is calculated using the function kmeansalgo and codebook is updated 
    long double newdistortion = kmeansalgo(codebook, region);
    le++;
    printf("\n----------------%d number codebook is printed --------------\n", le);
    for (i = 0; i < codebooksize; i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            printf("%Lf ", codebook[i][j]);  //codebook is updated 
        }
        printf("\n");
    }

   //if two consequtive codebook have distortion difference lesser than delta then break the loop
    while (absolute(olddistortion - newdistortion) > delta)
    {

        olddistortion = newdistortion;
        newdistortion = kmeansalgo(codebook, region); //distortion is returned and codebook is updated 

        le++;
        //below part prints codebook 
        printf("\n----------------%d number codebook is printed --------------\n", le);
        
        for (i = 0; i < codebooksize; i++)
        {
            for (j = 0; j < vectorSize; j++)
            {
                printf("%Lf ", codebook[i][j]); //codebook is printed 
            }
            printf("\n");
        }

        // printf("%d - d %Lf ", le, distortion);
    }
    //This below line prints the final distortion difference and the final codebook number 
    printf("Total number of codebook iteration before termination %d with final distortion %Lf", le, (olddistortion - newdistortion));
    getchar();
    return 0;
}
