#include <iostream>
#include "utils.hpp"

using namespace std;

u_int32_t getFileSize(FILE *fptr){
	if(fptr == NULL){
		cout << "Unable to open file to calculate its size" << endl;
		exit(EXIT_FAILURE);
	}
	u_int32_t file_size = -1;

	fseek(fptr, 0, SEEK_END);
	file_size = ftell(fptr);
	rewind(fptr);
	
	return file_size;
}