#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>

//this struct contains the name of a kmer, and pattern is a vector of its absence/presence
struct Kmer {
    std::string name;
    std::vector<int> pattern;
};

// declaration
Kmer process_line(const std::string& line_buffer);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "ERROR: usage : ./BinaryToText input.txt output.txt" << std::endl;
        exit(1);
    }

    // measuring time
    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Starting conversion of abundance matrix to presence/absence matrix ..." << std::endl;
    std::string matrix = argv[1]; //input abundance matrix
    std::string output = argv[2]; //output presence/absence matrix
    std::ifstream stream (matrix, std::ifstream::binary);
    std::ofstream outstream (output, std::ofstream::binary);
    size_t lastindex = output.find_last_of(".");
    std::string rawname = output.substr(0, lastindex);
    std::ofstream outstream_unique (rawname+"_unique.txt", std::ofstream::binary);
    // must use stream.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    if (stream.fail()) {
        perror("ERROR: could not open input file");
        std::exit(1);
    } else if (outstream.fail()) {
        perror("ERROR: could not open output file");
        std::exit(1);
    } else if (outstream_unique.fail()) {
        perror("ERROR: could not open output_unique file");
        std::exit(1);
    }
    std::vector<Kmer> vector_of_kmers;
    std::vector<std::vector<int>> vector_of_unique_patterns ;

    //read data by chunks
    while(stream.good() and outstream.good()) {
        // dont use !stream.eof() because it won't reach
        // the endOfFile as long as we did not actually read it until
        // read the data line by line:
        std::string line_buffer;
        while(std::getline(stream, line_buffer).good()) {
            vector_of_kmers.push_back(process_line(line_buffer));
            for (auto &i : vector_of_unique_patterns) {
                if (i != vector_of_kmers.end()->pattern) {
                    vector_of_unique_patterns.push_back(vector_of_kmers.end()->pattern);
                }
            }
        }
    }
    stream.close();

    //iterate with iterator on vectorOfKmer
    outstream << "ps\t";
    outstream_unique << "ps\t";
    for (const Kmer &i : vector_of_kmers) {
        outstream << i.name << "\t";
        outstream_unique << i.name << "\t";
    }
    outstream << "\n" ;
    // gets the number of lines that will be written, which corresponds to the number of 0/1 in the vector pattern of the structures
    for (int i = 0; i < vector_of_kmers.at(1).pattern.size(); ++i) {
        outstream << i << "\t" ;
        //go through the ieme values of each vector
        for (auto j: vector_of_kmers) {
            outstream << j.pattern.at(i) << "\t";
        }
        outstream << "\n" ;
    }
    outstream.close();

    // gets the number of lines that will be written, which corresponds to the number of 0/1 in the pattern of each vector
    // major diffference with above is that we iterate directly on the pattern vectors instead of the structures containing them
    for (int i = 0; i < vector_of_unique_patterns.at(1).size(); ++i) {
        outstream_unique << i << "\t" ;
        //go through the ieme values of each vector
        for (auto j: vector_of_unique_patterns) {
            outstream_unique << j.at(i) << "\t";
        }
        outstream_unique << "\n" ;
    }

    //finishing measuring time
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::minutes>(t2 - t1);
    std::cout << "Conversion took " << ms_int.count() << "min\n";
    return 0;
}

// definition
Kmer process_line(const std::string& line_buffer) {
    /*
     * this function processes lines by putting them in a structure than contains  its name, and a vector of its absence/presence pattern.
     */
    std::vector<int> output_pattern;
    std::string kmer_name;
    std::istringstream input(line_buffer);
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
    //Kmer output_struct{kmer_name, output_pattern};
    //output_pattern.clear();
    return {kmer_name, output_pattern};
}