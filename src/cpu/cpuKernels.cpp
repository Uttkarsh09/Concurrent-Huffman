#include <iostream>
#include "configuration.hpp"
#include "cpuKernels.hpp"
#include "utils.hpp"

using namespace std;


void __calculateFrequency__(
	string file_name, 
	u_int32_t read_offset,
	u_int32_t length_to_read,
	u_int32_t *char_freq
){
	FILE *fptr;
	openFile(&fptr, file_name, "rb");
	fseek(fptr, read_offset, SEEK_SET);

	u_int32_t count = 0;
	char ch;
	while(count < length_to_read && !feof(fptr)){
		ch = fgetc(fptr);
		char_freq[(int)ch]++;
		count++;
	}
}

void __readFile__(
	string file_name, 
	u_int32_t read_offset,
	u_int32_t length_to_read,
	vec_char *read_buffer
){
	FILE *fptr;
	openFile(&fptr, file_name, "rb");
	fseek(fptr, read_offset, SEEK_SET);

	u_int32_t count = 0;
	char ch;
	while(count < length_to_read && !feof(fptr)){
		ch = fgetc(fptr);
		read_buffer->push_back(ch);
		++count;
	}
}

void __compress__( 
	vec_char *input_ptr, 
	vec_string *output_ptr, 
	vec_uint *prefix_ptr, 
	int offset, 
	map_char_to_string *map, 
	u_int32_t length 
) {
	u_int32_t i=0, sum=0;
	string s;

 	while(i < length){
		s = map->at(input_ptr->at(i));
		output_ptr->at(offset + i) = s;
		sum += s.length();
		prefix_ptr->at(offset+i) = sum;
		++i;
	}
}


// TODO: File compression and parallel prefix in a single operation.