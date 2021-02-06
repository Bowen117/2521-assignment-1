// COMP2521 20T2 Assignment 1
// Dict.c ... implementsation of Dictionary ADT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Dict.h"
#include "WFreq.h"

typedef struct _DictNode *Link;

typedef struct  _DictNode {
   WFreq  data;
   Link   left;
   Link   right;
   int height;
} DictNode;

struct _DictRep {
   Link tree;
};

struct NewLink {
    WFreq *data; // data in the added node
    Link subRoot; // root of subtree containing the added node
};
struct NewLink NodeInsert(Link node, char *w);
static int max(int a, int b);
static int height(Link n);
static Link rotateLeft(Link n);
static Link rotateRight(Link n);
void set_height (Link n);
void travPrint (Link d);
WFreq *NodeFind(Link node, char *w);
void insertWfs(WFreq wfreq, WFreq *wfs, int n);
void traverse(Link d, WFreq *wfs, int n);


// create new empty Dictionary
Dict newDict()
{
   Dict Dictionary = malloc(sizeof(struct _DictRep));
   Dictionary->tree = NULL;
   return Dictionary;
}

// insert new word into Dictionary
// return pointer to the (word,freq) pair for that word
WFreq *DictInsert(Dict d, char *w) {
   char *word = malloc(sizeof(char) * (strlen(w)+1));
   strcpy(word, w);
   if (d->tree == NULL) {
     Link root = malloc(sizeof(struct _DictNode));
     root->data.word = word;
     root->data.freq = 1;
     root->left = NULL;
     root->right = NULL;
     root->height = 1;
     d->tree = root;
 
     return &(root->data); //need ptr to receive address.
   }
 
   struct NewLink newLink = NodeInsert(d->tree, word);

   d->tree = newLink.subRoot;

   return newLink.data;
}

struct NewLink NodeInsert(Link node, char *w) {
    if (node == NULL) {
        struct _DictNode *insert = malloc(sizeof(struct _DictNode));
        insert->data.word = w;
        insert->data.freq = 1;
        insert->left = NULL;
        insert->right = NULL;
        insert->height = 1;
        struct NewLink newLink = {&(insert->data), insert};
        return newLink;
    }

    int value;
    value = strcmp(w, node->data.word);
    WFreq *data;
    if (value < 0) {       
        struct NewLink newLink = NodeInsert(node->left, w);
        node->left = newLink.subRoot;
        data = newLink.data;                                          
    } else {
        struct NewLink newLink = NodeInsert(node->right, w);
        node->right = newLink.subRoot;
        data = newLink.data;        
    }

    set_height(node);
    Link subRoot = node;
    // left left case:
    if (height(node->left) - height(node->right) > 1 && strcmp(w, node->left->data.word) < 0) {
        subRoot = rotateRight(node);
    }
    // right right case:
    if (height(node->right) - height(node->left) > 1 && strcmp(w, node->right->data.word) > 0) {
        subRoot = rotateLeft(node);
    }
    //left right case:
    if (height(node->left) - height(node->right) > 1 && strcmp(w, node->left->data.word) > 0) {
        node->left = rotateLeft(node->left);
        subRoot = rotateRight(node);
    }//right left case:
    if (height(node->right) - height(node->left) > 1 && strcmp(w, node->right->data.word) < 0) {
        node->right = rotateRight(node->right);
        subRoot = rotateLeft(node);
    }
      
    struct NewLink newLink = {data, subRoot};
    return newLink; 
      
}       

static int max(int a, int b) {
    return a > b ? a : b;
}     
 
static int height(Link n) {
    if (n == NULL) {
        return -1;
    } else {
        return n->height;
    }
}

static Link rotateLeft(Link n) {

    if (n == NULL) {
        return NULL;
    }
    if (n->right == NULL) {
        return n;
    }
    Link y = n->right;
    Link t2 = y->left;
    
    y->left = n;
    n->right = t2;

    set_height(n);
    set_height(y);
    
    return y;
}      

static Link rotateRight(Link n) {

    if (n == NULL) {
        return NULL;
    }
    if (n->left == NULL){
        return n;
    }
    Link x = n->left;
    Link t2 = x->right;
    
    x->right = n;
    n->left = t2;    
    
    set_height(n);
    set_height(x);
    
    return x;
}

void set_height (Link n) {
    n->height = 1 + max(height(n->left), height(n->right));    
}
// find Word in Dictionary
// return pointer to (word,freq) pair, if found
// otherwise return NULL
WFreq *DictFind(Dict d, char *w) {
   if (d->tree == NULL)
        return NULL;
   return NodeFind(d->tree, w);
}

WFreq *NodeFind(Link node, char *w) { 
    int value = strcmp(node->data.word, w);
    if (value == 0) {
        return &(node->data);
    } else {
        if (value < 0) {
            if (node->right == NULL) {
                return NULL;
            } else {
                return NodeFind(node->right, w);
            }
        } else {
            if (node->left == NULL) {
                return NULL;
            } else {
                return NodeFind(node->left, w);
            }
        }
    }          
}

// find top N frequently occurring words in Dict
// input: Dictionary, array of WFreqs, size of array
// returns: #WFreqs in array, modified array
int findTopN(Dict d, WFreq *wfs, int n) 
{
   //traverse through the tree and add words to the array based on freq.
   traverse(d->tree, wfs, n);
   return 0;
}

void traverse(Link d, WFreq *wfs, int n) { 
    if (d == NULL){
        return;
    }
    traverse(d->left, wfs, n);   
    insertWfs(d->data, wfs, n);       
    traverse(d->right, wfs, n);
}

void insertWfs(WFreq wfreq, WFreq *wfs, int n) {
    int count = 0;
    while (count < n && wfs[count].freq > wfreq.freq) {
        count++;
    }
    if (count == n) {
        return;
    }
    //shifts the array by one each.
    for (int i = n-1; i >= count; i--) {
        wfs[i+1] = wfs[i];
    }
    wfs[count] = wfreq;                  
}

// print a dictionary
void showDict(Dict d) {
   travPrint(d->tree);
   return;
}

void travPrint (Link d) {
    if (d == NULL) {
        return;
    }
    travPrint(d->left);    
    printf("%d %s\n", d->data.freq, d->data.word);
    travPrint(d->right);    
}
