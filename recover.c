/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define BLOCK_SIZE  512

unsigned const char jpeg[] = {0xff, 0xd8, 0xff, 0xe0};

int jpegSeqFound(unsigned char block[]);
void writeFoundJpeg(unsigned char block[], FILE * infilePtr, FILE * outfilePtr);


int main(int argc, char* argv[])
{
    // ensures proper usage
    if(argc != 2)
    {
        printf("Usage: ./recover infile");
        return 1;
    }
    
    // open infile
    FILE * infilePtr = fopen(argv[1], "r");
    if(infilePtr == NULL)
    {
        printf("Could not open infile\n");
        return 2;
    }
        
    // find the block where the 1st jpeg begins
    unsigned char block[BLOCK_SIZE/sizeof(char)];
    while(jpegSeqFound(block) == 0)
    {
        if(fread(block, BLOCK_SIZE/sizeof(char), 1, infilePtr) != 1)
            return 3;
    }
    
    int i = 1;
    char index[8];
    FILE * outfilePtr = NULL;
        
  while(jpegSeqFound(block) == 1)
    {
        // creates name for output file
        sprintf(index, "%.3i.jpg", i);

        // open output file
        outfilePtr = fopen(index, "w");
        if(outfilePtr == NULL)
        {
            printf("Could not open outfile %s\n", index);
            return 4;
        }
        
        writeFoundJpeg(block, infilePtr, outfilePtr);
        fclose(outfilePtr);
        i++;
    }
   
    fclose(infilePtr);

    
    return 0;
}


// checks if jpeg sequence if found at the beginning of the block
int jpegSeqFound(unsigned char block[])
{
    if(block[0] == jpeg[0]  && block[1] == jpeg[1] 
        && block[2] == jpeg[2] && (block[3] == jpeg[3] || block[3] == 0xe1))     
        return 1;
    else
        return 0;   
}

// reads from infile and writes to outfile block by block
void writeFoundJpeg(unsigned char block[], FILE * infilePtr, FILE * outfilePtr)
{
    fwrite(block, BLOCK_SIZE*sizeof(char), 1, outfilePtr);
    
    // checks whether EOF was reached for infile
    if(fread(block, BLOCK_SIZE*sizeof(char), 1, infilePtr) != 1)   
        return;
        
    if(jpegSeqFound(block) == 0)
        writeFoundJpeg(block, infilePtr, outfilePtr);
}
