#include <iostream>
#include <fstream>
#include <cstring>

void process_line(char * line);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "ERROR: invalid number of arguments" << std::endl;
    }
    std::string();
    std::string matrix = argv[1];
    //std::string output = argv[2];
    std::ifstream stream (matrix, std::ifstream::binary);
    //std::ofstream outstream;

    //must use stream.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    if (stream.fail()) {
        perror("ERROR: could not open file:");
        std::exit(1);
    }

    // read data as a block:
    //stream.read (buffer,size);

    //read data by chunks
    char buffer[4096];
    while(stream.good()) { //dont use !stream.eof() because it wont reach
        // the endOfFile as long as we did not actually read it until
        // the end
        stream.read(buffer, sizeof buffer);
        //read the data line by line:
        char * line = strtok(buffer, "\n");
        while (line) {
            process_line(line);
            //using NULL instead of buffer in strtok() signals it is not the first call
            //of the function: allows to access to more than the 1st token (here named line)
            line = strtok(NULL, "\n");
        }
    }
    //outstream.close();
    stream.close();
    //delete[] buffer;
    return 0;
    //ofstream for output
}

void process_line(char * line) {
    //access the 1st token
    char * token = strtok(line, "\t");
    //as long as we can go without having an empty token
    while (token) {
        std::string token_to_str ;
        token_to_str = token;
        std::cout << token_to_str << std::endl; //output
        token = strtok(NULL, "\t");
    }
}
