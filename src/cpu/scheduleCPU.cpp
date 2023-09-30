#include "configuration.hpp"
#include "huffman.hpp"
#include <thread>
#include "logger.hpp"
#include "helper_timer.h"

using namespace std;

void compressOnCPU(string input_file_name){
	StopWatchInterface* timer = NULL;
	sdkCreateTimer(&timer);
	Huffman huff(input_file_name);
	Logger *Log = Logger::getInstance("./logs/log.txt");

	huff.concurrentCountCharFrequency();

	HuffNode *rootNode = huff.generateHuffmanTreeForCompression(huff.frequency_sum);
	string tempStr = "";
	huff.compressed_file_size_bits = huff.populateHuffmanTable(rootNode, tempStr);
	
	// cout << "Original File Size -> " << huff.input_file_size_bytes << endl;
	// cout << "Encoded file size (without headers) -> " << huff.compressed_file_size_bits << "bits = ";
	// cout << huff.compressed_file_size_bits / 8 << "Bytes + " << huff.compressed_file_size_bits%8 << "bits" << endl;

	huff.mapCharacterToEncoding();
	
	sdkStartTimer(&timer);
	huff.concurrentCompress();
	sdkStopTimer(&timer);	

	cout << "Time Taken on CPU : " << sdkGetTimerValue(&timer) << " ms" << endl;
	sdkDeleteTimer(&timer);

	// puts("Compressed and calculated prefix sum");
	huff.saveCompressedData();
}
