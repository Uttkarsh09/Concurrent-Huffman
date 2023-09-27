#include "configuration.hpp"
#include "huffman.hpp"
#include <thread>

using namespace std;

void compressOnCPU(string input_file_name){
	Huffman huff(input_file_name);

	huff.concurrentCountCharFrequency();

	HuffNode *rootNode = huff.generateHuffmanTree(huff.frequency_sum);
	cout << "\nAfter Generating Huffman Tree" << endl;
	string tempStr = "";
	huff.compressed_file_size_bits = huff.populateHuffmanTable(rootNode, tempStr);
	
	cout << "Original File Size -> " << huff.input_file_size_bytes << endl;
	cout << "Encoded file size (without headers) -> " << huff.compressed_file_size_bits << "bits = ";
	cout << huff.compressed_file_size_bits / 8 << "Bytes + " << huff.compressed_file_size_bits%8 << "bits" << endl;

	huff.mapCharacterToEncoding();

	huff.concurrentCompress();
	puts("Compressed and calculated prefix sum");

	for(unsigned short &g: huff.gap_array){
		cout << g << " ";
	}
	cout<< endl;

	// huff.saveCompressedData();
}
