#include"utils.hpp"

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