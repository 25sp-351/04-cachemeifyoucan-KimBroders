#include <stdlib.h> // malloc, free
#include <string.h> // memset, memcpy
#include <time.h> // time
#ifdef DEBUG
#include <stdio.h> // printf_s, scanf_s, gets
#endif
#include "cache.h"
#include "segments.h"

typedef struct cacheEntry cacheEntry;
struct cacheEntry {
    unsigned rodLength;
    unsigned rem;    
    unsigned value;
#ifdef LRU
    unsigned key;
#endif
};

typedef struct hashEntry hashEntry;
struct hashEntry {
    unsigned index; // -1 means empty
};

struct cache {
    unsigned int size;
    unsigned int count;
    cacheEntry *entries;
    unsigned int *quantities; // array of quantities numSegment times size
    hashEntry *hashTable; // hash table will be twice the size of the cache
} myCache = {0, 0, 0, 0};

// found this on stackoverflow, seems interesting
unsigned int hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

// Calculate hash bucket for given rodLength (i.e. hash func modulus hash table size)
unsigned int getHashBucket(const unsigned rodLength) {
    return rodLength % (myCache.size * 2);
    return hash(rodLength) % (myCache.size * 2); // uses a function found on stackoverflow, not sure if this is more effective or a waste of time.
}

// increment hash index, wrap around if at the end of the hash table
unsigned nextHashIndex(const unsigned hashIx) {
    unsigned int nextHashIx = hashIx + 1;
    if (nextHashIx == (myCache.size * 2)) {
        nextHashIx = 0;
    }
    return nextHashIx;
}

// find hash entry for given rodLength.  Start at getHashBucket and use linear probing if collision to find the entry (-1 if not found)
unsigned int findHashEntry(const unsigned rodLength) {
    unsigned hashIx = getHashBucket(rodLength);
    while (myCache.hashTable[hashIx].index != -1 && myCache.entries[myCache.hashTable[hashIx].index].rodLength != rodLength) {
        hashIx = nextHashIndex(hashIx);
    }
    if (myCache.hashTable[hashIx].index == -1) {
        return -1;
    }
    return hashIx;
}

#ifdef DEBUG
void verifyHash(const char* pMsg)
{
    unsigned int hashOkay = 1; // ok
    unsigned int count = 0;
    unsigned int useCount[200] = {0};
    unsigned int colCount = 0; // total number of collisions
    unsigned int colDist = 0; // total collision distance
    for(unsigned int ix = 0; ix < myCache.count; ++ ix){
        unsigned int hashIx = findHashEntry(myCache.entries[ix].rodLength);
        if (hashIx == -1) {
            printf_s("%s: Entry %u for rodlength %u not in hash table\n", pMsg, ix, myCache.entries[ix].rodLength);
            hashOkay = 0;
        }
        unsigned int hashBucket = getHashBucket(myCache.entries[ix].rodLength);
        if (hashIx != hashBucket) {
            colCount += 1;
            if (hashIx > hashBucket)
                colDist += hashIx - hashBucket;
            else
                colDist += (myCache.size * 2) - hashIx + hashBucket;
        }
    }

    for (unsigned int hashIx = 0; hashIx < myCache.size * 2; ++hashIx) {
        if (myCache.hashTable[hashIx].index != -1) {
            ++count;
            unsigned int ix = myCache.hashTable[hashIx].index;
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
            printf_s("%3d", myCache.hashTable[hashIx].index);
        exit(1);
    }
}
#endif

#ifdef LRU
#ifdef DEBUG
void verifyKeys(const char* pMsg) {
    printf_s("%s: ", pMsg);
    for (unsigned int ix = 0; ix < myCache.count; ++ix) {
        printf_s("%3u", myCache.entries[ix].key);
    }
    printf_s("\n");
}
#endif // DEBUG
void incrementLowerKeys(unsigned key) { // 2-16-2025: increments the keys lower than the current key by one then sets the current key to 0
    for (unsigned ix = 0; ix < myCache.count; ix++) {
        if (myCache.entries[ix].key < key) {
            myCache.entries[ix].key++;
        }
    }
    
    return;
}
#endif

unsigned int getFromCache(const unsigned rodLength, unsigned *pRem, unsigned quantities[]){
#ifdef DEBUG
    verifyHash("getFromCache, start");
    verifyKeys("getFromCache, start");
#endif
    unsigned hash = findHashEntry(rodLength);
    if (hash == -1) {
        return -1;
    }
    unsigned int cacheIndex = myCache.hashTable[hash].index;
#ifdef LRU
    
    incrementLowerKeys(myCache.entries[cacheIndex].key);
    myCache.entries[cacheIndex].key = 0;
#endif
    *pRem = myCache.entries[cacheIndex].rem;
    memcpy(quantities, myCache.quantities + (cacheIndex * numSegments), numSegments * sizeof(unsigned int));
#ifdef DEBUG
    verifyHash("getFromCache, end");
    verifyKeys("getFromCache, end");
#endif
    return myCache.entries[cacheIndex].value;
}



unsigned int indexToRemove() { // TODO: least recently used; currently using last-in, first-out
#ifdef LRU
    for (unsigned int ix = 0; ix < myCache.count; ++ix) {
        if (myCache.entries[ix].key == (myCache.count - 1)) {
            return ix;
        }
    }
#endif
    return rand() % myCache.count;
}

unsigned int removeFromCache() { 
    unsigned int ix = indexToRemove();
    unsigned int hashIx = findHashEntry(myCache.entries[ix].rodLength);
#ifdef DEBUG
    printf_s("Removing index: %u (rodLength %u), Hash index: %u\n", ix, myCache.entries[ix].rodLength, hashIx);
#endif
    myCache.count--;
    myCache.hashTable[hashIx].index = -1; // clear this hash table entry
#ifdef DEBUG
    printf_s("checking collisions\n"); 
#endif
    // move collisions back if necessary keep going until we reach an empty slot we may need to skip over entries that are not collisions
    for (unsigned int nextHashIx = nextHashIndex(hashIx); myCache.hashTable[nextHashIx].index != -1; nextHashIx = nextHashIndex(nextHashIx)) {
        unsigned int myHashBucket = getHashBucket(myCache.entries[myCache.hashTable[nextHashIx].index].rodLength); // the slot I belong in
        unsigned int midHashIx = nextHashIndex(hashIx);
        while (midHashIx != nextHashIx && myHashBucket != midHashIx) {
            midHashIx = nextHashIndex(midHashIx);
        }
        if (myHashBucket == midHashIx)
            continue; // I belong in the middle (between hashIx and nextIx) or where I am (nextIx)
#ifdef DEBUG
        printf_s("shifted collision back nextHashIx %u to hashIx %u for index %u representing rodlength %u\n",
                 nextHashIx, hashIx, myCache.hashTable[nextHashIx].index, myCache.entries[myCache.hashTable[nextHashIx].index].rodLength);
#endif
        myCache.hashTable[hashIx].index = myCache.hashTable[nextHashIx].index;
        myCache.hashTable[nextHashIx].index = -1;
        hashIx = nextHashIx; // hashIx always points to the empty slot
    }
#ifdef DEBUG
    printf_s("done removing\n");
#endif
    return ix;
}

void putInCache(const unsigned rodLength, unsigned rem, unsigned quantities[], unsigned value) {
#ifdef DEBUG
    verifyHash("putInCache, start");
    verifyKeys("putInCache, start");
#endif
    unsigned int cacheIndex = myCache.count; // default add to the end of cache
    if (myCache.count == myCache.size) {
        cacheIndex = removeFromCache(); // don't add to the end of cache, add where old item removed
    }

    myCache.entries[cacheIndex].rodLength = rodLength;
    myCache.entries[cacheIndex].rem = rem;
    myCache.entries[cacheIndex].value = value;
    memcpy(myCache.quantities + (cacheIndex * numSegments), quantities, numSegments * sizeof(unsigned int));
    unsigned int hashIx = getHashBucket(rodLength);
    while (myCache.hashTable[hashIx].index != -1) { // moves hashIx forward until available
        hashIx = nextHashIndex(hashIx);
    }
    myCache.hashTable[hashIx].index = cacheIndex;
    myCache.count++;
#ifdef LRU
    incrementLowerKeys(-1); // Needs to come before myCache.hashTable[hashIx].key = 0 and before count++. Increments all pre-existing keys.
    myCache.entries[cacheIndex].key = 0;
#endif
    
#ifdef DEBUG
    printf_s("added cache entry size=%u\n", myCache.count);
    verifyHash("putInCache, end");
    verifyKeys("putInCache, end");
#endif
    return;
}

void initializeCache(unsigned int size){
    myCache.count = 0;
    myCache.size = size;
    srand(time(0)); // use current time as seed for random generator
    myCache.entries = (cacheEntry *)malloc(size * sizeof(cacheEntry));
    myCache.hashTable = (hashEntry *)malloc(2 * size * sizeof(hashEntry));
    for (unsigned hashIx = 0; hashIx < 2*size; ++hashIx) {
        myCache.hashTable[hashIx].index = -1; // empty entry
    }
    for (unsigned ix = 0; ix < size; ++ix) {
        myCache.entries[ix].rodLength = 0; // 0 is not a valid rod length
        myCache.entries[ix].rem = 0;
        myCache.entries[ix].value = 0;
#ifdef LRU
        myCache.entries[ix].key = 0; // 2-16-2025: Not sure what to initialize key to (0 seems fine, -1 could work too)
#endif
    }
    myCache.quantities = (unsigned int *)malloc(size * numSegments * sizeof(unsigned int));
#ifdef DEBUG
    verifyHash("initializeCache, end");
    verifyKeys("initializeCache, end");
#endif
    return;
}

void freeCache() {
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