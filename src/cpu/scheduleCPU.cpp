#include <iostream>
#include <thread>
#include <cstring>
#include <vector>
#include "configuration.hpp"
#include "cpuHuffman.hpp"

using namespace std;

void compressOnCPU(string input_file_name, string output_file_name){
	// const unsigned short processor_count= thread::hardware_concurrency();
	const unsigned short processor_count= 1;
	FILE *fptr = NULL;
	vector<HuffNode*> total_frequencies;
	u_int32_t encoded_file_size_bits = 0;
	u_int32_t file_size;
	int frequencies[processor_count][TOTAL_CHARS];
	int i, j;
	
	fptr = fopen(input_file_name.c_str(), "r");
	if(fptr == NULL){
		cout << "Unable to open file - " << input_file_name << endl;
		exit(EXIT_FAILURE);
	}

	memset(frequencies, 0, sizeof(frequencies));

	file_size = concurrentReadFile(fptr, &frequencies, processor_count);
	
	u_int32_t count = 0;
	for(j=0 ; j<TOTAL_CHARS ; j++){
		count = 0;
		for(i=0 ; i<processor_count ; i++)
			count += frequencies[i][j];

		if(count > 0)
			total_frequencies.push_back(new HuffNode((char)j, count));
	}

	HuffNode *rootNode = generateHuffmanTree(total_frequencies);
	string tempStr = "";
	encoded_file_size_bits = populateHuffmanTable(&total_frequencies, rootNode, tempStr);

	cout << "Original File Size -> " << file_size << endl;
	cout << "Encoded file size (without headers) -> " << encoded_file_size_bits << "bits = ";
	cout << encoded_file_size_bits / 8 << "Bytes + " << encoded_file_size_bits%8 << "bits" << endl;


}
