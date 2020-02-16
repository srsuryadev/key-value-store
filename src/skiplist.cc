/*******************************************************************************
* Copyright 2020 Sanchit Jain <sanchit@cs.wisc.edu>                            *
*                                                                              *
* Redistribution and use in source and binary forms, with or without           *
* modification, are permitted provided that the following conditions are met:  *
*                                                                              *
* 1. Redistributions of source code must retain the above copyright notice,    *
* this list of conditions and the following disclaimer.                        *
*                                                                              *
* 2. Redistributions in binary form must reproduce the above copyright notice, *
* this list of conditions and the following disclaimer in the documentation    *
* and/or other materials provided with the distribution.                       *
*                                                                              *
* 3. Neither the name of the copyright holder nor the names of its contributors* 
* may be used to endorse or promote products derived from this software without*
* specific prior written permission.                                           *
*                                                                              *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE    *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR          *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF         *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR              *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER*
* IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)   *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   *
* POSSIBILITY OF SUCH DAMAGE.                                                  *
*                                                                              *
********************************************************************************
*                                                                              *
* File: skiplist.cc                                                            *
*                                                                              *
* Description: A concurrent skiplist based on the paper 'Lock-Free Linked Lists*
* and Skip Lists' by Mikhail Fomitchev and Eric Ruppert (23rd Annual ACM       *
* SIGACT-SIGOPS Symposium on Principles Of Distributed Computing, 2004)        *
*                                                                              *
*******************************************************************************/

#include "skiplist.h"

SkipList::SkipList() {
    node* prev_x = NULL;
    node* prev_y = NULL;
    for (int i = 0; i <= MAXLEVEL; i++) {
        node* x = (node*)aligned_alloc(CACHE_LINE_SIZE, sizeof(node));
        head_tower[i] = x;
        memset(x->key, 0, KEYLENGTH);
        node* y = (node*)aligned_alloc(CACHE_LINE_SIZE, sizeof(node));
        tail_tower[i] = y;
        memset(y->key, -1U, KEYLENGTH);
        x->value = y->value = NULL;
        x->back_link = NULL;
        y->back_link = x;
        x->down = prev_x;
        if (prev_x) {
            prev_x->up = x;
        }
        y->down = prev_y;
        if (prev_y) {
            prev_y->up = y;
        }
        x->successor = y;
        y->successor = NULL;
        prev_x = x;
        prev_y = y;    
    }
    prev_x->up = prev_x;
    prev_y->up = prev_y;
    head = head_tower[0];
    tail = tail_tower[0];
    count = 0;
}

SkipList::~SkipList() {

}

char SkipList::random_level() {
    char temp = 1;
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution(0.1, 1);
    double number = distribution(generator);
    while (((distribution(generator)) < probability) && (temp < MAXLEVEL)) {
        temp++;
    }
    return temp;
}

/***********************************************************************
 * SkipList::search() : searches for a root node with the supplied key *
 **********************************************************************/
node* SkipList::search(string key) {
    search_res res = search_to_level(key, 1);
    node* current_node = res.prev;
    node* next_node = res.next;
    if (string(current_node->key) == key) {
        return current_node;
    } else {
        return ((node*)((void*)NO_SUCH_KEY));
    }
}

/*******************************************************************************
 * SkipList::search_to_level() starts from the head tower and searches for two *
 * consecutive nodes on level 'level', such that the first has a key less than *
 * or equal to k, and the second has a key strictly greater than k             *
 ******************************************************************************/
search_res SkipList::search_to_level(string key, char level) {
    find_start_res res = find_start(level);
    node* current_node = res.lowest_node;
    char current_level = res.level;
    while (current_level > level) { // search down to level v + 1
        search_res temp_res = search_right(key, current_node, current_level);
        current_node = temp_res.prev->down;
        current_level--;
    }
    return search_right(key, current_node, current_level); // search on level v
}

/***************************************************************************
 * SkipList::find_start() searches the head tower for the lowest node that *
 * points to the tail tower                                                *
 ***************************************************************************/
find_start_res SkipList::find_start(char level) {
    node* current_node = head;
    char current_level = 1;
    while (1) {
        if (current_node->up == NULL) {
            break;
        }
        else if (current_node->up->successor == NULL) {
            break;
        }
        if ((((uintptr_t)(current_node->up->successor) != 
            (uintptr_t)tail_tower[current_level])) || 
        (current_level < level )) {
            current_node = current_node->up;
            current_level++;
        } else {
            break;
        }
    }
    find_start_res ret_val;
    ret_val.lowest_node = current_node;
    ret_val.level = current_level;
    return ret_val;
}


/*******************************************************************************
 * SkipList::search_right starts from node curr_node and searches the level for*
 * two consecutive nodes such that the first has a key less than or equal to k,*
 * and the second has a key strictly greater than k                            *
 ******************************************************************************/
search_res SkipList::search_right(string key, node* current_node, char cur_lev){
    node* next_node = PT(current_node->successor);
    while (next_node && ((uintptr_t)next_node != 
        (uintptr_t)tail_tower[cur_lev - 1]) 
        && (string(next_node->key) <= key)) {
        while (EXTRACT_MARK(next_node->tower_root->successor) == 1) {
            try_flag_node_res res = try_flag_node(current_node, next_node);
            current_node = res.prev_node;
            char status = res.res;
            if (status == IN) {
                help_flagged(current_node, next_node);
            }
            next_node = PT(current_node->successor);
        }
            if (string(next_node->key) <= key) {
                current_node = next_node;
                next_node = PT(current_node->successor);
            }
    }
    search_res ret_val;
    ret_val.prev = current_node;
    ret_val.next = next_node;
    return ret_val;
}


/*******************************************************************************
 * SkipList::search_right starts from node curr_node and searches the level for*
 * two consecutive nodes such that the first has a key less than k and the     *
 * second has a key greater than or equal to k                                 *
 ******************************************************************************/
search_res SkipList::search_right_other(string key, node* current_node, 
    char cur_lev) {
    node* next_node = PT(current_node->successor);
    while (next_node && ((uintptr_t)next_node != 
        (uintptr_t)tail_tower[cur_lev - 1]) 
        && (string(next_node->key) < key)) {
        while (EXTRACT_MARK(next_node->tower_root->successor) == 1) {
            try_flag_node_res res = try_flag_node(current_node, next_node);
            current_node = res.prev_node;
            char status = res.res;
            if (status == IN) {
                help_flagged(current_node, next_node);
            }
            next_node = PT(current_node->successor);
        }
        if (string(next_node->key) < key) {
            current_node = next_node;
            next_node = PT(current_node->successor);
        }
    }
    search_res ret_val;
    ret_val.prev = current_node;
    ret_val.next = next_node;
    return ret_val;
}



/***************************************************************************** 
 *SkipList::try_flag_node() attempts to flag the predecessor of target_node. *
 ****************************************************************************/

try_flag_node_res SkipList::try_flag_node(node* prev_node, node* target_node) {
    try_flag_node_res ret_val;
    while (1) {
        if ((((uintptr_t)PT(prev_node->successor)) == (uintptr_t)target_node) &&
        EXTRACT_FLAG(prev_node->successor) == 1) {
            ret_val.prev_node = prev_node;
            ret_val.res = IN;
            ret_val.success = false;
            return ret_val;
        }
        // try compare and swap
        // prev val : target_node, 0, 0
        // new val: target_node, 0, 1
        uintptr_t cas_result = CAS(&(prev_node->successor), 
            (uintptr_t)target_node, ((uintptr_t)(target_node) | 1));

        if (cas_result == (uintptr_t)target_node) {
            ret_val.prev_node = prev_node;
            ret_val.res = IN;
            ret_val.success = true;
            return ret_val;            
        } else if (cas_result == ((uintptr_t)target_node) | 1) {
            ret_val.prev_node = prev_node;
            ret_val.res = IN;
            ret_val.success = false;
            return ret_val;
        }

        while (EXTRACT_MARK(prev_node) == 1) {
            prev_node = prev_node->back_link;
        }

        // TODO
        char level = 1;

        search_res searching = 
            search_right_other(string(target_node->key), prev_node, level);
        prev_node = searching.prev;
        node* del_node = searching.next;
        if ((uintptr_t)del_node != (uintptr_t)target_node) {
            ret_val.prev_node = prev_node;
            ret_val.res = DELETED;
            ret_val.success = false;
            return ret_val;            
        }
    }
}


node* SkipList::create_new_node(string key, string value, node* down, 
    node* tower_root) {
    node* x = (node*)aligned_alloc(CACHE_LINE_SIZE, sizeof(node));
    memset(x->key, 0, KEYLENGTH);
    memcpy(x->key, key.c_str(), KEYLENGTH);
    char* val = (char*)aligned_alloc(CACHE_LINE_SIZE, value.length());
    memcpy(val, value.c_str(), value.length());
    x->value = val;
    x->down = NULL;
    x->tower_root = x;
    return x;    
}

node* SkipList::create_new_node(string key, node* down, node* tower_root) {
    node* x = (node*)aligned_alloc(CACHE_LINE_SIZE, sizeof(node));
    x->value = tower_root->value;
    x->up = NULL;
    x->down = down;
    x->successor = tail_tower[0];
    if (down) {
        down->up = x;
    }
    x->tower_root = tower_root;    
    memcpy(x->key, tower_root->key, KEYLENGTH);
    return x;
}

node* SkipList::insert(string key, string value) {
    search_res result = search_to_level(key, 1);
    node* prev_node = result.prev;
    node* next_node = result.next;
    if (string(prev_node->key) == key) {
        // update value and return DUPLICATE_KEY
        //std::free(prev_node->value);
        prev_node->value = (char*)aligned_alloc(CACHE_LINE_SIZE, 
            value.length());
        memcpy(prev_node->value, value.c_str(), value.length());
        return (node*)(void*)DUPLICATE_KEY;
    }
    node* new_rnode = create_new_node(key, value, NULL, NULL);
    new_rnode->tower_root = new_rnode;
    node* new_node = new_rnode;
    char new_level = random_level();
    char current_level = 1;
    while (1) {
        insert_node_res ins_result = insert_node(new_node, prev_node, next_node,
            value, current_level);
        node* prev_node = ins_result.prev;
        node* result = ins_result.new_node;

        if (((uintptr_t)result == DUPLICATE_KEY) && (current_level == 1)) {
            //std::free(new_node);
            return (node*)(void*)DUPLICATE_KEY;
        }
        if (EXTRACT_MARK(new_rnode) == 1) {
            if (((uintptr_t)result == (uintptr_t)new_node) &&
                ((uintptr_t)new_node != (uintptr_t)new_rnode)) {
                deletion_node(prev_node, new_node);
            }
            return new_rnode;
        }
        if (current_level == new_level) {
            return new_rnode;
        }
        current_level++;
        node* last_node = new_node;
        new_node = create_new_node(key, last_node, new_rnode);
        search_res res = search_to_level(key, current_level);
        prev_node = res.prev;
        next_node = res.next;
    }

} // end SkipList::insert()

insert_node_res SkipList::insert_node(node* new_node, node* prev_node,
    node* next_node, string value, char current_level) {
    insert_node_res ret_val;
    if (string(prev_node->key) == string(new_node->key)) {
        //std::free(prev_node->value);
        prev_node->value = 
            (char*)aligned_alloc(CACHE_LINE_SIZE, value.length());
        memcpy(prev_node->value, value.c_str(), value.length());
        ret_val.prev = prev_node;
        ret_val.new_node = ((node*)(void*)DUPLICATE_KEY);
        return ret_val;
    }
    while (1) {
        node* prev_successor = prev_node->successor;
        if (EXTRACT_FLAG(prev_successor) == 1) {
            help_flagged(prev_node, PT(prev_successor->successor));
        } else {
            new_node->successor = next_node;
            uintptr_t cas_result = CAS(&(prev_node->successor), 
                (uintptr_t)next_node, (uintptr_t)new_node);
            if (cas_result == (uintptr_t)next_node) {
                ret_val.prev = prev_node;
                ret_val.new_node = new_node;
                return ret_val;
            } else {
                if (EXTRACT_FLAG(prev_node->successor)) {
                    help_flagged(prev_node, PT(next_node->successor));
                }
                while (EXTRACT_MARK(prev_node)) {
                    prev_node = prev_node->back_link;
                }
            }
        }
        search_res res = search_right(string(new_node->key), prev_node,
            current_level);
        prev_node = res.prev;
        next_node = res.next;
        if (string(prev_node->key) == string(new_node->key)) {
            //std::free(prev_node->value);
            prev_node->value = 
                (char*)aligned_alloc(CACHE_LINE_SIZE, value.length());
            memcpy(prev_node->value, value.c_str(), value.length());
            ret_val.prev = prev_node;
            ret_val.new_node = ((node*)(void*)DUPLICATE_KEY);
            return ret_val;            
        }
    } // end while loop
} // end SkipList::insert_node()

void SkipList::put(string key, string value) {
    node* new_entry = insert(key, value);
}

string SkipList::get(string key) {
    string ret_val;
    node* search_res = search(key);
    if (uintptr_t(search_res) == NO_SUCH_KEY) {
        return ret_val;
    }
    return string(search_res->value);
}

void SkipList::enumerate(vector<string>* strings, node* x, string prefix) {
    int prefix_len = prefix.length();
    while (x) {
        if ((uintptr_t)x == (uintptr_t)tail) {
            break;
        }
        if (EXTRACT_MARK(x) == 1) {
            x = x->successor;
            continue; // marked for deletion
        }
        int str_len = string(x->key).length();
        if (prefix_len == str_len) {
            if (string(x->key) == prefix) {
                // the prefix is exactly same as this key
                strings->push_back(string(x->value));
            } else {
                // lengths are same but no match, so return
                return;
            }
        } else if (prefix_len < str_len) {
            if (string(x->key).substr(0, prefix_len) == prefix) {
                // prefix is exactly same as this key's prefix
                strings->push_back(string(x->value));
            } else {
                // prefix didn't match, return
                return;
            }
        } else {
            // length of the prefix is greater, return
            return;
        }
        x = x->successor;
    }
} // end SkipList::enumerate()

vector<string> SkipList::get_keys_for_prefix(string prefix) {
    vector<string> strings;
    search_res res = search_to_level(prefix, 1);
    node* current_node = res.prev;
    node* next_node = res.next;
    enumerate(&strings, current_node, prefix);
    return strings;
} // end SkipList::get_keys_for_prefix()

node* SkipList::deletion(string key) {
    return NULL;
}

node* SkipList::deletion_node(node* prev_node, node* del_node) {
    return NULL;
}

void SkipList::help_marked(node* prev_node, node* del_node) {
}

void SkipList::help_flagged(node* prev_node, node* del_node) {
}

void SkipList::try_mark(node* del_node) {
}