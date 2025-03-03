#include <stdlib.h> // malloc, free
#include <string.h> // memset, memcpy
#include <time.h> // time
// #define DEBUG
#ifdef DEBUG
#include <stdio.h> // printf_s, scanf_s, gets
#endif
#include "cache.h"
#include "segments.h"

#ifdef DEBUG
#define USE_COUNT 200
#endif // DEBUG

typedef struct cacheEntry cacheEntry;
struct cacheEntry {
    unsigned rodLength;
    unsigned remainder;
    unsigned value;
#ifdef LRU
    unsigned key;
#endif
};

struct cache {
    unsigned int size;
    unsigned int count;
    cacheEntry *entries;
    unsigned int *quantities; // array of quantities numSegment times size
    unsigned int *hashTable; // hash table will be twice the size of the cache
} myCache = {0, 0, 0, 0};

unsigned int getHashBucket(const unsigned rodLength) {
    // takes an unsigned int rodLength
    // Calculate hash bucket for given rodLength (i.e. hash func modulus hash table size)
    // returns an unsigned int hash bucket
    return rodLength % (myCache.size * 2);
    return hash(rodLength) % (myCache.size * 2); // uses a function found on stackoverflow, not sure if this is more effective or a waste of time.
}

unsigned nextHashIndex(const unsigned hashIx) { 
    // takes an unsigned int hashIx
    // increment hashIx, wrap around if at the end of the hash table
    // returns the next index in the hash table
    unsigned int nextHashIx = hashIx + 1;
    if (nextHashIx == (myCache.size * 2))
        nextHashIx = 0;
    return nextHashIx;
}

unsigned int findHashEntry(const unsigned rodLength) {
    // takes a rodLength
    // find hash entry for given rodLength.
    // Start at getHashBucket and use linear probing if collision to find the entry (-1 if not found)
    // returns the index of the entry in the hash table or -1 if not found
    unsigned hashIx = getHashBucket(rodLength);
    while (myCache.hashTable[hashIx] != -1 && myCache.entries[myCache.hashTable[hashIx]].rodLength != rodLength)
        hashIx = nextHashIndex(hashIx);
    if (myCache.hashTable[hashIx] == -1)
        return -1;
    return hashIx;
}

#ifdef DEBUG
#ifdef LRU
void printKeys(const char* pMsg) { // prints the keys for debugging purposes
    // takes a message to print before the keys, then prints the keys, returns nothing
    printf_s("%s: ", pMsg);
    for (unsigned int ix = 0; ix < myCache.count; ++ix)
        printf_s("%3u", myCache.entries[ix].key);
    printf_s("\n");
}
#endif // LRU

void printHash() { // prints the hash for debugging purposes
    // takes nothing, prints the hash table and the entries in the cache, returns nothing
    printf_s("Index:");
    for (unsigned ix = 0; ix < myCache.count; ++ix)
        printf_s("%3u", ix);
    printf_s("\nRodLen");
    for (unsigned ix = 0; ix < myCache.count; ++ix)
        printf_s("%3u", myCache.entries[ix].rodLength);
    printf_s("\nBucket");
    for (unsigned ix = 0; ix < myCache.count; ++ix)
        printf_s("%3u", getHashBucket(myCache.entries[ix].rodLength));
    printf_s("\nHashIx");
    for (unsigned ix = 0; ix < myCache.count; ++ix)
        printf_s("%3d", findHashEntry(myCache.entries[ix].rodLength));
    printf_s("\nBucket");
    for (unsigned hashIx = 0; hashIx < myCache.size * 2; ++hashIx)
        printf_s("%3u", hashIx);
    printf_s("\nIndex:");
    for (unsigned hashIx = 0; hashIx < myCache.size * 2; ++hashIx)
        printf_s("%3d", myCache.hashTable[hashIx]);
    return;
}

void verifyHash(const char* pMsg) { // verifies the hash table for debugging purposes
    // takes a message to print before the verification
    // checks that the hash table is correct, that all entries are in the hash table, and that there are no duplicates
    // if the hash table is incorrect, prints error messages and prints the hash table
    // if LRU is defined, prints the keys for debugging purposes
    // returns nothing
    unsigned int hashOkay = 1; // ok
    unsigned int count = 0;
    unsigned int useCount[USE_COUNT] = {0};
    for(unsigned int ix = 0; ix < myCache.count; ++ ix){
        unsigned int hashIx = findHashEntry(myCache.entries[ix].rodLength);
        if (hashIx == -1) {
            printf_s("%s: Entry %u for rodlength %u not in hash table\n", pMsg, ix, myCache.entries[ix].rodLength);
            hashOkay = 0;
        }
    }

    for (unsigned int hashIx = 0; hashIx < myCache.size * 2; ++hashIx) {
        if (myCache.hashTable[hashIx] != -1) {
            ++count;
            unsigned int ix = myCache.hashTable[hashIx];
            useCount[ix] += 1;
            if (ix >= myCache.count) {
                printf_s("%s: Cache index %u out of bounds!!!\n", pMsg, ix);
                hashOkay = 0;
            }
        }
    }
    if (count != myCache.count) {
        printf_s("%s: Count mismatch: cache count %u, hash count %u\n", pMsg, myCache.count, count);
        hashOkay = 0;
    }
    for (unsigned int useIx = 0; useIx < myCache.count; ++useIx) {
        if (useCount[useIx] != 1) {
            printf_s("%s: Use count error %u is used %u times\n", pMsg, useIx, useCount[useIx]);
            hashOkay = 0;
        }
    }
    if (hashOkay != 1) { // hash not ok
        printHash();
        exit(1);
    }
#ifdef LRU
    printKeys(pMsg);
#endif // LRU
    return;
}
#endif // DEBUG

#ifdef LRU
void incrementLowerKeys(unsigned key) {
    // takes an unsigned int key
    // increments the keys lower than the provided key by one then sets the current key to 0
    // returns nothing
    for (unsigned ix = 0; ix < myCache.count; ix++) {
        if (myCache.entries[ix].key < key)
            myCache.entries[ix].key++;
    }
    return;
}
#endif // LRU

unsigned int getFromCache(const unsigned rodLength, unsigned *pRemainder, unsigned quantities[]) { 
    // takes a rodLength, pRemainder, quantities, returns the value in the cache or -1 if not found
    // pRemainder is the remainder of the rodLength after cutting it into segments
    // quantities is an array of unsigned ints that will be filled with the quantities of each segment
    // returns the value in the cache or -1 if not found
#ifdef DEBUG
    verifyHash("getFromCache, start");
#endif // DEBUG
    unsigned hashIx = findHashEntry(rodLength);
    if (hashIx == -1)
        return -1;
    unsigned int cacheIndex = myCache.hashTable[hashIx];
#ifdef LRU
    incrementLowerKeys(myCache.entries[cacheIndex].key);
    myCache.entries[cacheIndex].key = 0;
#endif // LRU
    *pRemainder = myCache.entries[cacheIndex].remainder;
    memcpy(quantities, myCache.quantities + (cacheIndex * numSegments), numSegments * sizeof(unsigned int));
#ifdef DEBUG
    verifyHash("getFromCache, end");
#endif // DEBUG
    return myCache.entries[cacheIndex].value;
}

unsigned int indexToRemove() { // takes nothing, returns the index of the entry to remove from the cache
    // if LRU is defined, it is the index of the entry with the highest possible key (myCache.count - 1)
    // otherwise, it is a random index in the cache
#ifdef LRU
    for (unsigned int ix = 0; ix < myCache.count; ++ix) {
        if (myCache.entries[ix].key == (myCache.count - 1))
            return ix;
    }
#endif // LRU
    return rand() % myCache.count;
}
unsigned int shiftCollisions(unsigned int hashIx) { // takes an unsigned int hashIx, returns the index of the empty slot in the hash table
    // move collisions back if necessary keep going until we reach an empty slot we may need to skip over entries that are not collisions
    for (unsigned int nextHashIx = nextHashIndex(hashIx); myCache.hashTable[nextHashIx] != -1; nextHashIx = nextHashIndex(nextHashIx)) {
        unsigned int myHashBucket = getHashBucket(myCache.entries[myCache.hashTable[nextHashIx]].rodLength); // the slot I belong in
        unsigned int midHashIx = nextHashIndex(hashIx);
        while (midHashIx != nextHashIx && myHashBucket != midHashIx)
            midHashIx = nextHashIndex(midHashIx);
        if (myHashBucket == midHashIx)
            continue; // I belong in the middle (between hashIx and nextIx) or where I am (nextIx)
#ifdef DEBUG
        printf_s("shifted collision back nextHashIx %u to hashIx %u for index %u representing rodlength %u\n",
                 nextHashIx, hashIx, myCache.hashTable[nextHashIx], myCache.entries[myCache.hashTable[nextHashIx]].rodLength);
#endif // DEBUG
        myCache.hashTable[hashIx] = myCache.hashTable[nextHashIx];
        myCache.hashTable[nextHashIx] = -1;
        hashIx = nextHashIx; // hashIx always points to the empty slot
    }
    return hashIx;
}

unsigned int removeFromCache() { // takes nothing, removes an entry from the cache and returns the index of the entry removed
    unsigned int ix = indexToRemove();
    unsigned int hashIx = findHashEntry(myCache.entries[ix].rodLength);
#ifdef DEBUG
    printf_s("Removing index: %u (rodLength %u), Hash index: %u\n", ix, myCache.entries[ix].rodLength, hashIx);
#endif
    myCache.count--;
    myCache.hashTable[hashIx] = -1; // clear this hash table entry
#ifdef DEBUG
    printf_s("checking collisions\n"); 
#endif // DEBUG
    shiftCollisions(hashIx);
#ifdef DEBUG
    printf_s("done removing\n");
#endif // DEBUG
    return ix;
}

void putInCache(const unsigned rodLength, unsigned remainder, unsigned quantities[], unsigned value) {
    // takes a rodLength, remainder, quantities, and value
    // if the cache is full, removes an entry from the cache
    // puts the entry in the cache
    // returns nothing
#ifdef DEBUG
    verifyHash("putInCache, start");
#endif // DEBUG
    unsigned int cacheIndex = myCache.count; // default add to the end of cache
    if (myCache.count == myCache.size)
        cacheIndex = removeFromCache(); // don't add to the end of cache, add where old item removed

    myCache.entries[cacheIndex].rodLength = rodLength;
    myCache.entries[cacheIndex].remainder = remainder;
    myCache.entries[cacheIndex].value = value;

    memcpy(myCache.quantities + (cacheIndex * numSegments), quantities, numSegments * sizeof(unsigned int));
    unsigned int hashIx = getHashBucket(rodLength);
    while (myCache.hashTable[hashIx] != -1) // moves hashIx forward until available
        hashIx = nextHashIndex(hashIx);
    myCache.hashTable[hashIx] = cacheIndex;
    myCache.count++;
#ifdef LRU
    incrementLowerKeys(-1); // Needs to come before myCache.hashTable[hashIx].key = 0 and before count++. Increments all pre-existing keys.
    myCache.entries[cacheIndex].key = 0;
#endif // LRU
    
#ifdef DEBUG
    printf_s("added cache entry size=%u\n", myCache.count);
    verifyHash("putInCache, end");
#endif // DEBUG
    return;
}

void initializeCache(unsigned int size) {
    // takes an unsigned int size (the number of entries that can be put in the cache)
    // initializes the cache
    // returns nothing
    myCache.count = 0;
    myCache.size = size;
    srand(time(0)); // use current time as seed for random generator

    myCache.entries = (cacheEntry *)calloc(size, sizeof(cacheEntry));

    myCache.hashTable = (unsigned int *)malloc(2 * size * sizeof(unsigned int));
    for (unsigned hashIx = 0; hashIx < 2*size; ++hashIx)
        myCache.hashTable[hashIx] = -1; // empty entry
    
    myCache.quantities = (unsigned int *)malloc(size * numSegments * sizeof(unsigned int));
#ifdef DEBUG
    verifyHash("initializeCache, end");
#endif // DEBUG
    return;
}

void freeCache() { // takes nothing, frees the cache and returns nothing
    free(myCache.entries);
    free(myCache.hashTable);
    free(myCache.quantities);
    
    myCache.count = 0;
    myCache.size = 0;
    myCache.entries = 0;
    myCache.hashTable = 0;
    myCache.quantities = 0;
    return;
}