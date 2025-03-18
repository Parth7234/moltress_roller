#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct gamestate{
    char board[3][3];

}gamestate;
void boardDisplay(gamestate* gs){
    for(int i=0;i<3;i++){
        printf("   |   |   \n");
        for(int j=0;j<3;j++){
            if((gs->board)[i][j]==-1) printf("   ");
            else if((gs->board)[i][j]==0) printf(" 0 ");
            else if((gs->board)[i][j]==1) printf(" X ");
            if(j==0 || j==1)printf("|");
        }
        printf("");
        if(i==0 || i==1)printf("\n___|___|___");
        if (i!=2) printf("\n");


    }
    printf("\n   |   |   \n\n");
}
gamestate* initialise(){
    gamestate* gs=(gamestate*)malloc(sizeof(gamestate));
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            gs->board[i][j]=-1;
        }
    }
    return gs;
}
int isMovesLeft(gamestate* gs) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (gs->board[i][j] == -1) return 1; // Move available
        }
    }
    return 0; // No moves left
}
gamestate* radom(){
    srand(time(NULL));
    gamestate* gs=(gamestate*)malloc(sizeof(gamestate));
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            gs->board[i][j]=rand()%3-1;
        }
    }
    return gs;

}
int decision(gamestate* gs, int k){
    for(int j=0;j<3;j++){
        if(gs->board[0][j]==k && gs->board[1][j]==k && gs->board[2][j]==k) return 1;
        if(gs->board[j][0]==k && gs->board[j][1]==k && gs->board[j][2]==k) return 1;
    }
    if(gs->board[0][0]==k && gs->board[1][1]==k && gs->board[2][2]==k) return 1;
    if(gs->board[0][2]==k && gs->board[1][1]==k && gs->board[2][0]==k) return 1;
    return 0;
}
int checkWiner(gamestate * gs){
    if(decision(gs,0)) return -1;
    if(decision(gs,1)) return 1;
    return 0;

}
int max(int a, int b) {
    return (a > b) ? a : b;
}


int min(int a, int b) {
    return (a < b) ? a : b;
}
int invert(int i){
    if(i==1) return 0;
    if(i==0) return 1;
    return 0;
}
int botMove(gamestate *gs,int flag,int depth){


    int win=checkWiner(gs);
    if(win==1) return 10-depth;
    if(win==-1) return depth-10;
    if (!isMovesLeft(gs)) return 0; 
    



    if(flag==1){//maximise
        int bs=-100000;
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(gs->board[i][j]==-1){
                    gs->board[i][j]=1;
                    int score=botMove(gs,0,depth+1);
                    gs->board[i][j]=-1;

                    bs=max(score,bs);

                    
                }
                
            }
        }
        return bs;

    }
    else{
        int bs=100000;
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(gs->board[i][j]==-1){
                    gs->board[i][j]=0;
                    int score=botMove(gs,1,depth+1);
                    gs->board[i][j]=-1;

                    bs=min(score,bs);

                   
                }
            }
        }
        return bs;

    }
    



}

void bestMove(gamestate *gs){
    int bS=-1000;

    int Y=-1,X=-1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (gs->board[i][j] == -1) { 
                gs->board[i][j] = 1; 
                int score = botMove(gs, 0, 0); 
                gs->board[i][j] = -1; 

                if (score > bS) { 
                    bS = score;
                    X = i;
                    Y = j;
                }
            }
        }
    }


    if (X != -1 && Y != -1) {
        gs->board[X][Y] = 1; 
    }


}
void game(){
    gamestate* gs=initialise();
    printf("Wanna start first Y/N :");
    char op;
    scanf("%c",&op);

    printf("Input Map\n     |     |     \n(0,0)|(1,0)|(2,0)\n_____|_____|_____\n     |     |     \n(1,0)|(1,1)|(2,0)\n_____|_____|_____\n     |     |     \n(2,0)|(2,1)|(2,2)\n     |     |     \n\n");
    int X,Y;
    if(op=='Y'||op=='y'){
        printf("ROWS  Col: ");
  
        scanf("%d%d",&X,&Y);
        gs->board[X][Y]=0;
        boardDisplay(gs);
        bestMove(gs);
        printf("Bot is Moving.....\n");
        boardDisplay(gs);
    }
    else{
        gs->board[1][1]=1;
        boardDisplay(gs);
        
        

    }
    

    int win=0;

    while(win==0 && isMovesLeft(gs)){
        printf("ROWS  Col: ");
        scanf("%d%d",&X,&Y);
        if (X < 0 || X > 2 || Y < 0 || Y > 2 || gs->board[X][Y] != -1) { 
            printf("Invalid move! Try again.\n"); 
            continue;
        }
        gs->board[X][Y]=0;
        boardDisplay(gs);

        if (!isMovesLeft(gs)) break;

        bestMove(gs);
        printf("Bot is Moving.....\n");
        boardDisplay(gs);
        win=checkWiner(gs);
    }

    if (win == 1) {
        printf("Bot wins!\n");
    } else if (win == -1) {
        printf("You win!\n");
    } else {
        printf("It's a draw!\n");
    }
}
    

int main(){
    game();


}
