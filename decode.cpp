#include <iostream>
#include "huffman.hpp"                      // include the user defined headerfile
using namespace std;

int main (int argc, char const *argv[]) {
    if (argc != 3) {
        cout << "Enter File names: compressedFileName outputFileName while decoding\n";
        return 1;                           // indicate error
    }

    Huffman H(argv[1], argv[2]);            // pass the file arguments
    H.decompress();

    cout << "File decompressed successfully. Compare the size of compressed file\n";

    return 0;
}