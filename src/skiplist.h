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
* File: skiplist.h                                                             *
*                                                                              *
* Description: A concurrent skiplist based on the paper 'Lock-Free Linked Lists*
* and Skip Lists' by Mikhail Fomitchev and Eric Ruppert (23rd Annual ACM       *
* SIGACT-SIGOPS Symposium on Principles Of Distributed Computing, 2004)        *
*                                                                              *
*******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <random>
#include <atomic>

// Adapted from linux/blob/master/arch/x86/include/asm/cmpxchg.h
#define x86_64_cmpxchg(ptr, old, new, lock)                      \
({                                                               \
    volatile uintptr_t __ret;                                    \
    volatile uintptr_t __old = (old);                            \
    volatile uintptr_t __new = (new);                            \
    volatile uintptr_t *__ptr = (volatile uintptr_t *)(ptr);     \
    asm volatile(lock "cmpxchgq %2,%1"                           \
                   : "=a" (__ret), "+m" (*__ptr)                 \
                : "r" (__new), "0" (__old)                       \
                : "memory");                                     \
    __ret;                                                       \
})

#define CAS(ptr, old, new)                \
    x86_64_cmpxchg((ptr), (old), (new), "lock; ")

using namespace std;

typedef struct node node;
typedef struct find_start_res find_start_res;
typedef struct search_res search_res;
typedef struct try_flag_node_res try_flag_node_res;
typedef struct insert_node_res insert_node_res;

#define MAXLEVEL 16
#define KEYLENGTH 128
#define DUPLICATE_KEY 0
#define NO_SUCH_KEY 1 
#define NO_SUCH_NODE 2
#define DELETED 3
#define IN 4
#define LARGEST_KEY "LARGEST_KEY"
#define SMALLEST_KEY "SMALLEST_KEY"
#define CACHE_LINE_SIZE 64
#define EXTRACT_MARK(x) ((((uintptr_t)x >> 1) << 63) >> 63)
#define EXTRACT_FLAG(x) (((uintptr_t)x << 63) >> 63)
#define PT(x) (node*)(void*)(((uintptr_t)x >> 2) << 2)

struct node {
    char key[KEYLENGTH];
    node* back_link;
    node* successor;
    node* up;
    node* down;
    node* tower_root;
    char* value;
};

struct find_start_res {
    node* lowest_node;
    char level;
};

struct search_res {
    node* prev;
    node* next;
};

struct try_flag_node_res {
    node* prev_node;
    char res;
    bool success;
};

struct insert_node_res {
    node* prev;
    node* new_node;
};



class SkipList {
    public:
        ~SkipList();
        SkipList();
        void put(string key, string value);
        string get(string key);
        vector<string> get_keys_for_prefix(string prefix);
        std::atomic<int> count;

    private:
        node* head_tower[MAXLEVEL + 1];
        node* tail_tower[MAXLEVEL + 1];
        node* head;
        node* tail;
        char level;
        double probability = 0.5;
        char random_level();
        insert_node_res insert_node(node* new_node, node* prev_node, 
            node* next_node, string value, char level);
        node* insert(string key, string value);
        node* create_new_node(string key, string value, node* down, 
            node* tower_root);
        node* create_new_node(string key, node* down, node* tower_root);
        try_flag_node_res try_flag_node(node* prev_node, node* target_node);
        search_res search_right_other(string key, node* current_node, 
            char level);
        search_res search_right(string key, node* current_node, 
            char level);
        find_start_res find_start(char level);
        search_res search_to_level(string key, char level);
        node* search(string key);
        node* deletion(string key);
        node* deletion_node(node* prev_node, node* del_node);
        void help_marked(node* prev_node, node* del_node);
        void help_flagged(node* prev_node, node* del_node);
        void try_mark(node* del_node);
        void enumerate(vector<string>* strings, node* x, string prefix);
};