#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <map>
#include <unordered_set>

//this struct contains the name of a kmer, and pattern is a vector of its absence/presence
struct Kmer {
    std::string name;
    std::vector<int> pattern;
    // This function is used by unordered_set to compare
    // elements of Kmer.
    bool operator==(const Kmer& K) const
    {
        return (this->pattern == K.pattern);
    }
};

// class for hash function
class MyHashFunction {
public:
    // pattern is returned as hash function
    std::vector<int> operator()(const Kmer& K) const
    {
        return K.pattern;
    }
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
    // reads the input data
    while(stream.good() and outstream.good()) {
        // dont use !stream.eof() because it won't reach
        // the endOfFile as long as we did not actually read it until
        // read the data line by line:
        std::string line_buffer;
        while(std::getline(stream, line_buffer).good()) {
            vector_of_kmers.push_back(process_line(line_buffer));
        }
    }
    stream.close();

    // obtains a vector of unique patterns in the same order as the read order
    // first we build a map: each kmer and its position in the original order, and the reverse (index to kmer)
    std::vector<std::vector<int>> vector_of_unique_patterns ;
    std::vector<int> index_vector;
    auto comp = [](const Kmer& k1, const Kmer& k2){
        return k1.pattern < k2.pattern || (k1.pattern == k2.pattern && k1.name < k2.name);
    };
    std::map<Kmer, int, decltype(comp)> kmers_to_index(comp);
    std::map<int, Kmer> index_to_kmers;
    for (int i = 0; i != vector_of_kmers.size(); i++) {
        kmers_to_index.insert(std::pair<Kmer, int>(vector_of_kmers.at(i), i));
        index_to_kmers.insert(std::pair<int, Kmer>(i, vector_of_kmers.at(i)));
    }
    // then we build an unsorted_set of the Kmers from the input:
    std::unordered_set<Kmer, MyHashFunction> vector_set(vector_of_kmers.begin(), vector_of_kmers.end());
    // builds index of original positions of the unique vectors
    for (auto itr : vector_set) {
        index_vector.push_back(kmers_to_index[itr]);
    }
    // sorts index vector to get the indexes in order
    std::sort(index_vector.begin(), index_vector.end());
    // populates the vector of unique presence/absence patterns :
    for (int i = 0; i != index_vector.size(); i++) {
        vector_of_unique_patterns.at(i) = index_to_kmers[index_vector.at(i)].pattern;
    }

    // iterates with iterator on vectorOfKmer
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
        // goes through the ieme values of each vector
        for (auto j: vector_of_kmers) {
            outstream << j.pattern.at(i) << "\t";
        }
        outstream << "\n" ;
    }

    // gets the number of lines that will be written, which corresponds to the number of 0/1 in the pattern of each vector
    // major difference with above is that we iterate directly on the pattern vectors instead of the structures containing them
    for (int i = 0; i < vector_of_unique_patterns.at(1).size(); ++i) {
        outstream_unique << i << "\t" ;
        //go through the ieme values of each vector
        for (auto j: vector_of_unique_patterns) {
            outstream_unique << j.at(i) << "\t";
        }
        outstream_unique << "\n" ;
    }
    outstream_unique.close() ;
    outstream.close();

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