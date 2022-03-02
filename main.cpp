#include <iostream>
#include <cerrno>

int main(int argc, char** argv) {
    std::string matrix = argv[1];
    std::string output = argv[2];
    std::ifstream srteam; //does not work ?
    stream.open(matrix, std::ios::binary);
    //must use infile.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    //TODO: separate the funcitons in opening, reading, writing, and returning errors
    if (stream.fail()) {
        std::cout << "ERROR: could not open file:" << strerror(errno) << std::endl;
        std::exit(1);
    }
    //after opening the file, we must read it :
    while(! stream.eof())
    {
        //do smthg                                                        // count
    }
    return 0;
    //ofstream for output
}