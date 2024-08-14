#ifndef HUFFMAN_HPP                         // header guard, prevents file from being included multiple times
#define HUFFMAN_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <algorithm>
using namespace std;

class Node {                                // Node of a Huffman tree
public:
    char data;                              // character data 
    unsigned int frequency;                 // frequency of the character
    string code;                            // binary representation of the character
    Node *left, *right;     

    Node () {
        left = right = NULL;
    }
};

class Huffman {
    vector<Node*> arr;                      // populate Node objects based on the input file
    fstream inFile, outFile;                // input and output files
    string inFileName, outFileName;         //names of input and output files
    Node *root;                             // root of the huffman tree

    class Compare {                         // used by the priority queue (min-heap)
    public:                                 // to prioritize nodes based on frequency
        bool operator()(Node *left, Node *right) {
            return left->frequency > right->frequency;
        }
    };

    // Heap DS to store generate huffman tree, using optimal merge pattern
    // nodes with lower frequencies have higher priority in the heap    
    priority_queue<Node*, vector<Node*>, Compare> heap; 

    void createFreqArray();                 // Initialize a vector of treeNodes with ASCII value and its frequency.

    void createMinHeap();                   // Create a minHeap of Nodes by frequency, which will be used for optimal merge pattern and generating tree.

    void createTree();                      // Constructing the Huffman Tree using minHeap, while encoding file

    void generateCode (Node*, string);      // Generate huffman codes of each character by traversing the huffman tree

    void buildTree(char, string &);         // Construction tree again while decoding the File

    void createCodes();                     // Traverse the huffman tree and assign codes to each character

    int binaryToDecimal(string);            // Convert binary string to its equivalent decimal

    string decimalToBinary(int);            // Convert decimal number to its equivalent binary string.

    void saveEncodedFile();                 // Saving Huffman Encoded File.

    void getTreeFromEncodedFile();          // Read encoded file and generate the huffman tree again for decoding

    void saveDecodedFile();                 // Saving Decoded file to get original file.

public:
    void compress();                        // compressing input file
    void decompress();                      // decompressing input file
    
    Huffman(string inFileName, string outFileName) {
        this->inFileName = inFileName;
        this->outFileName = outFileName;
        createFreqArray();
    }
};

#endif 