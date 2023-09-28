#include "configuration.hpp"
using namespace std;

bool sortByCharFreq(const HuffNode* a, const HuffNode* b);

void openFile(FILE **fptr, string file_path, string access_modifier);

/// @brief Converts the number to binary (short) number.
/// @param number Number to be converted, guranteed to be <=7
/// as it is used to convert gap array number into binary.
// Hence max no of bits will be 3
/// @return binary number, eg 6 is returned as 110 (short)
short convertToBinary(short number);