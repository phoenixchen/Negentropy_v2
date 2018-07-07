#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "utility.h"
#include "algorithm.h"

void *copy(void *val)
{
    char *N = (char*)malloc(sizeof(char));
    *N = *((char*)val);
    return N;
}

int selection(Array *mcts){

    struct MCTS *node;

    int biggest_ucb_index = -1;
    double biggest_ucb = 0;

    for(int j=0;j<array_size(mcts);j++){
        array_get_at(mcts, j,(void*)&node);

//printf("select move:%d -> %d\n",node->start,node->end);
        if(node->ucb > biggest_ucb){
            biggest_ucb = node->ucb;
            biggest_ucb_index = j;
        }
    }
    //system("PAUSE");
    if(biggest_ucb_index==-1){
        printf("error!");
        system("PAUSE");
    }
    else
        return biggest_ucb_index;
}

void expansion(Array *mcts,int selected_index,char *chess_copy,int my_chess_color){

    struct MCTS *node;

    array_get_at(mcts, selected_index,(void*)&node);

    chess_copy[locatetransform(node->end)] = my_chess_color;

    chess_copy[locatetransform(node->start)] = 0;

//printf("really move:%3d -> %3d\n\n",node->start,node->end);

}

int * simulation(char *chess,int simulate_level,bool enemy_simulate_first,bool is_first){

    int mychess[10];
    int enemychess[10];

    char * my_chess_score = ScoreINIT(is_first);
    char * enemy_chess_score = ScoreINIT(!is_first);

    int my_chess_color;
    int enemy_chess_color;

    char * chess_copy = (char *)malloc(sizeof(char)*256);
    memcpy(chess_copy, chess,sizeof(char)*256);

    if(is_first){
        my_chess_color = 1;
        enemy_chess_color = 2;
    }
    else{
        my_chess_color = 2;
        enemy_chess_color = 1;
    }

    int j=0,k=0;
    for(int i =0;i<256;i++){
        if(chess[i] == my_chess_color){
            mychess[j] = (int)locatetransformrevers(i);
            j++;
        }
        if(chess[i] == enemy_chess_color){
            enemychess[k] = (int)locatetransformrevers(i);
            k++;
        }
    }

    char * element;
    Array *ends;
    Array *array_tmp;
    array_new(&array_tmp);
    int score_tmp;
    int my_score_increase = 0;
    int enemy_score_increase = 0;
    int move_index;

    srand(time(NULL));

    struct SimulateTmp * simulate_tmp;

//printf("\nbefore simulate:\n");
//printfieldwithoutcls(chess_copy,-12);

    for(int k=0;k<simulate_level;k++){

        if(!enemy_simulate_first){
            for(int i =0;i<10;i++){
                ends = ReturnAllOfTheMove(chess_copy,mychess[i]);
                for(int j=0;j<array_size(ends);j++){
                    array_get_at(ends, j,(void*)&element);
                    score_tmp = ScoreCaculate(my_chess_score,mychess[i],*element);
                    if(score_tmp>0){
                        simulate_tmp = (struct SimulateTmp *)malloc(sizeof(struct SimulateTmp));
                        simulate_tmp->start = mychess[i];
                        simulate_tmp->end = *element;
                        simulate_tmp->score = score_tmp;
                        array_add(array_tmp,simulate_tmp);
                    }
                }
                array_remove_all_free(ends);
                array_remove_all(ends);
            }

            move_index = rand()%array_size(array_tmp);
            array_get_at(array_tmp, move_index,(void*)&simulate_tmp);
            chess_copy[locatetransform(simulate_tmp->start)] = 0;
            chess_copy[locatetransform(simulate_tmp->end)] = my_chess_color;

//printf("\nafter move %3d->%3d\n",simulate_tmp->start,simulate_tmp->end);
//printfieldwithoutcls(chess_copy,-12);

            my_score_increase = my_score_increase + simulate_tmp->score;

            for(int i=0;i<10;i++){
                if(mychess[i]==simulate_tmp->start){
                    mychess[i]=simulate_tmp->end;
                    break;
                }
            }

            array_remove_all_free(array_tmp);
            array_remove_all(array_tmp);
            enemy_simulate_first = true;
        }
        else{

            for(int i =0;i<10;i++){
                ends = ReturnAllOfTheMove(chess_copy,enemychess[i]);
                for(int j=0;j<array_size(ends);j++){
                    array_get_at(ends, j,(void*)&element);
                    score_tmp = ScoreCaculate(enemy_chess_score,enemychess[i],*element);
                    if(score_tmp>0){
                        struct SimulateTmp * simulate_tmp = (struct SimulateTmp *)malloc(sizeof(struct SimulateTmp));
                        simulate_tmp->start = enemychess[i];
                        simulate_tmp->end = *element;
                        simulate_tmp->score = score_tmp;
                        array_add(array_tmp,simulate_tmp);
                    }
                }
                array_remove_all_free(ends);
                array_remove_all(ends);
            }

            move_index = rand()%array_size(array_tmp);
            array_get_at(array_tmp, move_index,(void*)&simulate_tmp);
            chess_copy[locatetransform(simulate_tmp->start)] = 0;
            chess_copy[locatetransform(simulate_tmp->end)] = enemy_chess_color;

//printf("\nafter move %3d->%3d\n",simulate_tmp->start,simulate_tmp->end);
//printfieldwithoutcls(chess_copy,-12);

            enemy_score_increase = enemy_score_increase + simulate_tmp->score;

            for(int i=0;i<10;i++){
                if(enemychess[i]==simulate_tmp->start){
                    enemychess[i]=simulate_tmp->end;
                    break;
                }
            }

            array_remove_all_free(array_tmp);
            array_remove_all(array_tmp);
            enemy_simulate_first = false;
        }
    }

    array_destroy(ends);
    array_destroy(array_tmp);

    if(my_score_increase-enemy_score_increase>5)
        return 2;
    if(my_score_increase-enemy_score_increase<=5 && my_score_increase-enemy_score_increase>0)
        return 1;
    if(my_score_increase-enemy_score_increase<=0)
        return 0;
}

void backpropagation_fake(Array *mcts,int selected_index,int win_or_lose,int visit_count){

    struct MCTS *node;

    array_remove_at(mcts, selected_index,(void*)&node);

    node->win_rate = node->win_rate +win_or_lose;

    node->visit_times = node->visit_times +2;

    node->ucb = ((double)node->win_rate/(double)node->visit_times) + \
    (double)sqrt(2*log((double)visit_count)/(double)node->visit_times);

    array_add_at(mcts,(void*)node,selected_index);
}

bool IsEqual(char *a,char *b,int length){

    bool equal=true;

    for(int i=0;i<length;i++){
        if(a[i]!=b[i]){
            equal = false;
            break;
        }
    }
    return equal;
}

void Negentropy(SOCKET sclient,char * chess,int my_chess_color){

    const int MCTS_times = 20000;
    const int simulate_level = 30;

    Array *ends;
    Array *mcts;
    array_new(&mcts);

    char * element;

    int visit_count=0;

    int mychess[10];
    int j=0;
    bool is_first;
    bool enemy_simulate_first = true;
    if(my_chess_color ==1)
        is_first = true;
    if(my_chess_color ==2)
        is_first = false;

    for(int i =0;i<256;i++){
        if(chess[i] == my_chess_color){
            mychess[j] = (int)locatetransformrevers(i);
            j++;
        }
    }

    struct MCTS *node;

    char * chess_copy = (char *)malloc(sizeof(char)*256);
    memcpy(chess_copy, chess,sizeof(char)*256);

/*
    for(int i=0;i<256;i++){
        printf("%2d->",chess[i]);
        printf("%2d  ",chess_copy[i]);
        if(i%10==0)
            printf("\n");
    }
    system("PAUSE");
*/

    for(int i =0;i<10;i++){

        ends = ReturnAllOfTheMove(chess,mychess[i]);

        for(int j=0;j<array_size(ends);j++){
            array_get_at(ends, j,(void*)&element);
            node = (struct MCTS *)malloc(sizeof(struct MCTS));
            node->end = *element;
            node->start = mychess[i];
            node->ucb = 65535;
            node->visit_times = 0;
            node->win_rate = 0;
            node->child_end = -1;
            node->child_from = -1;
            node->parent = 65535;
            node->level = 1;
            array_add(mcts, (void*)node);
        }
        array_remove_all_free(ends);
        array_remove_all(ends);
    }

    array_destroy(ends);

    int win_or_lose;

    int selected_index;

    for(int i=0;i<MCTS_times;i++){
        selected_index = selection(mcts);
        expansion(mcts,selected_index,chess_copy,my_chess_color);
        win_or_lose = simulation(chess_copy,simulate_level,enemy_simulate_first,is_first);
        visit_count = visit_count+2;
//printf("win_or_lose:%d\n",win_or_lose);
        backpropagation_fake(mcts,selected_index,win_or_lose,visit_count);

        array_get_at(mcts, selected_index,(void*)&node);
        chess_copy[locatetransform(node->end)] = 0;
        chess_copy[locatetransform(node->start)] = my_chess_color;
    }

    int best_move_start;
    int best_move_end;
    int best_ucb = 0;

    for(int j=0;j<array_size(mcts);j++){
        array_get_at(mcts, j,(void*)&node);
        if(node->ucb > best_ucb){
            best_ucb = node->ucb;
            best_move_start = node->start;
            best_move_end = node->end;
        }
//printf("%2.2f ",node->ucb);
    }

//system("PAUSE");

    chess[locatetransform(best_move_start)] = 0;
    chess[locatetransform(best_move_end)] = my_chess_color;


    char *string_part = NULL;
    char *token = ",";
    char send_string[256]={0};

    Array *path;
    array_new(&path);
    if(abs(best_move_end-best_move_start)==7 || abs(best_move_end-best_move_start)==10 || abs(best_move_end-best_move_start)==17){
        //printf("%3d -> %3d\n",best_move_start,best_move_end);
        string_part = IntToString((int)best_move_start);
        strcat(send_string,string_part);
        strcat(send_string,token);
        string_part = IntToString((int)best_move_end);
        strcat(send_string,string_part);
    }
    else{
        FindJumpPath(chess,best_move_start,best_move_end,path);
        for(int i=0;i<array_size(path);i++){
            array_get_at(path, i,(void*)&element);
            //printf("%3d ->",*element);
            string_part = IntToString((int)*element);
            strcat(send_string,string_part);
            strcat(send_string,token);
        }
        printf("\n");
        for(int i=0;i<256;i++){
            if(send_string[i]=='\0'){
                send_string[i-1]='\0';
                break;
            }
        }
    }

    array_destroy(path);

    int length=0;
    for(int i=0;i<256;i++){
        if(send_string[i]=='\0')
            length = i;
    }
    send(sclient, send_string, length, 0);
}
