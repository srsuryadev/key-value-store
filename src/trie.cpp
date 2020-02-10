#include "trie.h"

Trie::Trie() {
	root = (node*) malloc(sizeof(node));
	memset(root, 0, sizeof(node)); 
}

node* Trie::new_node() {
	int num_bytes = sizeof(node);
	node* ptr = (node*) malloc(num_bytes);
	memset(ptr, 0, num_bytes);
	return ptr; 	
}

void Trie::put(string key, string value) {
	root = put(root, key, value, 0);
}


string Trie::get(string key) {
	node* x = get(root, key, 0);
	if (x == NULL) {
		return NULL;
	} else {
		return (string)(x->value);
	}
}

node* Trie::get(node* x, string key, int depth) {
	if (x == NULL) {
		return NULL;
	}
	if (depth == key.length()) {
		return x;
	}
	char c = key[depth];
	return get(x->kids[c], key, depth + 1);
}


vector<string> Trie::get_keys_for_prefix(string prefix) {
	vector<string> strings;
	node* prefix_node = get(root, prefix, 0);
	if (prefix_node == NULL) {
		return strings;
	}
	enumerate(&strings, prefix_node);
	return strings;
}

void Trie::enumerate(vector<string>* strings, node* x) {
	if (x->value != NULL) {
		strings->push_back((string)(x->value));
	}	
	for (int i = 0; i < 256; i++) {
		if (x->kids[i] != NULL) {
			enumerate(strings, x->kids[i]);
		}
	}
}


node* Trie::put(node* x, string key, string value, int depth) {
	if (x == NULL) {
		x = new_node();
	}
	if (depth == key.length()) {
		int len = value.length();
		char* val = (char*) malloc(len);
		memcpy(val, value.c_str(), len);
		x->value = val;
		x->value_len = len;
		return x;
	}
	char c = key[depth];
	x->kids[c] = put(x->kids[c], key, value, depth + 1);
	return x;
}