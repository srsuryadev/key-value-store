#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <stdint.h>
#include <string>

using namespace std;
typedef struct node node;

struct node {
	node* kids[256];
	char* value;
	int value_len;
};

class Trie {
	public:
		Trie();
		void put(string key, string value);
		string get(string key);
		vector<string> get_keys_for_prefix(string prefix);

	private:
		node* root;
		node* new_node();
		node* put(node* x, string key, string val, int depth);
		node* get(node* x, string key, int depth);
		void enumerate(vector<string>* strings, node* x);
};


