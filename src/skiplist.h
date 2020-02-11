#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <stdint.h>
#include <string>
#include <random>

using namespace std;

typedef struct node node;
#define MAXLEVEL 16
#define KEYLENGTH 128

struct node {
	node* forward[MAXLEVEL];
	char key[KEYLENGTH];
	char* value;
};

class SkipList {
	public:
		SkipList();
		void put(string key, string value);
		string get(string key);
		vector<string> get_keys_for_prefix(string prefix);

	private:
		node* root;
		char level;
		double probability = 0.5;
		char random_level();
		node* new_node(string key, string value);
		node* get(node* x, string key, int depth);
		void enumerate(vector<string>* strings, node* x, string prefix);
};