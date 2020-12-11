/*
* @file MyBot.cpp
* @author Adhokshaja V Madhwaraj
* @date 2019-02-24
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <iostream>
#include <bits/stdc++.h>
#include <vector>

#define ply 10
#define LARGE 10000

using namespace std;
using namespace Desdemona;



Turn ourTurn;
Move finalmove(-1, -1);

class Node {
public:    
    double h;
    OthelloBoard board;
    Turn type;
    int depth;

    Node(Turn t, OthelloBoard b, int d) {
        board = b;
        depth = d;
        type = t;
    }
};
 

class MyBot: public OthelloPlayer {
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
    private:
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn ){}


OthelloBoard ModifyBoard(Node *n, Move moveMade) {
    OthelloBoard b = n->board;
    b.makeMove(n->type, moveMade);
    return b;
}
// working properly

Node *createChild(Node *parent, Turn t, Move previousMove) {
    int depth = parent->depth+1;
    OthelloBoard newBoard = ModifyBoard(parent, previousMove);
    Node *child = new Node(other(t), newBoard, depth); 
    return child;
}

double getMax(double a, double b) {
    if (a>b) return a;
    else return b;
}
double getMin(double a, double b) {
    if (a<b) return a;
    else return b;
}


double Evaluation(Node *n){
    OthelloBoard& curBoard = n->board;
    //Find the number of black tiles and red tiles using getBlackCount() and getRedCount()
    int nb = curBoard.getBlackCount();
    int nr = curBoard.getRedCount();
    int forChecking = nb + nr;
    //Find turn
    Turn presentTurn = ourTurn;//for now
    if(forChecking%2 == 0){
        presentTurn = BLACK;
    }
    else{
        presentTurn = RED;
    }
    //find the number of legal moves using getValidmoves(turn)
    int nv;//Number of valid moves
    list<Move> moves = curBoard.getValidMoves(presentTurn);
    nv = moves.size();

    //Check corners and give out how many red and black;
    int ncb=0;//Number of corners that are black
    int ncr=0;
    if(curBoard.get(0,0) == 'X'){
        ncb++;
    }
    else if(curBoard.get(0,0) == 'O'){
        ncr++;
    }
    if(curBoard.get(7,0) == 'X'){
        ncb++;
    }
    else if(curBoard.get(7,0) == 'O'){
        ncr++;
    }
    if(curBoard.get(0,7) == 'X'){
        ncb++;
    }
    else if(curBoard.get(0,7) == 'O'){
        ncr++;
    }
    if(curBoard.get(7,7) == 'X'){
        ncb++;
    }
    else if(curBoard.get(7,7) == 'O'){
        ncr++;
    }
    //Do the stability
     int score[8][8] = {
        {20, -3, 11,  8,  8,  11,-3, 20},
        {-3, -7, -4,  1,  1, -4, -7, -3},
        {11, -4,  2,  2,  2,  2, -4, 11},
        {8 ,  1,  2, -3, -3,  2,  1,  8},
        {8 ,  1,  2, -3, -3,  2,  1,  8},
        {11, -4,  2,  2,  2,  2, -4, 11},
        {-3, -7, -4,  1,  1, -4, -7, -3},
        {20, -3, 11,  8,  8,  11,-3, 20}
        };
    int stability = 0;

    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(presentTurn == BLACK && curBoard.get(i,j)=='X'){
                stability += score[i][j];
            }
            else if(presentTurn == RED && curBoard.get(i,j)=='O'){
                stability += score[i][j];
            }
            else if(presentTurn == RED && curBoard.get(i,j)=='X'){
                stability -= score[i][j];
            }
            else if(presentTurn == BLACK && curBoard.get(i,j)=='O'){
                stability -= score[i][j];
            }
        }
    }
    //Can do corner closeness
    int nccb = 0;
    int nccr = 0;
    if(curBoard.get(0,0)== NULL){
        if(curBoard.get(0,1) == 'X'){nccb++;}
        else if(curBoard.get(0,1) == 'O'){nccr++;}

        if(curBoard.get(1,1) == 'X'){nccb++;}
        else if(curBoard.get(1,1) == 'O'){nccr++;}

        if(curBoard.get(1,0) == 'X'){nccb++;}
        else if(curBoard.get(1,0) == 'O'){nccr++;}
    }

    //Do the evalution function
    double eval;
    if (presentTurn == BLACK){
        eval = 0.01*(nb - nr)+1.0*(nv)+100.0*(ncb - ncr)+10.0*stability+50.0*(nccb - nccr);
    }
    else{
        eval = 0.01*(nr - nb)+1.0*(nv)+100.0*(ncr - ncb)+10.0*stability+50.0*(nccr - nccb);
    }
    return eval;
}


// this alphabeta is done for all the immediate min nodes. So write a for loop for children in another function and 
// then run this on all the children and update their h values. After that go through them and find which is the 
// best option... print out that child.

double alphabeta(Node *node, double alpha, double beta) {
    if (node->depth == ply) {
        return Evaluation(node);
    }
    double value;
    if (node->type == ourTurn) {
        value = -LARGE;
        list<Move> children_moves = (node->board).getValidMoves(node->type);
        for (list<Move>::iterator iter = children_moves.begin(); iter != children_moves.end(); iter++) {
            Node *child = createChild(node, node->type, *iter); 
            value = getMax(value, alphabeta(child, alpha, beta));
            alpha = getMax(alpha, value);
            if (alpha >= beta) {
                break;
            }
        }
        return value;
    }
    if (node->type == other(ourTurn)) {
        value = LARGE;
        list<Move> children_moves = (node->board).getValidMoves(node->type);
        for (list<Move>::iterator iter = children_moves.begin(); iter != children_moves.end(); iter++) {
            Node *child = createChild(node, node->type, *iter); 
            value = getMin(value, alphabeta(child, alpha, beta));
            beta = getMin(beta, value);
            if (alpha >= beta) {
                break;
            }
        }
        return value;
    }
}

Move bestMove(Node *root) {
    double dummy = -LARGE;
    Move bestmove(-1, -1);
    list<Move> children_moves = (root->board).getValidMoves(root->type);
    for (list<Move>::iterator iter = children_moves.begin(); iter != children_moves.end(); iter++) {
        Node *child = createChild(root, root->type, *iter);
        double temp = alphabeta(child, -LARGE, +LARGE);
        if (temp > dummy) {
            dummy = temp;
            bestmove = *iter;
        }
    }
    return bestmove;
}


Move MyBot::play( const OthelloBoard& board )
{
    ourTurn = turn;
    Node *root = new Node(ourTurn, board, 0);
    return bestMove(root);
}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}




