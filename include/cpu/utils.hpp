#include "configuration.hpp"
using namespace std;

bool sortByCharFreq(const HuffNode* a, const HuffNode* b);

void openFile(FILE **fptr, string file_path, string access_modifier);