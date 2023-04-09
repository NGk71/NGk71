// implement an OOP solution for the Huffman Tree
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>
using namespace std;

struct Node
{
	char c;
	int freq;
	int addtime;
	Node *left;
	Node *right;
	Node(char c, int freq, int addtime = 0, Node *left = nullptr, Node *right = nullptr)
		: c(c), freq(freq), left(left), right(right), addtime(addtime) {}
};

class HuffmanTree
{
private:
	vector<Node *> heap;
	Node *root;
	// the frequency of each character
	map<char, int> freq;
	// the Huffman code of each character
	map<char, string> codes;
	map<string, char> reverseCodes;

	void push(Node *node);
	Node *pop();

public:
	bool empty();
	void createTree();
	void readFreqFromInput(istream &in);
	// helper function
	void printCodes();
	void printCode(Node *root, string code);
	// decode and encode
	string encode(char s);
	char decode(string s);
};
