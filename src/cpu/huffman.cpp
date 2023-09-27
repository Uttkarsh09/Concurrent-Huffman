#include "huffman.hpp"
#include <cstring>

using namespace std;


HuffNode* Huffman::combineNodes(HuffNode* a, HuffNode* b){
	printf("Combining %c(%d) & %c(%d)\n", a->ch, a->frequency, b->ch, b->frequency);

	HuffNode* parent = new HuffNode((a?a->frequency:0) + (b?b->frequency:0));
	parent->left = b;
	parent->right = a;
	return parent;
}


HuffNode* Huffman::generateHuffmanTree(vector<HuffNode*> frequencies){
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
	
	for(HuffNode *freq : frequencies){
		cout << "char - " << freq->ch << " freq - " << freq->frequency << endl;
	}

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
			cout << rootNode->ch << "->" << rootNode->compressed << endl;
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
		cout << "Starting Thread = 	" << i <<endl;
		
		// ? Last thread will get the lenth till the end of file
		if(i == processor_count-1){
			cout << "Last thread" << endl << endl;
			cout << input_file_size_bytes << " % " << segment_size_bytes << "=" << input_file_size_bytes % segment_size_bytes << endl;
			if(segment_size_bytes == 1){
				length_to_read = input_file_size_bytes - (processor_count-1);
			} else {
				length_to_read += input_file_size_bytes % segment_size_bytes;
			}
		}
		read_offset = i * segment_size_bytes;
		cout << "Read Offset - " << read_offset << endl;
		cout << "Length To Read - " << length_to_read << endl;

		threads[i] = thread(
			__calculateFrequency__,  
			input_file_name, 
			read_offset,
			length_to_read,
			frequencies[i]
		);
	}

	for(i=0 ; i<processor_count ; i++) 
		threads[i].join();

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
	fseek(input_file_ptr, 0, SEEK_SET);
	u_int32_t segment_size = input_file_size_bytes / processor_count;
	u_int32_t offset=0, length_to_compress=segment_size;
	thread threads[processor_count];
	vec_char input_buffer;
	int i=0, j, gap_idx, gap_segments_count;
	char ch;

	while(!feof(input_file_ptr)) {
		ch = fgetc(input_file_ptr);
		if(!feof(input_file_ptr)) input_buffer.push_back(ch);
	}

	compressed_output.resize(input_buffer.size());
	prefix_sum.resize(input_buffer.size());

	cout << "Processor Cores -> " << processor_count <<endl;
	cout << "Input File size - " << input_file_size_bytes << endl;
	cout << "Segment size - " << segment_size << endl;
	puts("Starting the scheduler");

	for(i=0 ; i<processor_count ; i++){
		offset = i * segment_size;
		// cout << "offset -> " << offset << endl;
		if(i == processor_count - 1){
			length_to_compress += (input_file_size_bytes % processor_count);
		}
		// cout << "length to compress -> " << length_to_compress << endl;

		threads[i] = thread(
			__compress__, 
			&input_buffer, 
			&compressed_output, 
			&prefix_sum, 
			offset, 
			&char_encoding_map, 
			length_to_compress
		);
	}

	puts("Joining");
	for(i=0 ; i<processor_count ; i++){	
		threads[i].join();
	}


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

	puts("Combining the partial sum & calculating the gap array");

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

	puts("Completed");

	for(i=0 ; i<prefix_sum.size() ; i++){
		printf("%3d ", prefix_sum.at(i));
	}puts("");
}


void Huffman::mapCharacterToEncoding(){
	for(HuffNode *fs: frequency_sum) { 
		char_encoding_map[fs->ch] = fs->compressed; 
		cout << fs->ch << " = " << fs->compressed << endl;
	}
}


void Huffman::saveCompressedData(){
	
}