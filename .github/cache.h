#ifndef CACHE_H
#define CACHE_H

unsigned int getFromCache(const unsigned rodLength, unsigned *pRem, unsigned quantities[]); // returns the value of the rodcutting
void putInCache(const unsigned rodLength, unsigned rem, unsigned quantities[], unsigned value);

void initializeCache(unsigned int size);
void freeCache();

#endif // CACHE_H