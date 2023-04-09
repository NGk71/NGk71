#include "huffmanTree.h"

string HuffmanTree::encode(char s)
{
	return codes[s];
}
char HuffmanTree::decode(string s)
{
	return reverseCodes[s];
}

int getTreeDepth(Node *root)
{
	if (root == nullptr)
	{
		return 0;
	}
	return max(getTreeDepth(root->left), getTreeDepth(root->right)) + 1;
}

// helper function to create Huffman tree
static int i = 0;
void HuffmanTree::createTree()
{
	int addtime = 1;
	while (heap.size() > 1)
	{
		Node *left = pop();
		Node *right = pop();
		Node *parent = new Node(' ' - 1, left->freq + right->freq, addtime, left, right);
		addtime++;
		push(parent);
	}
	root = heap.front();
}

void HuffmanTree::push(Node *node)
{
	heap.push_back(node);
	int i = heap.size() - 1;
	while (i > 0)
	{
		if (heap[(i - 1) / 2]->freq > heap[i]->freq)
		{
			swap(heap[(i - 1) / 2], heap[i]);
			i = (i - 1) / 2;
		}
		else if (heap[(i - 1) / 2]->freq == heap[i]->freq)
		{
			// if the frequency and ascii is same, use addtime to sort
			if (heap[(i - 1) / 2]->c == heap[i]->c)
			{
				if (heap[(i - 1) / 2]->addtime < heap[i]->addtime)
				{
					swap(heap[(i - 1) / 2], heap[i]);
					i = (i - 1) / 2;
				}
			}
			else if (heap[(i - 1) / 2]->c > heap[i]->c)
			{
				swap(heap[(i - 1) / 2], heap[i]);
				i = (i - 1) / 2;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}

Node *HuffmanTree::pop()
{
	Node *node = heap.front();
	heap[0] = heap.back();
	heap.pop_back();

	int i = 0;
	while (2 * i + 1 < heap.size())
	{
		int leftChildIdx = 2 * i + 1;
		int rightChildIdx = 2 * i + 2;
		int minIdx = i;

		if (heap[leftChildIdx]->freq < heap[minIdx]->freq)
		{
			minIdx = leftChildIdx;
		}
		else if (heap[leftChildIdx]->freq == heap[minIdx]->freq && heap[leftChildIdx]->c < heap[minIdx]->c)
		{
			// if the frequency is same, sort by ASCII value
			minIdx = leftChildIdx;
		}
		else if (heap[leftChildIdx]->freq == heap[minIdx]->freq && heap[leftChildIdx]->c == heap[minIdx]->c)
		{
			// if the frequency and ascii is same, use addtime to sort
			if (heap[leftChildIdx]->addtime > heap[minIdx]->addtime)
			{
				minIdx = leftChildIdx;
			}
		}

		if (rightChildIdx < heap.size() && heap[rightChildIdx]->freq < heap[minIdx]->freq)
		{
			minIdx = rightChildIdx;
		}
		else if (rightChildIdx < heap.size() && heap[rightChildIdx]->freq == heap[minIdx]->freq && heap[rightChildIdx]->c < heap[minIdx]->c)
		{
			// if the frequency is same, sort by ASCII value
			minIdx = rightChildIdx;
		}
		else if (rightChildIdx < heap.size() && heap[rightChildIdx]->freq == heap[minIdx]->freq && heap[rightChildIdx]->c == heap[minIdx]->c)
		{
			// if the frequency and ascii is same, use addtime to sort
			if (heap[rightChildIdx]->addtime > heap[minIdx]->addtime)
			{
				minIdx = rightChildIdx;
			}
		}

		if (i != minIdx)
		{
			swap(heap[i], heap[minIdx]);
			i = minIdx;
		}
		else
		{
			break;
		}
	}
	return node;
}

bool HuffmanTree::empty()
{
	return heap.empty();
}

// helper function to print Huffman codes
void HuffmanTree::printCode(Node *root, string code)
{
	if (root->left == nullptr && root->right == nullptr)
	{
		// example: Symbol: 0, Frequency: 2, Code: 000
		cout << "Symbol: " << root->c << ", Frequency: " << root->freq << ", Code: " << code << endl;
		codes[root->c] = code;
		reverseCodes[code] = root->c;
		return;
	}
	if (root->left != nullptr)
	{
		printCode(root->left, code + "0");
	}
	if (root->right != nullptr)
	{
		printCode(root->right, code + "1");
	}
}

void HuffmanTree::printCodes()
{
	printCode(root, "");
}

void HuffmanTree::readFreqFromInput(istream &in)
{
	std::string line;
	// The input has multiple lines
	// where each line contains information of character and frequency
	while (getline(in, line))
	{
		std::istringstream iss(line);
		char c;
		int f;

		// The first character is a space
		if (line.at(0) == ' ')
		{
			if (iss >> f)
			{
				c = ' ';
				freq[c] = f;
			}
		}
		else if (iss >> c >> f)
		{
			freq[c] = f;
		}
	}

	// push the node into the heap
	for (auto it = freq.begin(); it != freq.end(); it++)
	{
		push(new Node(it->first, it->second));
	}
}
