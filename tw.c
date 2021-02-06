// COMP2521 20T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "Dict.h"
#include "WFreq.h"
#include "stemmer.h"

#define STOPWORDS "stopwords"
#define MAXLINE 1000
#define MAXWORD 100

#define STARTING "*** START OF"
#define ENDING   "*** END OF"

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')

void addWord(char *word, int wordLength, Dict stopwords, Dict wfreqs);

int main( int argc, char *argv[])
{
   // TODO ... add any variables you need

   FILE  *in;         // currently open file
   Dict   stopwords;  // dictionary of stopwords
   Dict   wfreqs;     // dictionary of words from book
   WFreq *results;    // array of top N (word,freq) pairs
                      // (dynamically allocated)

   char *fileName;    // name of file containing book text
   int   nWords;      // number of top frequency words to show

   char   line[MAXLINE];  // current input line
   char   word[MAXWORD];  // current word

   // process command-line args
   switch (argc) {
   case 2:
      nWords = 10;
      fileName = argv[1];
      break;
   case 3:
      nWords = atoi(argv[1]);
      if (nWords < 10) nWords = 10;
      fileName = argv[2];
      break;
   default:
      fprintf(stderr,"Usage: %s [Nwords] File\n", argv[0]);
      exit(EXIT_FAILURE);
   }
 
   // build stopword dictionary
   stopwords = newDict();
   in = fopen(STOPWORDS, "r");
  
   if (in == NULL) {
       fprintf(stderr, "Can't open stopwords\n");
       exit(EXIT_FAILURE);
   }
   char *stopWordLine = fgets(word, MAXWORD, in);
   while (stopWordLine != NULL) {
       if (word[strlen(word)-1] == '\n')
           word[strlen(word)-1] = '\0';
     DictInsert(stopwords, word);
     stopWordLine = fgets(word, MAXWORD, in);
   }

   fclose(in);
   // scan File, up to start of text
   in = fopen(fileName, "r");
   if (in == NULL) {
       fprintf(stderr, "Can't open %s\n",fileName);
       exit(EXIT_FAILURE);
   }    
   char *bookLine = fgets(line, MAXLINE, in);
   while (strncmp(line, STARTING, strlen(STARTING)) != 0){   
        bookLine = fgets(line, MAXLINE, in);
        if (bookLine == NULL) {
            fprintf(stderr, "Not a Project Gutenberg book\n");
            exit(EXIT_FAILURE);
       }
   }
   bookLine = fgets(line, MAXLINE, in);   

   // scan File reading words and accumualting counts
   wfreqs = newDict();
   while (strncmp(line, ENDING, strlen(ENDING)) != 0){

     int wordLength = 0;
     for (int i=0; line[i] != 0; i++) {
         if (line[i] <= 'Z' && line[i] >= 'A') {
             line[i] = line[i] + 32;
         }
         if ((line[i] <= 'z' && line[i] >= 'a') || (line[i] == 39)|| (line[i] == 45)){
           word[wordLength] = line[i];
           wordLength++;
         } else {
           addWord(word, wordLength, stopwords, wfreqs);
           wordLength = 0;
         }
     }
     
     addWord(word, wordLength, stopwords, wfreqs);     
     bookLine = fgets(line, MAXLINE, in);
     if (bookLine == NULL) {
            fprintf(stderr, "Not a Project Gutenberg book\n");
            exit(EXIT_FAILURE);
       }
   }
   fclose(in);

   // compute and display the top N words
   results = malloc(nWords * sizeof(WFreq));
   for (int i=0; i<nWords; i++) {
     results[i].word = NULL;
     results[i].freq = 0;
   }
   findTopN(wfreqs, results, nWords);
   for (int i=0; i<nWords; i++) {
       if (results[i].freq > 0) {
         printf("%d %s\n", results[i].freq, results[i].word);
      }
   }       
   // done
   return EXIT_SUCCESS;
}

void addWord(char *word, int wordLength, Dict stopwords, Dict wfreqs) {
    word[wordLength] = 0;
    if (wordLength > 1) {
        char *newWord = malloc(MAXWORD * sizeof(char));
        strcpy(newWord, word);
        if (DictFind(stopwords, newWord) == NULL) {
            int stemWordLength = stem(newWord, 0, wordLength-1);
            newWord[stemWordLength+1] = 0;
            WFreq *wfreq = DictFind(wfreqs, newWord);
            if (wfreq != NULL) {
                wfreq->freq++;
            } else {
                DictInsert(wfreqs, newWord);
            }
        }
    }
}


