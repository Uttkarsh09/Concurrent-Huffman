#include <iostream>
#include "configuration.hpp"
#include "cpuHuffman.hpp"
#include <thread>

using namespace std;

int main(){
	compressOnCPU("./docs/test.txt");

	return 0;
}