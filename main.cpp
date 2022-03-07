#include <iostream>
#include <fstream>
#include <vector>

void process_line(std::string &line);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "ERROR: invalid number of arguments" << std::endl;
    }
    std::string line;
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
    std::vector<char> buffer (4096,0); //reads only the first 4096 bytes
    while(!stream.good()) { //dont use !stream.eof() because it wont reach
        // the endOfFile as long as we did not actually read it until
        // the end
        stream.read(buffer.data(), buffer.size());
        std::streamsize s=stream.gcount();
        //read the data line by line:
        while (std::getline(stream, line)) //cant use s because int
        {
            //std::istringstream iss(line);
            process_line(line);
        }

    }
    //outstream.close();
    stream.close();

    return 0;
    //ofstream for output
}

void process_line(std::string &line) {
    std::string delimiter="\t";
    size_t pos;
    std::string token;
    while ( (pos = line.find(delimiter)) != std::string::npos ) {
        token = line.substr(0, pos); //extracts substring from string : characters before delimiter
        std::cout << token << std::endl; //output
        line.erase(0, pos + delimiter.length()); //cuts the part we read unit delimiter was met plus delimiter length
    }
}
