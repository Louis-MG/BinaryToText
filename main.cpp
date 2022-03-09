#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

std::string process_line(const std::string& line_buffer);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "ERROR: invalid number of arguments" << std::endl;
    }
    std::string();
    std::string matrix = argv[1];
    std::string output = argv[2];
    std::ifstream stream (matrix, std::ifstream::binary);
    std::ofstream outstream (output, std::ofstream::binary);

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
    while(stream.good() and outstream.good()) { //dont use !stream.eof() because it wont reach
        // the endOfFile as long as we did not actually read it until
        //read the data line by line:
        std::string line_buffer;
        std::string variable;
        while(std::getline(stream, line_buffer).good()) {
            variable = process_line(line_buffer);
            outstream << variable << std::endl;
        }

    }
    outstream.close();
    stream.close();
    return 0;
    //ofstream for output
}

std::string process_line(const std::string& line_buffer) {
    std::string output_line;
    std::istringstream input;
    input.str(line_buffer);
    for (std::string word; std::getline(input, word, '\t'); ) {
        if (word.starts_with(">")) {
            output_line = word;
        } else if (word.ends_with("*")) {
            output_line.append("\t0");
        } else {
            output_line.append("\t1");
        }
    }
    return output_line;
}