#include<iostream>
#include "cpuHuffman.hpp"

using namespace std;

void calculateFrequency(FILE *fptr, u_int32_t length_to_read, int *char_freq){
	if(fptr == NULL){
		cout << "Cannot access file" << endl;
		exit(EXIT_FAILURE);
	}

	u_int32_t offset = 0;
	char ch;
	while(offset < length_to_read && !feof(fptr)){
		ch = fgetc(fptr);
		char_freq[(int)ch]++;
		offset++;
	}
}