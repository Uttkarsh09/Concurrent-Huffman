#include "configuration.hpp"
#include <thread>

using namespace std;

bool sortByCharFreq(const HuffNode* a, const HuffNode* b);

void openFile(FILE **fptr, string file_path, string access_modifier);

/// @brief Converts the number to binary (short) number.
/// @param number Number to be converted, guranteed to be <=7
/// as it is used to convert gap array number into binary.
// Hence max no of bits will be 3
/// @return binary number, eg 6 is returned as 110 (short)
short convertToBinary(short number);

/// @brief Returns the number of digits 
/// @param number number to count digits of
/// @return 
int digitCount(int number);

int reverseNumber(int num);

/// @brief  Returns a string encoded in binary to store length, bit more complex than that 
/// @param length
/// @return 
string encodeNumberLength(u_int32_t length);

void joinThreads(int size, thread *thread_arr);