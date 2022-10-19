// LBGNew.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "conio.h"
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define codebooksize 8
#define vectorSize 12
#define delta 0.00001
// this function splits the codebook and doubles its size
void codebooksplit(long double codebook[codebooksize][vectorSize], int *size)
{
    /*
    l denotes current size of codebook
    i and j is used inside for loop

     */
    int l = (*size), i, j;
    if (l == codebooksize)
        return;
    int d = 0;
    long double vector[vectorSize]; // every vector of current codebook is temporarily stored here
    for (i = 0; i < vectorSize; i++)
        vector[i] = 0.0;
    long double newcodebook[9][vectorSize]; // new codebook is taken it will store the new codevectors after splitting
    for (i = 0; i < l; i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            // each vector from current codebook is taken into vector array
            vector[j] = codebook[i][j];
        }
        for (j = 0; j < vectorSize; j++)
        {
            // one new vector created and added into the newcodebook
            newcodebook[d][j] = vector[j] * (1 + 0.03);
        }
        d++;
        for (j = 0; j < vectorSize; j++)
        { // one new vector created and added into the newcodebook
            newcodebook[d][j] = vector[j] * (1 - 0.03);
        }
        d++;
    }

    for (i = 0; i < 2 * l; i++)
    {
        for (j = 0; j < vectorSize; j++)
        { // newcodebook is copied into the codebook
            codebook[i][j] = newcodebook[i][j];
        }
    }
}
// This function calculates the absolute value of a number a
long double absolute(long double a)
{

    return (a > 0) ? a : (-1.0) * a;
}

/*
After calculating distortion this function is called it
updates the codebook by calculating centroid of each region

*/
void codebookupdatenew(long double codebook[codebooksize][vectorSize], int region[], int *size)
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
    long double newcodebook[codebooksize][vectorSize]; // this is the updated codebook that we found by updating current codebook
    for (i = 0; i < (*size); i++)
    {
        for (j = 0; j < vectorSize; j++)
        {
            newcodebook[i][j] = 0.0; // newcodebook is initialized with 0.0 to avoid garbage value
        }
    }
    int regionsize[codebooksize]; // this array stores the regionsize of each region

    for (i = 0; i < (*size); i++)
        regionsize[i] = 0; // regionsize is initialized with 0

    long double distortion = 0.0, totalDistortion = 0.0;
    while (fscanf(fp, "%Lf", &number) != EOF)
    {

        univvector[ind++] = number; // values are read from file and stored in the temporary array univvector
        // if ind >= vectorsize it says we found one array
        if (ind >= vectorSize)
        {
            // regionsize is incremente by 1 each time we hit that region
            regionsize[region[d]]++;
            for (i = 0; i < vectorSize; i++)
                // summing the vectors of a region and storing that in newcodebook
                newcodebook[region[d]][i] += univvector[i];
            d++; // d says the vector index in the universe file

            ind = 0; // ind is set to 0
        }
    }

    // this below nested for loop is used to update the newcodebook with the centroid vectors of each region
    for (i = 0; i < (*size); i++)
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
    for (i = 0; i < (*size); i++)
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
    long double w[] = {1.0, 3.0, 7.0, vectorSize.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};
    long double sum = 0.0; // distance is initialized
    int i;
    for (i = 0; i < vectorSize; i++)
    {
        sum += w[i] * (a[i] - b[i]) * (a[i] - b[i]); // distance is added
    }

    return sum; // distance is returned
}
// this function calculates a vector belongs to which region and after mapping minimum distortion is updated
int findcluster(long double univ[], long double codebook[codebooksize][vectorSize], long double *distortionret, int *size)
{
    // initialized with region =-1
    int i, j, region = -1;
    long double dist = 0, mindist = 99999.0, distortion = 0;
    for (i = 0; i < (*size); i++)
    { // it returns the  thokura distance between a vector univ and the ith vector of codebook
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
long double classification(long double codebook[codebooksize][vectorSize], int region[], int *size)
{
    // univector holds the temporary vector that is read from file
    long double univvector[vectorSize], number;
    int i, j, d = 0; // d denotes the index of the vector read from file
    for (i = 0; i < vectorSize; i++)
        univvector[i] = 0.0; // temporary array is initialized with 0
    FILE *fp = fopen("Universe_TXT.txt", "r");
    // handels the case if file isn't opened
    if (fp == NULL)
        printf("File not opened \n");
    int ind = 0;
    // distortiuon variable holds the distortion value of each vector
    long double distortion = 0.0, totalDistortion = 0.0; // total distortion is found by adding all distortions

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
            region[d] = findcluster(univvector, codebook, &distortion, size);
            // totalDistortion is updated by adding current distortion
            totalDistortion += distortion;

            d++; // d is updated

            ind = 0;
        }
    }

    totalDistortion /= d; // total distortion is divided by the universe size
    codebookupdatenew(codebook, region, size);

    return totalDistortion; // totalDistortion of the universe with respect to the passed codevector is returned
}

// this function is the driver function of kmeansalgorithm
long double kmeansalgo(long double codebook[codebooksize][vectorSize], int region[], int *size)
{

    long double ret = classification(codebook, region, size);
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
   
    int region[10000]; // this array stores the region number as region[i] of the universe vector i
    int i;
    for (i = 0; i < 10000; i++)
    {

        region[i] = -1; // region vector is initialized
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
    int c = 0, ind = 0;
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
        printf("File Not Opened \n");
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
            if (c < 1)
            {
                // vector is added into codebook
                addintocodebook(univvector, codebook);

                c++;
            }

            ind = 0;
        }
    }

    int le = 0;

    int sizeofcodebook = 1;   // size of codebook is initialized
    long double netdis = 0.0; // netdis is initialized with 0.0

    // while sizeofcodebbok doesn't match with codebooksize
    while (sizeofcodebook <= codebooksize)
    {
        for (i = 0; i < 10000; i++)
        {

            region[i] = -1;
        }
        // distortion initialized with 1st call
        long double distortion = kmeansalgo(codebook, region, &sizeofcodebook);
        // distortion is stored as olddistortion
        long double olddistortion = distortion;

        // newdistortion is updated now
        long double newdistortion = kmeansalgo(codebook, region, &sizeofcodebook);

        // if difference of distortion between two consqutive codevector is lessthan delta then break the loop
        while (absolute(olddistortion - newdistortion) > delta)
        {

            olddistortion = newdistortion;
            // newdistortion is updated from the kmeansalgo
            newdistortion = kmeansalgo(codebook, region, &sizeofcodebook);
            // netdis stores the difference between two codevector
            netdis = absolute(olddistortion - newdistortion);
        }

        printf("code book of size %d --\n", sizeofcodebook);
        // codebook is printed with the size
        for (i = 0; i < sizeofcodebook; i++)
        {
            for (j = 0; j < vectorSize; j++)
            {
                printf("%Lf ", codebook[i][j]);
            }
            printf("\n");
        }
        // now the codebook is splitted
        codebooksplit(codebook, &sizeofcodebook);
        // size is doubled
        sizeofcodebook *= 2;
    }
    // final distortion is printed
    printf("Final Distortion %Lf ", netdis);

    getch();
    return 0;
}
