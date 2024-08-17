#include "huffman.hpp"

void Huffman::createFreqArray() {                           // Initialize a vector of treeNodes with ASCII value and its frequency.
    for (int i = 0; i <= 127; i++) {
        Node *cur = new Node();
        cur->data = i;
        cur->frequency = 0;
        arr.push_back(cur);
    }
}

void Huffman::generateCode(Node *root, string str) {
    if (root->left == NULL && root->right == NULL) {        // If Leaf Node is found, assign the code to respective Node of that character.
        root->code = str;
        return;
    }

    generateCode(root->left, str + '0');                    // 0 for left edge
    generateCode(root->right, str + '1');                   // 1 for right edge //Refer Image
}

void Huffman::buildTree(char ch, string &code) {            // A - freq 3 - code - 001 //We generate Tree again with the help of code and character from root to leaf Node.
    Node *temp = root;
    for (char c : code) {
        if (c == '0') {                                     // move left, if left is not present, create left node and then move
            if (!temp->left) {
                temp->left = new Node();
            }
            temp = temp->left;
        }
        else if (c == '1') {                                // move right, if right is not present, create node and then move
            if (!temp->right) {
                temp->right = new Node();
            }
            temp = temp->right;
        }
    }
    
    temp->data = ch;                                        // When complete path is visited, we get leaf node, so assign character to leaf Node.
}

void Huffman::createMinHeap() {
    char ch;                                                // temp variable To read file character by character
    inFile.open(inFileName, ios::in);                       // Open the input file in input mode;
    inFile.get(ch);                                         // read first char of file

    while (!inFile.eof()) {                                 // Read whole File and increase frequency of characters which appear in input file
        arr[ch]->frequency++;
        inFile.get(ch);
    }
    inFile.close();                                         // Close input file once all characters are read.

    for (int i = 0; i <= 127; i++) {                        // Push Nodes to minHeap
        if (arr[i]->frequency > 0)
        {
            heap.push(arr[i]);
        }
    }
}

void Huffman::createTree() {                                // Constructing the Huffman Tree using minHeap, while encoding file
    priority_queue<Node *, vector<Node *>, Compare> pq(heap); // creating copy of heap for use
    Node *left, *right;

    while (pq.size() != 1) {
        left = pq.top();
        pq.pop();
        right = pq.top();
        pq.pop();                                           // Pop out 2 Nodes with minimum freq, add freq and push it in heap again.

        root = new Node();
        root->frequency = left->frequency + right->frequency;

        root->left = left, root->right = right;
        pq.push(root);
    }
}

void Huffman::createCodes(){                                 // Traverse the huffman tree and assign codes to each character
    generateCode(root, "");
}

int Huffman::binaryToDecimal(string binary) {
    int ans = 0;
    for (auto ch : binary) {
        ans = ans * 2 + (ch - '0');
    }
    return ans;
}

string Huffman::decimalToBinary(int num) {
    string temp = "", ans = "";
    while (num > 0) {
        temp += (num % 2 + '0');
        num /= 2;
    }
    ans.append(8 - temp.size(), '0');
    reverse(temp.begin(), temp.end());
    ans.append(temp);
    return ans;
}

void Huffman::saveEncodedFile() {                           // {minHeapSize}{minHeap}{Encoded inputFile character}{cnt0}
    inFile.open(inFileName, ios::in);
    outFile.open(outFileName, ios::out | ios::binary);
    string in = "";                                         // store encoded data
    string s = "";                                          // storing temp data
    char ch;                                                // read character from file

                                                            // Saving Meta Data
                                                            //{minHeapSize}{mihHeap}
    in += (char)heap.size();
    priority_queue<Node *, vector<Node *>, Compare> pq(heap);
    while (!pq.empty()) {
        Node *temp = pq.top();
        in += temp->data;                                   // store char in encoded file.

                                                            // save code temporarily in 's' and append it later in 'in'
                                                            // Code should be of fixed size,i.e.8 bits
        s.assign(127 - temp->code.length(), '0');
        s += '1';                                           // 1 as seperator
        s += temp->code;

        in += (char)binaryToDecimal(s.substr(0, 8));        // Now save s in 'in'
        for (int i = 0; i < 15; i++)
        {
            s = s.substr(8);                                // remove first 8 characters
            in += (char)binaryToDecimal(s.substr(0, 8));
        }
        pq.pop();
    }
    s.clear();

    inFile.get(ch);                                         // Saving Codes of every character
    while (!inFile.eof()) {
        s += arr[ch]->code;
        
        while (s.length() > 8) {                            // save decimal value of each 8 bit binary code;
            in += (char)binaryToDecimal(s.substr(0, 8));
            s = s.substr(8);                                // remove first 8 bits
        }
        inFile.get(ch);
    }
    int cnt = 8 - s.length();
    if (s.length() < 8) {
        s.append(cnt, '0');
    }
    in += (char)binaryToDecimal(s);
    in += (char)cnt;

    outFile.write(in.c_str(), in.size());                   // Write the in string to outputfile
    inFile.close();
    outFile.close();
}

void Huffman::getTreeFromEncodedFile(){
    inFile.open(inFileName, ios::in | ios::binary);         // open input file

    unsigned char size;                                     // Read size of huffman tree i.e. stored in 1st byte of file
    inFile.read(reinterpret_cast<char *>(&size), 1);

    root = new Node();
    
    for (int i = 0; i < size; i++) {                        // Next size*(1+16) contains (data)char and code[inDecimal](string)
        char ch;
        unsigned char code[16];
        inFile.read(&ch, 1);
        inFile.read(reinterpret_cast<char *>(code), 16);

        string codeBinary = "";                             // convert decimal char to binary to obtain code
        for (int i = 0; i < 16; i++) {
            codeBinary += decimalToBinary(code[i]);
        }

        int k = 0;
        while (codeBinary[k] == '0') {                      // ignore padding
            k++;
        }
        codeBinary = codeBinary.substr(k + 1);              // remove first k+1 characters

        buildTree(ch, codeBinary);
    }
    inFile.close();
}

void Huffman::saveDecodedFile() {
    inFile.open(inFileName, ios::in | ios::binary);
    outFile.open(outFileName, ios::out);                    // Open Input and output files

    unsigned char size;                                     // size of huffman tree
    inFile.read(reinterpret_cast<char *>(&size), 1);        // Read size i.e stored as 1st byte in encoded file.

    char cnt0;                                              // no. of zeroes appended to make size 8
    inFile.seekg(-1, ios::end);                             // set file position to end
    inFile.read(&cnt0, 1);

    inFile.seekg(1 + 17 * size, ios::beg);                  // Ignore the metadata, skip (1 + 17*size) and read remaining file

    vector<unsigned char> encodedText;
    unsigned char textSegment;
    inFile.read(reinterpret_cast<char *>(&textSegment), 1); // read first byte from set pos
    while (!inFile.eof()) {
        encodedText.push_back(textSegment);
        inFile.read(reinterpret_cast<char *>(&textSegment), 1); // read next byte
    }

    Node *temp = root;                                      // Traverse the generated huffman tree and append the character by using code
    string path;
    for (int i = 0; i < encodedText.size() - 1; i++) {      // ignore last cnt0, so -1.
        path = decimalToBinary(encodedText[i]);
        if (i == encodedText.size() - 2) {                  // if last byte, ignore inserted 0 bits 
            path = path.substr(0, 8 - cnt0);
        }
        for (auto ch : path) {
            if (ch == '0') {
                temp = temp->left;
            }
            else {
                temp = temp->right;
            }

            if (temp->left == NULL && temp->right == NULL){  // If leaf node, insert char in out File
                outFile.put(temp->data);
                temp = root;
            }
        }
    }
    inFile.close();
    outFile.close();
}

void Huffman::compress()
{
    createMinHeap();
    createTree();
    createCodes();
    saveEncodedFile();
}

void Huffman::decompress()
{
    getTreeFromEncodedFile();
    saveDecodedFile();
}
