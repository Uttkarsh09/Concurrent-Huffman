#include <iostream>
#include <thread>
#include <vector>
#include "configuration.hpp"
#include "cpuHuffman.hpp"

using namespace std;

// ! Sequential
// TODO: Make purely concurrent

u_int32_t concurrentReadFile(FILE *fptr, int (*frequencies)[][TOTAL_CHARS], u_int8_t processor_count){
	short i;
	u_int32_t file_size, segment_size_bytes, length_to_read;
	thread threads[processor_count];
	
	fseek(fptr, 0, SEEK_END);
	file_size = ftell(fptr);
	segment_size_bytes = file_size / processor_count;
	length_to_read = segment_size_bytes;
	rewind(fptr);

	for(i=0 ; i<processor_count ; i++){
		cout << "Starting Thread = 	" << i <<endl;
		
		if(i == processor_count-1){
			cout << "Last thread" << endl << endl;
			// ? Last thread will get the lenth till the end of file
			length_to_read += file_size % segment_size_bytes;
			threads[i] = thread(calculateFrequency, fptr, length_to_read, (*frequencies)[i]);
		}
		else {
			threads[i] = thread(calculateFrequency, fptr, length_to_read, (*frequencies)[i]);
		}
	}

	for(i=0 ; i<processor_count ; i++) 
		threads[i].join();

	rewind(fptr);

	return file_size;
}

void concurrentPrefixSum(vector<HuffNode*> vec, vector<int> prefixSums, u_int8_t processor_count){
}