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

	puts("Calculating char freq");

	sdkStartTimer(&timer);
	huff.concurrentCountCharFrequency();
	sdkStopTimer(&timer);
	cout << "Time count freq : " << sdkGetTimerValue(&timer) << " ms" << endl;
	sdkResetTimer(&timer);
	puts("finished calculating char freq\n");

	puts("Generating HUffman Tree");
	sdkStartTimer(&timer);
	HuffNode *rootNode = huff.generateHuffmanTreeForCompression(huff.frequency_sum);
	sdkStopTimer(&timer);	
	cout << "Huffman Tree : " << sdkGetTimerValue(&timer) << " ms" << endl;
	sdkResetTimer(&timer);
	puts("Generated tree\n");

	string tempStr = "";
	puts("populating huffman table");
	huff.compressed_file_size_bits = huff.populateHuffmanTable(rootNode, tempStr);
	sdkStopTimer(&timer);	
	cout << "Time table population : " << sdkGetTimerValue(&timer) << " ms" << endl;
	sdkResetTimer(&timer);
	puts("table populated\n");
	
	// cout << "Original File Size -> " << huff.input_file_size_bytes << endl;
	// cout << "Encoded file size (without headers) -> " << huff.compressed_file_size_bits << "bits = ";
	// cout << huff.compressed_file_size_bits / 8 << "Bytes + " << huff.compressed_file_size_bits%8 << "bits" << endl;

	puts("mapping characters to encoding");
	huff.mapCharacterToEncoding();
	puts("mapped");
	
	puts("compressing");
	sdkStartTimer(&timer);
	huff.concurrentCompress();
	sdkStopTimer(&timer);	
	puts("compressed");

	cout << "Time Taken on CPU : " << sdkGetTimerValue(&timer) << " ms" << endl;
	sdkDeleteTimer(&timer);

	puts("writing data");
	huff.saveCompressedData();
	puts("written");
}
