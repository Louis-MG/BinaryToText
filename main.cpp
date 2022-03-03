#include <iostream>
#include <fstream>
#include <cerrno>

int main(int argc, char** argv) {
    std::string matrix = argv[1];
    std::string output = argv[2];
    std::ifstream stream;
    stream.open(matrix, std::ios::binary);
    //must use stream.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    //TODO: find out why strerror is not recognised
    if (stream.fail()) {
        perror("ERROR: could not open file:");
        std::exit(1);
    }
    //after opening the file, we must read it :
    while(! stream.eof())
    {
        std::string line ;
        while(getline(stream, line)) {
            std::cout << line << std::endl;
        }
    }
    return 0;
    //ofstream for output
}