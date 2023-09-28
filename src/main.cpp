#include <iostream>
#include "configuration.hpp"
#include "cpuHuffman.hpp"
#include <thread>
#include "logger.hpp"

using namespace std;

Logger *Logger::l_instance = NULL;

int main(){
	compressOnCPU("./docs/test.txt");

	return 0;
}