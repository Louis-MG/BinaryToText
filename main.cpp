#include <iostream>
#include <fstream>
#include <cerrno>

int main(int argc, char** argv) {
    std::string matrix = argv[1];
    std::string output = argv[2];
    std::ifstream stream;
    std::ofstream outstream;

    // get size of file
    stream.seekg (0,stream.end);
    long size = stream.tellg();
    stream.seekg (0);

    // allocate memory for file content
    char* buffer = new char[size];

    stream.open(matrix, std::ios::binary);
    //must use stream.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
    //TODO: find out why strerror is not recognised
    if (stream.fail()) {
        perror("ERROR: could not open file:");
        std::exit(1);
    }


    // read data as a block:
    stream.read (buffer,size);

    // print content:
    std::cout.write (buffer,size);
    // release dynamically-allocated memory
    delete[] buffer;
    outstream.close();
    stream.close();

    return 0;
    //ofstream for output
}