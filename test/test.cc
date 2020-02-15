#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>

using namespace std;

#define NUM_THREADS 5
#define NUM_OP_PER_THREAD 10
#define MAX_LEN_STRING 30
#define WRITE_OP 0
#define GET_OP 1
#define GET_PREFIX 2


void gen_random(char *s, int len) {
   

    static const char valid_chars[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "_$-@#:;,.";
    int i;
    for (i = 0; i < len; i++) {
        s[i] = valid_chars[rand() % 61];
    }
    s[i] = '\0';
}

 string gen_random() {
    int len = 0;
    
    while (len == 0)
        len = rand() % MAX_LEN_STRING;
    char* s = (char*) malloc(len * sizeof(char));
    gen_random(s, len);
    string result = s;
    return result;
 }


void *call(void *threadid) {
    long tid;
    tid = (long)threadid;
    
    vector<string> keys;
    int count = 0;
    while(count < NUM_OP_PER_THREAD) {
        int op = rand() % 3;
        if (op == WRITE_OP) {
            string key = gen_random();
            string value = gen_random();
            
            keys.push_back(key);
            cout<< "key: " << key << " Value: " << value << endl;
            // TODO: CALL SET() in KVSTORE
        } else if (op == GET_OP && keys.size() > 0) {
            int index = rand() % keys.size();
            // TODO: CALL GET() in KVSTORE
        } else if (op == GET_PREFIX && keys.size() > 0) {  
            // TODO: CALL GETPREFIX in KVSTORE
        }
        count++;
    }
   pthread_exit(NULL);
}


int main () {
   pthread_t threads[NUM_THREADS];
   
   for( int i = 0; i < NUM_THREADS; i++ ) {
      int rc = pthread_create(&threads[i], NULL, call, (void *)i);
      
      if (rc) {
         cout << "Not able to create thread" << endl;
         exit(-1);
      }
   }
   pthread_exit(NULL);
}