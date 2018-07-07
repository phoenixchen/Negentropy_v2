#include <WINSOCK2.H>
#include "array.h"

void *copy(void *val);

struct MCTS{
    int visit_times;
    int win_rate;
    double ucb;
    char start;
    char end;
    int parent;
    int child_from;
    int child_end;
    int level;
};

struct SimulateTmp{
    char start;
    char end;
    int score;
};

int selection(Array *mcts);

void expansion(Array *mcts,int selected_index,char *chess_copy,int my_chess_color);

int * simulation(char *chess,int simulate_level,bool enemy_simulate_first,bool is_first);

void backpropagation_fake(Array *mcts,int selected_index,int win_or_lose,int visit_count);

bool IsEqual(char *a,char *b,int length);

void Negentropy(SOCKET sclient,char * chess,int my_chess_color);
