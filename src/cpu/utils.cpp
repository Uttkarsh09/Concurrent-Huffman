#include"utils.hpp"
#include <thread>

bool sortByCharFreq(const HuffNode* a, const HuffNode* b){
	return a->frequency > b->frequency;
}

void openFile(FILE **fptr, string file_path, string access_modifier){
	*fptr = fopen(file_path.c_str(), access_modifier.c_str());
	if(*fptr == NULL){
		cout << "CANNOT OPEN FILE - " << file_path << endl;
		exit(EXIT_FAILURE) ;
	}
}

short convertToBinary(unsigned short number){
	short binary_number = 0;
	short remainder;

	while(number > 0){
		remainder = number % 2;
		binary_number = ((binary_number << 1) & (remainder));
		number /= 2;	
	}

	return binary_number;
}

void joinThreads(int size, thread *thread_arr){
	for(int i=0 ; i<size ; i++){
		thread_arr[i].join();
	}
}

int digitCount(int number){
	int count = 0;
	while(number != 0){
		number /= 10;
		++count;
	}
	return count;
}

string reverseString(string str){
	string reverse = "";
	while(str.size() != 0){
		reverse += str.at(str.size()-1);
		str.pop_back();
	}

	return reverse;
}

string encodeNumberLength(u_int32_t length_gap_arr){
	string encoding = "";
	int length_of_gap_array_size = digitCount(length_gap_arr);
	unsigned char byte;
	string length_gap_arr_str = to_string(length_gap_arr); 
	string reverse_length_gap_arr_str = reverseString(length_gap_arr_str);
	int byte_count = 0, last_digit;

	if(length_of_gap_array_size%2 == 1){
		reverse_length_gap_arr_str+="0";
	}

	while(reverse_length_gap_arr_str.size() != 0){
		byte = byte << 8;
		
		last_digit = (reverse_length_gap_arr_str.at(reverse_length_gap_arr_str.size()-1)) - '0';
		reverse_length_gap_arr_str.pop_back();
		byte = byte | last_digit;
		// cout << "byte -> " << (int) byte << endl;

		byte = byte << 4;

		last_digit = (reverse_length_gap_arr_str.at(reverse_length_gap_arr_str.size()-1))-'0';
		reverse_length_gap_arr_str.pop_back();
		byte = byte | last_digit;
		// cout << "byte -> " << (int) byte << endl;

		encoding.push_back(byte);
		++byte_count;

		// cout << "reverse_length_gap_arr = " << reverse_length_gap_arr_str << endl; 
	}

	return (char)(byte_count) + encoding;
}