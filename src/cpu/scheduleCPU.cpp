#include <iostream>
#include <thread>
#include <cstring>
#include <vector>
#include <unordered_map>
#include "configuration.hpp"
#include "cpuHuffman.hpp"

using namespace std;

void compressOnCPU(string input_file_name, string output_file_name){
	// const unsigned short processor_count= thread::hardware_concurrency();
	const unsigned short processor_count= 1;
	FILE *fptr = NULL;
	vector<HuffNode*> total_frequencies;
	u_int32_t compressed_file_size_bits = 0;
	u_int32_t original_file_size;
	vec_int prefix_sum;
	vec_ushort gap_array;
	vec_string compressed_output;
	int frequencies[processor_count][TOTAL_CHARS];
	int i, j;
	
	fptr = fopen(input_file_name.c_str(), "r");
	if(fptr == NULL){
		cout << "Unable to open file - " << input_file_name << endl;
		exit(EXIT_FAILURE);
	}

	memset(frequencies, 0, sizeof(frequencies));

	original_file_size = __concurrentCountCharFrequency__(fptr, &frequencies, processor_count);
	
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
	compressed_file_size_bits = populateHuffmanTable(&total_frequencies, rootNode, tempStr);

	cout << "Original File Size -> " << original_file_size << endl;
	cout << "Encoded file size (without headers) -> " << compressed_file_size_bits << "bits = ";
	cout << compressed_file_size_bits / 8 << "Bytes + " << compressed_file_size_bits%8 << "bits" << endl;

	map_char_to_string map;
	for(HuffNode *t: total_frequencies) { map[t->ch] = t->compressed; }
	
	__concurrentCompress__(input_file_name, &map, &compressed_output, &prefix_sum);
	puts("Compressed and calculated prefix sum");

	// ! Innefficient GAP array calculation
	// TODO: GAP Array can be calculated while merging the partial prefix sums
	int gap_segments = prefix_sum.back() / GAP_SEGMENT_SIZE;
	if(prefix_sum.back() % GAP_SEGMENT_SIZE >= 1){
		++gap_segments;
	}
	gap_array.resize(gap_segments);
	
	gap_array.at(0) = 0;
	i=1;
	cout << "gap segment size" << GAP_SEGMENT_SIZE << endl;
	for(int sum: prefix_sum){
		if(sum >= (i * GAP_SEGMENT_SIZE)){
			gap_array[i] = sum - (i * GAP_SEGMENT_SIZE);
			printf("gap - %d - %d = %d\n", sum, i * GAP_SEGMENT_SIZE, sum - (i * GAP_SEGMENT_SIZE));
			i++;
		}
	}puts("");

	

}
