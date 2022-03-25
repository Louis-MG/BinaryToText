#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <set>
#include <map>

//this struct contains the name of a kmer, and pattern is a vector of its absence/presence
struct SKmer {
    std::string name;
    std::vector<int> pattern;
    int corrected;
};


// declarations
SKmer process_line(const std::string& line_buffer);
SKmer binarise_counts(SKmer& data);
SKmer minor_allele_description(SKmer& data);
void write_bugwas_gemma(const std::vector<std::vector<int>>& vector_of_unique_patterns, std::string& rawname, std::vector<std::string>& filenames, std::map<std::vector<int>, std::vector<int>>& map_unique_to_all);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "ERROR: usage : ./BinaryToText input.txt output.binary" << std::endl;
        exit(1);
    }

    // measuring time
    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Starting conversion of abundance matrix to presence/absence matrix ..." << std::endl;
    std::string matrix = argv[1]; //input abundance matrix
    std::string output = argv[2]; //output presence/absence matrix
    // streams
    std::ifstream stream (matrix, std::ifstream::binary);
    std::ofstream outstream (output, std::ofstream::binary);
    std::ofstream weight_corr_track ("weight_correction", std::ofstream::binary);
    // other files names prefix
    size_t lastindex = output.find_last_of(".");
    std::string rawname = output.substr(0, lastindex);
    // must use stream.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    if (stream.fail()) {
        perror("ERROR: could not open input file");
        std::exit(1);
    } else if (outstream.fail()) {
        perror("ERROR: could not open output file");
        std::exit(1);
    } else if (weight_corr_track.fail()) {
        perror("ERROR: could not open weight_correction output file");
        std::exit(1);
    }

    // variables
    std::vector<SKmer> vector_of_kmers;
    std::vector<std::vector<int>> vector_of_unique_patterns ;
    std::vector<std::string> filenames ;
    std::vector<std::vector<int>> unique_to_all ;
    std::map<std::vector<int>, std::vector<int>> map_unique_to_all ; //each time a unique is accountered, insert the pattern as a key and the n in the vector of values
    // reads the input data
    outstream << "ps ";
    // read the data line by line:
    std::string line_buffer;
    std::set<std::vector<int>> vector_set;
    int n = 0; // line counter
    while(std::getline(stream, line_buffer).good()) {
        if (line_buffer.starts_with("query")) {
            // we obtain the file names and store them:
            std::istringstream input(line_buffer);
            for (std::string word; std::getline(input, word, '\t'); ) {
                filenames.push_back(word);
            }
            // removes "query"
            filenames.erase(filenames.begin());
            // write the header of both all_rows and all_rows_unique :
            for (const std::string &i : filenames) {
                outstream << i << " ";
            }
            outstream << "\n" ;
        } else {
            // we write the body:
            // 1: parse the line and build the SKmer
            SKmer raw_data = process_line(line_buffer);
            // 2: changes abundance counts to presence/absence (0 stays 0 and more than 1 becomes 1)
            SKmer binarised_data = binarise_counts(raw_data);
            // 3: change, if needed, the allele description of the SKmer
            SKmer data = minor_allele_description(binarised_data);
            // 4: keep track of the change in allele description
            weight_corr_track << data.corrected << "\n";
            // next
            vector_of_kmers.push_back(data);
            outstream << n << " " ;
            for (const auto &i : data.pattern) {
                outstream << i << " " ;
            }
            outstream << "\n" ;
            // looks for the vector in the set of unique vectors :
            auto search = vector_set.find(data.pattern) ;
            // if not found, adds it to the vector of unique vectors
            if (search == vector_set.end()) {
                vector_of_unique_patterns.push_back(data.pattern);
                vector_set.emplace(data.pattern);
                std::vector<int> unitigs{n};
                map_unique_to_all.emplace(data.pattern, unitigs);
            } else {
                map_unique_to_all[data.pattern].push_back(n);
                // add n to the item (vector) pointed out above
            }
            n++;
        }
    }
    stream.close();
    outstream.close();
    weight_corr_track.close();

    // writes uniques and unique_to_all, gemma unique patterns to nb unitigs outputs
    write_bugwas_gemma(vector_of_unique_patterns, rawname, filenames, map_unique_to_all);

    // finishing measuring time
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::minutes>(t2 - t1);
    std::cout << "Conversion took " << ms_int.count() << "min\n";
    return 0;
}

// definitions
SKmer process_line(const std::string& line_buffer) {
    /*
     * this function processes lines by putting them in a structure than contains  the Kmer name, a vector of its absence/presence pattern. Ignores the corrected attribute.
     */
    std::vector<int> output_pattern;
    std::string kmer_name;
    std::istringstream input(line_buffer);
    //loop over tab-separated words in the line :
    for (std::string word; std::getline(input, word, '\t'); ) {
        if (word.starts_with(">")) { // if id resets line
            kmer_name = word;
        } else if (word.ends_with("*")) { // if abundance is 0 : 0-20:*
            output_pattern.push_back(0);
        } else { // if abundance is more than 0
            size_t lastindex = word.find_last_of(":");
            std::string appearance_nb = word.substr(lastindex+1,word.size()-lastindex); // finds 17 in 0-20:17
            output_pattern.push_back(std::stoi(appearance_nb));
        }
    }
    // Kmer output_struct{kmer_name, output_pattern};
    // output_pattern.clear();
    return {kmer_name, output_pattern};
}

SKmer binarise_counts(SKmer& data) {
    /*
     * this function binaries the abundance of a kmer
     */
    for (int i = 0; i < data.pattern.size(); i++) {
        switch (data.pattern.at(i)) {
            case 0:
                break;
            default:
                data.pattern.at(i) = 1;
                break;
        }
    }
    return data;
}

SKmer minor_allele_description(SKmer& data) {
    /*
     * this function changes the pattern of presence/absence of a Kmer into the minor allele description if needed, and changed the 'corrected' accordingly (1: did not change; -1: changed).
     */
    float sum ;
    std::vector<int> corr_vector;
    for (auto& n : data.pattern) {
        sum += n;
    }
    if (sum/(float)data.pattern.size() > 0.5) {
        for (int i = 0; i < data.pattern.size(); i++) {
            switch (data.pattern.at(i)) {
                case 0:
                    corr_vector.push_back(1);
                    break;
                case 1:
                    corr_vector.push_back(0);
                    break;
            }
        }
        data.corrected = -1;
        data.pattern = corr_vector;
    } else {
        data.corrected = 1;
    }
    return data;
}

void write_bugwas_gemma(const std::vector<std::vector<int>>& vector_of_unique_patterns, std::string& rawname, std::vector<std::string>& filenames, std::map<std::vector<int>, std::vector<int>>& map_unique_to_all) {
    /*
     * this function builds output files : unique_patterns, unique_to_all, and gemma_pattern_to_nb_unitigs, gemma_unitig_to_patterns.
     */
    std::ofstream outstream_unique (rawname+"_unique.binary", std::ofstream::binary);
    std::ofstream outstream_unique_to_all (rawname+"_unique_to_all.binary", std::ofstream::binary);
    std::ofstream outstream_gemma_pattern_to_nb_unitigs (rawname + "_gemma_pattern_to_unitigs.binary", std::ofstream::binary);
    std::ofstream outstream_gemma_unitig_to_patterns (rawname + "_gemma_unitigs_to_patterns.binary", std::ofstream::binary);


    //error check
    if (outstream_unique.fail()) {
        perror("ERROR: could not open output_unique file");
        std::exit(1);
    } else if (outstream_unique_to_all.fail()) {
        perror("ERROR: could not open output_unique_to_all file");
        std::exit(1);
    } else if (outstream_gemma_pattern_to_nb_unitigs.fail()) {
        perror("ERROR: could not open output_gemma_pattern_to_unitigs file");
        std::exit(1);
    } else if (outstream_gemma_unitig_to_patterns.fail()) {
        perror("ERROR: could not open output_gemma_unitig_to_patterns file");
        std::exit(1);
    }

    //TODO: check that my understanding of the output files is ok

    // header for unique_pattern
    outstream_unique << "ps ";
    for (const std::string &i : filenames) {
        outstream_unique << i << " ";
    }
    outstream_unique << "\n";

    int n = 0;
    for (const auto &i : vector_of_unique_patterns) {
        // writes the unique patterns in their file
        outstream_unique << n << " ";
        for (const auto &j : i) {
            outstream_unique << j << " " ;
        }
        outstream_unique << "\n" ;
        // writes connection between unique pattern and all the unitigs each represents in unique_to_all, and the reciprocal in unitig_to_pattern
        for (const auto &j : map_unique_to_all[i]) {
            outstream_unique_to_all << j << " ";
            outstream_gemma_unitig_to_patterns << j << " " << n << "\n";
        }
        outstream_unique_to_all << "\n" ;

        // writes the number of unitigs that each unique pattern represents
        outstream_gemma_pattern_to_nb_unitigs << n << " " << map_unique_to_all[i].size() << "\n";

        n++;
    }
    outstream_unique.close();
    outstream_unique_to_all.close();
    outstream_gemma_pattern_to_nb_unitigs.close();
    outstream_gemma_unitig_to_patterns.close();
}
