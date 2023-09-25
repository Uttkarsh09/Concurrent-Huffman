#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include "configuration.hpp"
#include "cpuHuffman.hpp"
#include "utils.hpp"

using namespace std;

// ! Sequential
// TODO: Make purely concurrent

u_int32_t __concurrentCountCharFrequency__(FILE *fptr, int (*frequencies)[][TOTAL_CHARS], u_int8_t processor_count){
	short i;
	u_int32_t file_size, segment_size_bytes, length_to_read;
	thread threads[processor_count];
	
	file_size = getFileSize(fptr);
	segment_size_bytes = file_size / processor_count;
	length_to_read = segment_size_bytes;

	for(i=0 ; i<processor_count ; i++){
		cout << "Starting Thread = 	" << i <<endl;
		
		// ? Last thread will get the lenth till the end of file
		if(i == processor_count-1){
			cout << "Last thread" << endl << endl;
			length_to_read += file_size % segment_size_bytes;
		}
		// ! NOT CONCURRENT
		threads[i] = thread(calculateFrequency, fptr, length_to_read, (*frequencies)[i]);
	}

	for(i=0 ; i<processor_count ; i++) 
		threads[i].join();

	rewind(fptr);

	return file_size;
}

void compress( vec_char *input_ptr, vec_string *output_ptr, vec_int *prefix_ptr, int offset, map_char_to_string *map, u_int32_t length ) {
	u_int32_t i=0, sum=0;
	string s;

 	while(i < length){
		s = map->at(input_ptr->at(offset+i));
		output_ptr->at(offset + i) = s;
		sum += s.length();
		prefix_ptr->at(offset+i) = sum;
		++i;
	}
}

void __concurrentCompress__(string input_file, map_char_to_string *map, vec_string *compressed_output, vec_int *prefix_sum){
	const short processor_count = thread::hardware_concurrency();
	// const short processor_count = 27;
	FILE *iptr = fopen(input_file.c_str(), "rb");
	u_int32_t input_file_size = getFileSize(iptr);
	u_int32_t segment_size = input_file_size / processor_count;
	u_int32_t offset=0, length_to_compress=segment_size;
	thread threads[processor_count];
	vec_char input_buffer;
	int i=0, j;
	char ch;

	while(!feof(iptr)) {
		ch = fgetc(iptr);
		if(!feof(iptr)) input_buffer.push_back(ch);
	}

	compressed_output->resize(input_buffer.size());
	prefix_sum->resize(input_buffer.size());

	cout << "Processor COres -> " << processor_count <<endl;
	cout << "Input File size - " << input_file_size << endl;
	cout << "Segment size - " << segment_size << endl;
	puts("Starting the scheduler");

	for(i=0 ; i<processor_count ; i++){
		offset = i * segment_size;
		// cout << "offset -> " << offset << endl;
		if(i == processor_count - 1){
			length_to_compress += (input_file_size % processor_count);
		}
		// cout << "length to compress -> " << length_to_compress << endl;

		threads[i] = thread(
			compress, 
			&input_buffer, 
			compressed_output, 
			prefix_sum, 
			offset, 
			map, 
			length_to_compress
		);
	}

	puts("Joining");
	for(i=0 ; i<processor_count ; i++)
		threads[i].join();
	
	puts("Combining the partial sum");

	// Note: I don't know whether i'll remember, why ive done, what ive done while explaining :(
	int stretch_till_end=0;
	for(i=1 ; i<processor_count ; i++){
		offset = i * segment_size;
		// cout << "i -> " << i << endl;
		// cout << "Offset -> " << offset << endl;
		for(j=offset ; j<offset+segment_size ; j++){
			if(i >= processor_count-1){
				segment_size = length_to_compress;
			}
			// cout << "j -> " << j<< endl; 
			prefix_sum->at(j) += prefix_sum->at(offset - 1);
		}
	}

	puts("Completed");

	for(i=0 ; i<prefix_sum->size() ; i++){
		printf("%3d ", prefix_sum->at(i));
	}puts("");
}

// TODO: File compression and parallel prefix in a single operation.