I/* 
   @ author Yongxin Yang
   @ since 4/23/2020 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX_SIZE 1048576

//function phrototypes
unsigned int readFile(unsigned char buffer[]);
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes);
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes);
unsigned int charPacker(unsigned char, unsigned char, unsigned  char, unsigned char);
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount);
void computeMessageDigest(unsigned int message[], unsigned int blockCount);
unsigned int f(unsigned int t, unsigned int B, unsigned int C, unsigned int D);
unsigned int K(unsigned int t);
unsigned int S(unsigned int X, int n);

//to turn on debug printing
 int turnOnPrinting = 0;

int main(){
  unsigned char buffer[MAX_SIZE];
  unsigned int message[MAX_SIZE];
  int size = 0; 
  int block = 0;
  
  size = readFile(buffer);
  block = calculateBlocks(size);
  //test size and block
  //printf("the size is %d\n",size);
  //printf("the number of blocks is %d\n", block);
   
  //test buffer
  /*
  printf("buffer[0] = %c = 0x%02x\n", buffer[0],buffer[0]);
  printf("buffer[1] = %c = 0x%02x\n", buffer[1],buffer[1]);
  printf("buffer[2] = %c = 0x%02x\n", buffer[2],buffer[2]);
  printf("buffer[3] = %c = 0x%02x\n", buffer[3],buffer[3]);
  printf("buffer[55] = %c = 0x%02x\n", buffer[55],buffer[55]);
  printf("buffer[56] = %c = 0x%02x\n", buffer[56],buffer[56]);
*/
  convertCharArrayToIntArray(buffer,message,size);
  addBitCountToLastBlock(message,size,block);
  
  //test message[]
 /*
  printf("message[0] = %08x\n", message[0]);
  printf("message[1] = %08x\n", message[1]);
  printf("message[2] = %08X\n", message[2]);
  printf("message[3] = %08x\n", message[3]);
  printf("message[4] = %08x\n", message[4]);
  printf("message[13] = %08x\n", message[13]);
  printf("message[14] = %08x\n", message[14]);
  printf("message[15] = %08x\n", message[15]);
  printf("message[16] = %08x\n", message[16]);
  printf("message[30] = %08x\n", message[30]);
  printf("message[31] = %08x\n", message[31]);
  */
  computeMessageDigest(message, block);
return 0;
}

//read the file and store the contents
unsigned int readFile(unsigned char buffer[]){
    int letter = 0;
    int i = 0;
    
    letter = getchar();
    //get rid of newline and " "
    while(EOF != letter && letter!= '\n' && letter!= ' '){
        if (i<MAX_SIZE){
            buffer[i] = letter;
            i++;
            letter = getchar();
        }
        else{
            printf("The input file is too big!");
            exit(1);
        }
    }
    getchar();
    //append 1 at the end
    buffer[i] = 0x80;
    return i;//return the size of the file
}



//to count the number of blocks
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes){
    unsigned int blocks = 0;
    blocks = ((((8 * sizeOfFileInBytes) + 1) / 512) + 1); 
    if ((((8 * sizeOfFileInBytes) + 1) % 512) > (512 - 64)){
        blocks = blocks + 1;
    }
    return blocks;
}


void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes){
    int i = 0;
    int blocks = calculateBlocks(sizeOfFileInBytes);
    for (i=0;i<(16*blocks);i++){
        message[i] =charPacker(buffer[0+4*i], buffer[1+4*i],buffer[2+4*i],buffer[3+4*i]);   
    }
    return;
}
    


/*
 * This function should pack four chars into one integer.
 * The parameters are four unsigned characters.
 * The return is an unsigned integer.
 */
unsigned int charPacker(unsigned char letter1, unsigned char letter2, unsigned  char letter3, unsigned char letter4){
    int num1 = letter1 << 24;
    int num2 = letter2 << 16;
    int num3 = letter3 << 8;
    int num4 = letter4;
    int sum = num1 | num2 | num3 | num4;
    return sum;
}


void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount){
    int sizeOfTheFileInBits = sizeOfFileInBytes * 8;
    int indexOfEndOfLastBlock = (blockCount * 16 ) - 1;
    //append zeros at the first word if the length<2 to the power of 32;
    message[indexOfEndOfLastBlock - 1] = 0x00;
    //append the original length 
    message[indexOfEndOfLastBlock] = sizeOfTheFileInBits;
   
}



void computeMessageDigest(unsigned int message[], unsigned int blockCount){

    int h0 = 0x67452301;
    int h1 = 0xEFCDAB89;
    int h2 = 0x98BADCFE;
    int h3 = 0x10325476;
    int h4 = 0xC3D2E1F0;
    int A,B,C,D,E;
    int i = 0;
   // int t = 0;
    unsigned int temp = 0;
    unsigned int W[80];
    while(i<blockCount){
        int t = 0;
        for(t=0;t<16;t++){
            W[t] = message[t+16*i];
        }
       // printf("t = %d",t);
        for(t=16;t<80;t++){
            W[t] = S(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);
        }
        t = 0;
        A=h0;B=h1;C=h2;D=h3;E=h4;
        for(t=0;t<80;t++){
            temp = (S(A,5)) + f(t,B,C,D) + E + W[t] + K(t);
            E = D;
            D = C;
            C = S(B,30);
            B = A;
            A = temp;
            //debug printing (off)
            if(turnOnPrinting == 1){
                printf(" t =  %d: %08X  %08X  %08X  %08X  %08X\n", t,A,B,C,D,E); 
            }
        }
        //debug printing(off)
        if(turnOnPrinting == 1){
        printf("Block %d had been processed. The value of {Hi} are\n", i+1);
        printf("H0 = %08X  +  %08X  =  %08X\n ",h0, A, h0+A);
        printf("H1 = %08X  +  %08X  =  %08X\n ",h1, B, h1+B);
        printf("H2 = %08X  +  %08X  =  %08X\n ",h2, C, h2+C);
        printf("H3 = %08X  +  %08X  =  %08X\n ",h3, D, h3+D);
        printf("H4 = %08X  +  %08X  =  %08X\n ",h4, E, h4+E);
        }
        h0 = h0 + A;
        h1 = h1 + B;
        h2 = h2 + C;
        h3 = h3 + D;
        h4 = h4 + E;
        i++;
    }
    printf("\nMessage digest = %08X  %08X  %08X  %08X  %08X\n",h0,h1,h2,h3,h4);

}

//helper
unsigned int f(unsigned int t, unsigned int B, unsigned int C, unsigned int D){
    int result = 0;
    if(0 <= t && t <=19){
        result = (B & C) | ((~B) & D);
    }
    else if(20 <= t && t <=39){
        result = B ^ C ^ D;
    }
    else if (40 <= t && t <=59){
        result = (B & C) | (B & D) | (C & D);
    }
    else if (60 <= t && t <=79){
        result = B ^ C ^ D;
    }
return result;

}


unsigned int K(unsigned int t){
    int k;
    if(0 <= t && t <=19){
        k = 0x5A827999;
    }
    else if(20 <= t && t <=39){
        k = 0x6ED9EBA1;
    }
    else if (40 <= t && t <=59){
        k = 0x8F1BBCDC;
    }
    else if (60 <= t && t <=79){
        k = 0xCA62C1D6;
    }
return k;

}

unsigned int S(unsigned int X, int n){
   return (X<<n)|(X>>(32-n));
}






