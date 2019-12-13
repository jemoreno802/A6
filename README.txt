Jacquelyn Moreno and Clayton Emmel

Build:
	g++ -std=c++11 -o main main.cpp
Run: 	./main


carray_simple_set: My approach to this set was to have an array of longs(64 bits) which acts as a bit vector for the set. I took the constructor parameters l and h and subtracted them to find the size of the set’s domain. 
To find the number of words(longs) that were needed to have enough bits to store each element, I computed numWords = ((h - l)/64) + 1. For inserting and deleting an element from the set, I flipped its corresponding bit to 1 or 0 respectively. To determine if a set contains an element, I checked the bit corresponding to that element. This implementation for the carray_simple_set allows for constant time insertion, removal and access while only using (h-l)+ c, c <=63 bits. 