#include "skiplist.h"

SkipList::SkipList() {
	root = (node*) malloc(sizeof(node));
	memset(root, 0, sizeof(node));
	memset(root->key, -1U, KEYLENGTH);
	level = 1;
}

node* SkipList::new_node(string key, string value) {
	node* x = (node*) malloc(sizeof(node));
	memset(x, 0, sizeof(node));
	//x->level = 1;
	memcpy(x->key, key.c_str(), KEYLENGTH);
	int value_len = value.length();
	char* val_ptr = (char*) malloc(value_len);
	memcpy(val_ptr, value.c_str(), value_len);
	x->value = val_ptr;
	return x;
} // end SkipList::new_node()

char SkipList::random_level() {
	char temp = 1;
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_real_distribution<double> distribution(0.1, 1);
	double number = distribution(generator);
	while (((distribution(generator)) < probability) && temp < MAXLEVEL) {
		temp++;
	}
	return temp;
} // end SkipList::random_level()

void SkipList::put(string key, string value) {
	node* update[MAXLEVEL];
	node* x = root;
	for (int i = level - 1; i >= 0; i--) {
		while (x->forward[i] && (string(x->forward[i]->key) < key)) {
			x = x->forward[i];
		} // end (inner) while loop
		update[i] = x;
	} // end (outer) for loop
	x = x->forward[0];

	if (x && (string(x->key) == key)) {
		int value_len = value.length();
		char* val_ptr = (char*) malloc(value_len);
		memcpy(val_ptr, value.c_str(), value_len);
		free(x->value);
		x->value = val_ptr;
	} else {
		char new_level = random_level();
		if (new_level > level) {
			for (int i = level; i <= new_level - 1; i++) {
				update[i] = root;
			}
			level = new_level;
		}
		x = new_node(key, value);
		for (int i = 0; i < new_level; i++) {
			x->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = x;
		}
	} // end if-else
} // end SkipList::put()

string SkipList::get(string key) {
	node* x = root;
	for (int i = level - 1; i >=0; i--) {
		while (x->forward[i] && (string(x->forward[i]->key) < key)) {
			x = x->forward[i];
		}
	}
	x = x->forward[0];
	if (x && (string(x->key) == key)) {
		return string(x->value);
	} else {
		return NULL;
	}
} // end SkipList::get()

vector<string> SkipList::get_keys_for_prefix(string prefix) {
	node* x = root;
	vector<string> strings;
	for (int i = level - 1; i >=0; i--) {
		while (x->forward[i] && (string(x->forward[i]->key) < prefix)) {
			x = x->forward[i];
		} // end (inner) while loop
	} // end (outer) for loop
	if (x) {
		enumerate(&strings, x, prefix);
	}
	// we've had a match and x is non-NULL, so continue search
	return strings;
} // end SkipList::get_keys_for_prefix()

void SkipList::enumerate(vector<string>* strings, node* x, string prefix) {
	int prefix_len = prefix.length();	
	while (x = x->forward[0]) { 
		int str_len = string(x->key).length();
		if (prefix_len == str_len) {
			if (string(x->key) == prefix) {
				// the prefix is exactly same as this key
				strings->push_back(x->value);
			} else {
				// lengths are same but no match, so return
				return;
			}
		} else if (prefix_len < str_len) {
			if (string(x->key).substr(0, prefix_len) == prefix) {
				// prefix is exactly same as this key's prefix
				strings->push_back(x->value);
			} else {
				// prefix didn't match, return
				return;
			}
		} else {
			// length of the prefix is greater, return
			return;
		}
	} else {
		// x is NULL, return
		return;
	}
} // end SkipList::enumerate()