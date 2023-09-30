#include "huffman.hpp"
#include <cstring>
#include "logger.hpp"
#include "utils.hpp"

using namespace std;


HuffNode* Huffman::combineNodes(HuffNode* a, HuffNode* b){
	// printf("Combining %c(%d) & %c(%d)\n", a->ch, a->frequency, b->ch, b->frequency);

	HuffNode* parent = new HuffNode((a?a->frequency:0) + (b?b->frequency:0));
	parent->left = b;
	parent->right = a;
	return parent;
}


HuffNode* Huffman::generateHuffmanTreeForCompression(vector<HuffNode*> frequencies){
	HuffNode *one, *two, *parent;

	sort(frequencies.begin(), frequencies.end(), sortByCharFreq);
	if(frequencies.size() == 1){
		return combineNodes(frequencies.back(), nullptr);
	}
	
	while(frequencies.size() > 2){
		one = *(frequencies.end() - 1);
		two = *(frequencies.end() - 2);
		parent = combineNodes(one, two);
		frequencies.pop_back();
		frequencies.pop_back();
		frequencies.push_back(parent);
		
		vector <HuffNode*>::iterator itr = frequencies.end() - 2;
		while(((*itr)->frequency < parent->frequency) && (itr != frequencies.begin())){
			--itr;
		}
		sort(itr, frequencies.end(), sortByCharFreq);
	}

	one = *(frequencies.end() - 1);
	two = *(frequencies.end() - 2);
	
	// for(HuffNode *freq : frequencies){
	// 	cout << "char - " << freq->ch << " freq - " << freq->frequency << endl;
	// }

	return combineNodes(one, two);
}


u_int32_t Huffman::populateHuffmanTable(HuffNode* rootNode, string &value){
	u_int32_t temp = 0;

	if(rootNode){
		value.push_back('0');
		temp = populateHuffmanTable(rootNode->left, value);
		value.pop_back();

		if(!rootNode->left && !rootNode->right){
			rootNode->compressed = value;
			temp += value.size() * rootNode->frequency;
			// cout << rootNode->ch << "->" << rootNode->compressed << endl;
		}

		value.push_back('1');
		temp += populateHuffmanTable(rootNode->right, value);
		value.pop_back();
	}

	return temp;
}


void Huffman::concurrentCountCharFrequency(){
	short i;
	u_int32_t segment_size_bytes, length_to_read, count, read_offset;
	u_int32_t frequencies[processor_count][TOTAL_CHARS];
	thread threads[processor_count];
	segment_size_bytes = input_file_size_bytes / processor_count;
	length_to_read = segment_size_bytes;

	memset(frequencies, 0, sizeof(frequencies));

	for(i=0 ; i<processor_count ; i++){
		// cout << "Starting Thread = 	" << i <<endl;
		
		// ? Last thread will get the lenth till the end of file
		if(i == processor_count-1){
			// cout << "Last thread" << endl << endl;
			// cout << input_file_size_bytes << " % " << segment_size_bytes << "=" << input_file_size_bytes % segment_size_bytes << endl;
			if(segment_size_bytes == 1){
				length_to_read = input_file_size_bytes - (processor_count-1);
			} else {
				length_to_read += input_file_size_bytes % segment_size_bytes;
			}
		}
		read_offset = i * segment_size_bytes;
		// cout << "Read Offset - " << read_offset << endl;
		// cout << "Length To Read - " << length_to_read << endl;

		threads[i] = thread(
			__calculateFrequency__,  
			input_file_name, 
			read_offset,
			length_to_read,
			frequencies[i]
		);
	}

	joinThreads(processor_count, threads);

	for(j=0 ; j<TOTAL_CHARS ; j++){
		count = 0;
		for(i=0 ; i<processor_count ; i++)
			count += frequencies[i][j];

		if(count > 0)
			frequency_sum.push_back(new HuffNode((char)j, count));
	}

	rewind(input_file_ptr);
}


void Huffman::concurrentCompress(){
	// const short processor_count = 27;
	vec_uint prefix_sum;
	fseek(input_file_ptr, 0, SEEK_SET);
	u_int32_t segment_size = input_file_size_bytes / processor_count;
	u_int32_t offset=0, length_to_compress=segment_size, length_to_read = segment_size;
	thread threads[processor_count];
	vec_char input_buffer[processor_count];
	int i=0, j, gap_idx, gap_segments_count;
	char ch;

	// puts("Reading input file for compression");
	// while(!feof(input_file_ptr)) {
	// 	ch = fgetc(input_file_ptr);
	// 	if(!feof(input_file_ptr)) input_buffer.push_back(ch);
	// }

	offset = 0;
	for(i=0 ; i<processor_count ; i++){
		offset = i * segment_size;
		if(i == processor_count - 1){
			length_to_read += (input_file_size_bytes % processor_count);
		}
		threads[i] = thread(
			__readFile__,
			input_file_name,
			offset,
			length_to_read,
			&input_buffer[i]
		);
	}

	joinThreads(processor_count, threads);
	// puts("READ");

	u_int32_t total_charactes = 0;
	for(i=0 ; i<processor_count ; i++){
		total_charactes += input_buffer[i].size();
	}

	compressed_output.resize(total_charactes);
	prefix_sum.resize(total_charactes);

	// cout << "Processor Cores -> " << processor_count <<endl;
	// cout << "Input File size - " << input_file_size_bytes << endl;
	// cout << "Segment size - " << segment_size << endl;
	// puts("Starting the scheduler");

	for(i=0 ; i<processor_count ; i++){
		offset = i * segment_size;
		// cout << "offset -> " << offset << endl;
		if(i == processor_count - 1){
			length_to_compress += (input_file_size_bytes % processor_count);
		}
		// cout << "length to compress -> " << length_to_compress << endl;

		threads[i] = thread(
			__compress__, 
			&input_buffer[i], 
			&compressed_output, 
			&prefix_sum, 
			offset, 
			&char_encoding_map, 
			length_to_compress
		);
	}

	// puts("Joining");
	joinThreads(processor_count, threads);



	gap_idx=1;
	gap_segments_count = (compressed_file_size_bits / GAP_SEGMENT_SIZE);  // +1 because the first one is by default 0
	if((compressed_file_size_bits % GAP_SEGMENT_SIZE) >= 1){
		++gap_segments_count;
	}
	gap_array.resize(gap_segments_count);
	gap_array.at(0) = 0;

	for(int i=0 ; i<segment_size ; i++){
		if(prefix_sum.at(i) >= (gap_idx * GAP_SEGMENT_SIZE)){
			gap_array[gap_idx] = prefix_sum.at(i) - (gap_idx * GAP_SEGMENT_SIZE);
			gap_idx++;
		}
	}

	// puts("Combining the partial sum & calculating the gap array");

	// Note: I don't know whether i'll remember, why ive done, what ive done while explaining :(
	for(i=1 ; i<processor_count ; i++){
		offset = i * segment_size;
		// cout << "i -> " << i << " Offset -> " << offset << endl;
		for(j=offset ; j<offset+segment_size ; j++){
			if(i >= processor_count-1){
				segment_size = length_to_compress;
			}
			// cout << "j -> " << j<< endl; 
			prefix_sum.at(j) += prefix_sum.at(offset - 1);
			if(prefix_sum.at(j) >= (gap_idx * GAP_SEGMENT_SIZE)){
				gap_array[gap_idx] = prefix_sum.at(j) - (gap_idx * GAP_SEGMENT_SIZE);
				gap_idx++;
			}
		}
	}

	// puts("Completed");

	// for(i=0 ; i<prefix_sum.size() ; i++){
	// 	printf("%3d ", prefix_sum.at(i));
	// }puts("");
}


void Huffman::mapCharacterToEncoding(){
	for(HuffNode *fs: frequency_sum) { 
		char_encoding_map[fs->ch] = fs->compressed; 
		// cout << fs->ch << " = " << fs->compressed << endl;
	}
}


void Huffman::saveCompressedData(){
	FILE *output_file_ptr;
	short padding = (8 - ((compressed_file_size_bits) & (7))) & (7);
	string output_header_buffer = "";
	u_int32_t count = 0, j = 0;
    unsigned char unique_characters = 255;
	unsigned char bin_representation;
	string gap_header_buffer = "";
	// Logger *l = Logger::getInstance("./logs/log.txt");

	rewind(input_file_ptr);
	openFile(&output_file_ptr, input_file_name.append(".huff"), "wb");

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HEADER ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	// puts("Generating header");
    // ? UniqueCharacter start from -1 {0 means 1, 1 means 2, to conserve memory}
	for(int i=0 ; i<frequency_sum.size() ; i++){
		output_header_buffer.push_back((int)frequency_sum[i]->ch);
		output_header_buffer.push_back(frequency_sum[i]->compressed.length());
		output_header_buffer += frequency_sum[i]->compressed;
		++unique_characters;
	}
	// cout << "Unique Characters = " << (int)unique_characters << endl;
	// output_header_buffer = (char)unique_characters + output_header_buffer + (char)padding;
	output_header_buffer = ((char)unique_characters) + output_header_buffer;
	// puts("Header Generated\n");

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ GAP ARRAY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// puts("Generating Gap Header");
	u_int32_t gap_array_size = gap_array.size();
	// cout << "Gap array size = " << gap_array_size << endl;
	
	gap_header_buffer += encodeNumberLength(gap_array_size);
	// cout << "gap header buffer len = " << gap_header_buffer.size() << endl;
	// cout << "gap header buffer = " << gap_header_buffer << endl;

	// ? Only go in loop for >=2 elements. and if odd then exclude the last one.
	// ? if gap_array_size = 1 won't go in loop
	// ? if gap_array_size = 2 loop will run only 1 time
	// ? if gap_array_size = 3 loop will run only 1 time and then following if will be true
	count = 0;
	gap_array_size = gap_array.size();
	while((count < gap_array_size-1) && (gap_array_size!=1)){
		bin_representation = 0;
		for(j=0 ; j<2 ; j++){
			if(j == 0){
				bin_representation = gap_array[count+j];
			}
			else {
				bin_representation = bin_representation << 3;
				bin_representation = bin_representation | gap_array[count+j];
				bin_representation = bin_representation << 2;
			}
		}
		gap_header_buffer.push_back((char)bin_representation);
		count+=2;
	}
	// cout << "gap header buffer len = " << gap_header_buffer.size() << endl;

	if(gap_array_size%2 == 1){
		gap_header_buffer.push_back((char)gap_array[0]);
	}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	string segment_header_buffer = "";
	segment_header_buffer += encodeNumberLength((u_int32_t)GAP_SEGMENT_SIZE);
	// cout << "gap header buffer len = " << segment_header_buffer.size() << endl;
	// puts("Gap Header Generated\n");

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	output_header_buffer += gap_header_buffer + segment_header_buffer;

	// puts("Writing Header");
	// cout << "output_header_buffer len = " << output_header_buffer.size() << endl;
	// cout << output_header_buffer << endl;

	// cout << "appending GAP HEADER BUFFER " << endl;
	output_header_buffer += gap_header_buffer + ((char)padding);
	// cout << "output_header_buffer len = " << output_header_buffer.size() << endl;
	// cout << output_header_buffer << endl;
	
	count = 0;
	while(count < output_header_buffer.size()){
		fputc(output_header_buffer[count++], output_file_ptr);
	}
	// puts("Header Written");

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	unsigned char ch, fch = 0;
	char counter = 7;
	u_int32_t size = 0, i;
	
	while(size != input_file_size_bytes){
		ch = fgetc(input_file_ptr);
		// cout << "read -> " << ch;
		i=0;
		const string &huffmanStr = char_encoding_map[ch];
		// cout << " - " << huffmanStr << endl;

		while(huffmanStr[i] != '\0'){
			// cout << "fch -> " << (int)fch << " | counter -> " << (int)counter << endl ;
			// cout << (int)(huffmanStr[i]-'0')<< endl;
			fch = fch | ((huffmanStr[i] - '0') << counter);
			counter = (counter + 7) & 7; // reduces the counter by 1
			if(counter == 7){
				// cout <<"writing -> " << (int)fch << endl;
				fputc(fch, output_file_ptr);
				fch ^= fch;
			}
			++i;
		}
		++size;
		// if((size * 100 / input_file_size_bytes) > ((size - 1) * 100 / input_file_size_bytes)) {
        //     printf("\r%lld%% completed  ", (size * 100 / input_file_size_bytes));
        // }
	}

	if(fch) {
		// cout << "LAST WRITE -> " << (int)fch << endl;
        fputc(fch, output_file_ptr);
    }

	fclose(output_file_ptr);
}


// void Huffman::generateHuffmanTreeForDecompression(const string encoding, const unsigned char ch){
// 	HuffNode *traverse = this->rootNode;
// 	int i = 0;
// 	while(encoding[i] != '\0'){
// 		if(encoding[i] == '0'){
// 			if(!traverse->left){
// 				traverse->left = new HuffNode(0);
// 			}
// 			traverse = traverse->left;
// 		}
// 		else {
// 			if(!traverse->right){
// 				traverse->right = new HuffNode(0);
// 			}
// 			traverse = traverse->right;
// 		}
// 		++i;
// 	}
// 	traverse->ch = ch;
// }


// void Huffman::decodeCompressedHeader(){
// 	this->rootNode = new HuffNode(0);
// 	int total_characters, buffer, total_length = 1;
// 	char current_char, char_encoding_length;
// 	string encoding;
	
// 	total_characters = fgetc(this->input_file_ptr);
// 	++total_characters; // as 0=1, 1=2 as per the compression logic, hence adding 1 to get the actual count

// 	while(total_characters){
// 		encoding = "";
// 		current_char = fgetc(this->input_file_ptr);
// 		char_encoding_length = fgetc(this->input_file_ptr);
// 		buffer = char_encoding_length;

// 		while(buffer > encoding.size()){
// 			encoding.push_back(fgetc(this->input_file_ptr));
// 		}

// 		total_length += encoding.size();

// 		this->generateHuffmanTreeForDecompression(encoding, current_char);
// 		--total_characters;
// 	}

		
// }


// void Huffman::concurrentDecompress(){
	
// }