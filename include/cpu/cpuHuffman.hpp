#pragma once

#ifndef HUFF_FUNCTIONS
#define HUFF_FUNCTIONS
#include <iostream>
#include <vector>
#include "configuration.hpp"

using namespace std;

/// @brief Calculate the frequency by reading character by character .
/// @param start_ptr File pointer.
/// @param length Number of bytes to read from the start_ptr.
/// @param huffTable Pointer to array to store the counts.
void calculateFrequency(FILE *fptr, u_int32_t length_to_read, int *char_freq);

/// @brief 
/// @param fptr File pointer.
/// @param prefixSumArr Array pointer to store prefix sum.
void calculatePrefixSum(FILE *fptr, u_int32_t fileSize, u_int32_t *prefixSumArr);

/// @brief 
/// @param file_name Name of the file to compress.
void compressOnCPU(string input_file_name, string output_file_name);

/// @brief Generating Huffman Tree ON COPY OF THE FREQUENCY VECTOR.
/// @param frequencies Vector (pass by value, not reference) of HuffNode populated with character frequencies.
/// @return Parent Node of the huffman tree.
HuffNode* generateHuffmanTree(vector<HuffNode*> frequencies);

/// @brief 
/// @param frequencies HuffNode Vector arr.
/// @param rootNode The root node of the Huffman Tree.
/// @param value Intermediate variable for recursive logic.
/// @return the size of the encoded text in bits.
u_int32_t populateHuffmanTable(vector<HuffNode*> *frequencies, HuffNode* rootNode, string &value);

/// @brief Read files concurrently and populate the frequencies array.
/// @param fptr File pointer.
/// @param frequencies 2 dimentional integer array to store frequencies of all characters from all threads.
/// @param processor_count Number of processor.
/// @return The size of file in bytes.
u_int32_t concurrentReadFile(FILE *fptr, int (*frequencies)[][TOTAL_CHARS], u_int8_t processor_count);

/// @brief Calculate prefix sum on input vector concurrently.
/// @param vec Input vector to calculate prefix sum of.
/// @param prefixSums Resultant vector where the sums will be stored.
/// @param processor_count Number of CPU cores.
void concurrentPrefixSum(vector<HuffNode*> vec, vector<int> prefixSums, u_int8_t processor_count);

#endif