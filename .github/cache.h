#ifndef CACHE_H
#define CACHE_H

unsigned int getFromCache(const unsigned rodLength, unsigned *pRemainder, unsigned quantities[]); // returns the value of the rodcutting
void putInCache(const unsigned rodLength, unsigned remainder, unsigned quantities[], unsigned value);

void initializeCache(unsigned int size);
void freeCache();

#endif // CACHE_H