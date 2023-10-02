#include <algorithm>
#include <thread>
#include <stdlib.h>
#include <filesystem>
#include "configuration.hpp"
#include "cpuKernels.hpp"
#include "utils.hpp"
#include <array>
	
using namespace std;

// TODO: Initialize frequencies arr in constructor

class Huffman{
	const int processor_count = thread::hardware_concurrency();
	// const int processor_count = 1000;
	string input_file_name;
	FILE *input_file_ptr;
	int i, j;

	public:
		vector<HuffNode*> frequency_sum;
		vec_ushort gap_array;
		vec_string compressed_output;
		u_int32_t input_file_size_bytes, compressed_file_size_bits;
		map_char_to_string char_encoding_map;

		/// @brief Constructor
		/// @param input_file_name input file name. 
		/// During compression `input_file_name` refers to raw original file.
		/// And during decompression it is the compressed file.
		Huffman(string input_file_name){
			this->input_file_name = input_file_name;
			openFile(&input_file_ptr, input_file_name, "rb");
			input_file_size_bytes = std::filesystem::file_size(input_file_name);
			// cout << "Input File size = " << input_file_size_bytes << endl;
		}
		~Huffman(){
			fclose(input_file_ptr);
		}

		/// @brief Generating Huffman Tree ON COPY OF THE FREQUENCY VECTOR.
		/// @param frequencies Vector (pass by value, not reference) of HuffNode populated with character frequencies.
		/// @return Parent Node of the huffman tree.
		HuffNode* generateHuffmanTreeForCompression(vector<HuffNode*> frequencies);
		
		/// @brief 
		/// @param frequencies HuffNode Vector arr.
		/// @param rootNode The root node of the Huffman Tree.
		/// @param value Intermediate variable for recursive logic.
		/// @return the size of the encoded text in bits.
		u_int32_t populateHuffmanTable(HuffNode* rootNode, string &value);

		/// @brief CPU KERNEL - Reads files concurrently and populates the frequency array.
		/// @param fptr File pointer.
		/// @param frequencies 2 dimentional integer array to store frequencies of all characters from all threads.
		/// @param processor_count Number of processor.
		/// @return The size of file in bytes.
		void concurrentCountCharFrequency();

		/// @brief compress file concurently into buffers
		void concurrentCompress();
		
		/// @brief maps the characters to their respective compressed bits
		void mapCharacterToEncoding();

		void saveCompressedData();

		/// @brief Generates a simpler huffman tree compared to the one during compression
		/// @param root The root node of the tree
		/// @param encoding The binary code string to construct the tree from
		/// @param ch The character for which the binary compressed encoding is.
		void generateHuffmanTreeForDecompression(const string encoding, const unsigned char ch);

		void decodeCompressedHeader();

		void concurrentDecompress();
	// Methods
	private:
		HuffNode* combineNodes(HuffNode* a, HuffNode* b);
		// bool sortByCharFreq(const HuffNode* a, const HuffNode* b);
};