#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>

struct kmer {
    std::string name;
    std::vector<int> pattern;
};

kmer process_line(const std::string& line_buffer);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "ERROR: invalid number of arguments" << std::endl;
        exit(1);
    }

    //measuring time
    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Starting conversion of abundance matrix to presence/absence matrix ..." << std::endl;
    std::string();
    std::string matrix = argv[1]; //input abundance matrix
    std::string output = argv[2]; //output presence/absence matrix
    std::ifstream stream (matrix, std::ifstream::binary);
    std::ofstream outstream (output, std::ofstream::binary);
    std::vector<kmer> vectorOfKmers;

    //must use stream.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    if (stream.fail()) {
        perror("ERROR: could not open input file");
        std::exit(1);
    } else if (outstream.fail()) {
        perror("ERROR: could not open output file");
        std::exit(1);
    }

    //read data by chunks
    while(stream.good() and outstream.good()) {
        //dont use !stream.eof() because it won't reach
        // the endOfFile as long as we did not actually read it until
        //read the data line by line:
        std::string line_buffer;
        while(std::getline(stream, line_buffer).good()) {
            kmer kmer_buffer ;
            kmer_buffer = process_line(line_buffer); //error: conversion from ‘std::string’ {aka ‘std::__cxx11::basic_string<char>’} to non-scalar type ‘kmer’ requested
            vectorOfKmers.push_back(kmer_buffer);
        }
    }
    stream.close();

    //iterate with iterator on vectorOfKmer
    for (auto i : vectorOfKmers) {
        outstream << i.name << "\t" ;
    }
    outstream << "\n" ;
    for (int i = 0; i < vectorOfKmers.at(1).pattern.size(); ++i) { //gets the number of lines that will be written, which corresponds to the number of 0/1 in the vector pattern of the structures
        std::cout << i << std::endl;
        //go through the ieme values of each vector
        for (auto j: vectorOfKmers) {
            std::cout << j.pattern.at(i) << std::endl;
            outstream << j.pattern.at(i) << "\t";
        }
        outstream << "\n" ;
    }

    outstream.close();

    //finishing measuring time
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::minutes>(t2 - t1);
    std::cout << "Conversion took " << ms_int.count() << "min\n";
    return 0;
}

kmer process_line(const std::string& line_buffer) {
    /*
     * this function processes lines by puting them in a structure than contains  its name, and a vector of its absence/presence pattern.
     */
    std::vector<int> output_pattern;
    std::istringstream input;
    std::string kmer_name;
    input.str(line_buffer);
    //loop over tab-separated words in the line :
    for (std::string word; std::getline(input, word, '\t'); ) {
        if (word.starts_with(">")) { // if id resets line
            kmer_name = word;
        } else if (word.ends_with("*")) { // if abundance is 0
            output_pattern.push_back(0);
        } else {
            output_pattern.push_back(1); // if abundance is anything else than 0
        }
    }
    kmer output_struct{kmer_name, output_pattern};
    output_pattern.clear();
    return output_struct;
}