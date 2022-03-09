#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>

std::string process_line(const std::string& line_buffer);

struct kmer {
    std::string name;
    std::vector<int> pattern;
};

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
        std::string line;
        while(std::getline(stream, line_buffer).good()) {
            kmer kmer_buffer = process_line(line_buffer);
            vectorOfKmers.push_back(kmer_buffer);
            //outstream << line << std::endl;
        }
    }
    stream.close()

    //iterate with iterator on vectorOfKmer

    outstream.close();

    //finishing measuring time
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::minutes>(t2 - t1);
    std::cout << "Conversion took " << ms_int.count() << "min\n";
    return 0;
}

std::string process_line(const std::string& line_buffer) {
    /*
     * this function processes lines by puting them in a structure than contains  its name, and a vector of its absence/presence pattern.
     */
    std::vector<int> output_pattern;
    output_pattern.clear();
    std::istringstream input;
    input.str(line_buffer);
    //loop over tab-separated words in the line :
    for (std::string word; std::getline(input, word, '\t'); ) {
        if (word.starts_with(">")) { // if id resets line
            std::string kmer_name = word;
        } else if (word.ends_with("*")) { // if abundance is 0
            output_pattern.push_back(0);
        } else {
            output_pattern.push_back(1); // if abundance is anything else than 0
        }
    }
    kmer output_struct{kmer_name, output_pattern};
    return output_struct;
}