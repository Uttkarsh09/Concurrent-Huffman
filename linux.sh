executable_file="./bin/CPU_Huffman"

clear
rm $executable_file
g++ -o $executable_file ./src/cpu/*.cpp ./src/main.cpp -I./include/common/ -I./include/cpu/
$executable_file