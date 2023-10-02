#include "configuration.hpp"

/// @brief Calculate the frequency by reading character by character .
/// @param start_ptr File pointer.
/// @param length Number of bytes to read from the start_ptr.
/// @param huffTable Pointer to array to store the counts.
void __calculateFrequency__(
	string file_name, 
	u_int32_t read_offset,
	u_int32_t length_to_read,
	u_int32_t *char_freq
);

/// @brief 
/// @param input_buffer_ptr 
/// @param output_buffer_ptr 
/// @param prefix_buffer_ptr 
/// @param offset 
/// @param map 
/// @param length 
void __compress__(vec_char *input_buffer_ptr, vec_uint *prefix_buffer_ptr, int offset, map_char_to_string *map, u_int32_t length);


void __readFile__(
	string file_name, 
	u_int32_t read_offset,
	u_int32_t length_to_read,
	vec_char *read_buffer
);