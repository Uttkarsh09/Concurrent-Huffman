#include <iostream>
#include "configuration.hpp"
#include "cpuHuffman.hpp"
#include <thread>

using namespace std;

int main(){
	compressOnCPU("./docs/test.txt", "./docs/test.txt.huff");

	return 0;
}