#include <iostream>
#include "configuration.hpp"
#include "cpuHuffman.hpp"
#include <thread>
#include "logger.hpp"
#include "helper_timer.h"

using namespace std;

Logger *Logger::l_instance = NULL;

int main(){
	// compressOnCPU("./docs/test.txt");
	compressOnCPU("/home/uttkarsh/VIT/4th_Year/Major_Project/Implementation_Guides/Implementations/data/random.txt");

	return 0;
}