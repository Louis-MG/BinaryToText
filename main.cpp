#include <iostream>

int main(int argc, char** argv) {
    std::string matrix = argv[1];
    std::string output = argv[2];
    std::ifstream infile;
    infile.open(matrix, std::ios::binary)
    //ofstream for output
    //after openning the file, we must read it :
    while(! infile.eof())
    {
        getline(infile,line[x]);                                           // Fill line array
        x++;                                                          // count
    }
    return 0;
    //must use infile.fail() with a switch case to now if the thing went fine : no
    // file operation causes C++ to stop.
    // see https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
}
