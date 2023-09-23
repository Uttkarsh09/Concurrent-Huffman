#pragma once

#include <iostream>
using namespace std;

#ifndef CONFIG
#define CONFIG

/// Total number of characters 
#define TOTAL_CHARS 256

struct HuffNode{
	char ch;
	u_int32_t frequency;
	HuffNode *left, *right;
	short compressed_length;
	string compressed;

	HuffNode (u_int32_t count){
		this->ch = 0;
		this->frequency = count;
		this->left = this->right = nullptr;
		this->compressed_length = 0;
		this->compressed = "";
	}

	HuffNode (char ch, u_int32_t count){
		this->ch = ch;
		this->frequency = count;
		this->left = this->right = nullptr;
		this->compressed_length = 0;
		this->compressed = "";
	}
};

/// @brief Number of bits in each segment for encoding
#define SEGMENT_SIZE 512 

#endif