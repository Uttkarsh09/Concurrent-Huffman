#include <iostream>
#include <vector>
#include <thread>

// Function to compute prefix sum for a portion of the input vector
void prefixSumPartial(std::vector<int>& input, int start, int end) {
    for (int i = start + 1; i <= end; i++) {
        input[i] += input[i - 1];
    }
}

// Function to perform parallel prefix sum using threads
void parallelPrefixSum(std::vector<int>& input, int numThreads) {
    int n = input.size();
    int chunkSize = n / numThreads;

    // Create and start threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
		// ! VERIFY 
        int end = (i == numThreads - 1) ? (n - 1) : (start + chunkSize - 1);
        threads.emplace_back(prefixSumPartial, std::ref(input), start, end);
    }

    // Wait for all threads to finish
    for (std::thread& thread : threads) {
        thread.join();
    }

    // Perform the final step to adjust for the chunks
    for (int i = 1; i < numThreads; i++) {
        int offset = i * chunkSize;
        for (int j = offset; j < n; j++) {
            input[j] += input[offset - 1];
        }
    }
}

int main() {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8};
    int numThreads = 4; // Adjust the number of threads as needed

    // Print the original data
    std::cout << "Original Data: ";
    for (int val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Compute parallel prefix sum
    parallelPrefixSum(data, numThreads);

    // Print the prefix sum result
    std::cout << "Prefix Sum: ";
    for (int val : data) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}
