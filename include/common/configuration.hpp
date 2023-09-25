#pragma once

#ifndef CONFIG
#define CONFIG

#include <iostream>
#include<vector>
#include<unordered_map>
using namespace std;

/// Total number of characters 
#define TOTAL_CHARS 256
#define TWO_GB 2^30

typedef unordered_map<char, string> map_char_to_string ;
typedef vector<char> vec_char;
typedef vector<string> vec_string;
typedef vector<int> vec_int;
typedef vector<unsigned short> vec_ushort;

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

	#if TEST
		/// @brief Number of bits in each segment for encoding
		#define GAP_SEGMENT_SIZE 16
	#else
		/// @brief Number of bits in each segment for encoding
		#define GAP_SEGMENT_SIZE 512 
	#endif

#endif 