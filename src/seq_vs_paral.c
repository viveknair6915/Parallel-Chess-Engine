#include<omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include<sys/time.h>


#define MAX +1
#define MIN -1
#define INFINI INT_MAX

#define KGRN "\x1B[32m"
#define KRED "\x1B[31m"
#define KWHT "\x1B[37m"


struct config {
	char mat[8][8];
	int val;
	char xrN, yrN, xrB, yrB;
	char roqueN, roqueB;
};

int PawnIsolated = -30;

int PawnDouble = -30;

int RookOpenFile = 70;

int RookSemiOpenFile = 50;

int QueenOpenFile = 50;

int QueenSemiOpenFile = 30;

int BishopPair = 50;

int PawnTable[64] = {
	0	    ,  	0   	,   0   	,   0   	,   0   	,  	0	    ,	0	    ,	0	    ,
	50	    ,  	50	    ,   0	    ,   -10     ,   -10     ,  	0	    ,	50	    ,	50	    ,
	25	    ,  	0	    ,  	0	    ,  	25	    ,  	25	    ,  	0	    ,	0	    ,	25	    ,
	0	    ,  	0	    ,  	50	    ,  	90	    ,  	90	    ,  	50	    ,	0	    ,	0	    ,
	25	    ,  	25	    ,  	25	    ,  	50	    ,  	50	    ,  	25	    ,	25	    ,	25	    ,
	50      ,  	50	    ,  	50	    ,  	90	    ,  	90	    ,  	50	    ,	50	    ,	50	    ,
	90	    ,   90 	    ,  	90	    ,  	90	    ,  	90	    ,  	90	    ,	90	    ,	90	    ,
	0	    ,  	0	    ,  	0	    ,  	0	    ,  	0	    ,  	0	    ,	0	    ,	0
};

int KnightTable[64] = {
	0   	,	-50	    ,	0	    ,	0	    ,	0	    ,	0	    ,	-50 	,	0   	,
	0   	,	0	    ,	0	    ,	25	    ,	25	    ,	0	    ,	0	    ,	0   	,
	0	    ,	0	    ,	50	    ,	50	    ,	50	    ,	50	    ,	0	    ,	0	    ,
	0	    ,	25	    ,	50	    ,	90	    ,	90	    ,	50	    ,	25	    ,	0   	,
	25	    ,	50	    ,	70	    ,	90	    ,	90	    ,	70	    ,	50	    ,	25	    ,
	25	    ,  	50	    ,	50	    ,	90	    ,	90	    ,	50	    ,	50	    ,	25	    ,
	0	    ,	0	    ,	25	    ,	50	    ,	50	    ,	25	    ,	0	    ,	0   	,
	0	    ,	0	    ,	0	    ,	0	    ,	0	    ,	0	    ,	0	    ,	0
};

int BishopTable[64] = {
	0	    ,	0	    ,	-50	    ,	0	    ,	0	    ,	-50	    ,	0	    ,	0   	,
	0	    ,  	0	    ,	0	    ,	50	    ,	50	    ,	0	    ,	0	    ,	0	    ,
	0	    ,	0	    ,	50	    ,	70	    ,	70	    ,	50  	,	0	    ,	0	    ,
	0	    ,	50	    ,	70	    ,	90	    ,	90	    ,	70	    ,	50	    ,	0   	,
	0	    ,	50	    ,	70	    ,	90	    ,	90	    ,	70	    ,	50	    ,	0   	,
	0	    ,	0	    ,	50	    ,	70	    ,	70	    ,	50	    ,	0	    ,	0   	,
	0	    ,	0	    ,	0	    ,	50	    ,	50	    ,	0   	,	0	    ,	0   	,
	0	    ,	0   	,	0	    ,	0	    ,	0	    ,	0   	,	0	    ,	0
};

int RookTable[64] = {
    0   	,	0	    ,	25  	,	50	    ,	50	    ,	25	    ,	0	    ,	0	    ,
	0	    ,	0	    ,	25	    ,	50	    ,	50	    ,	25	    ,	0	    ,	0   	,
	0	    ,	0	    ,	25	    ,	50	    ,	50	    ,	25	    ,	0	    ,	0	    ,
	0	    ,	0	    ,	25	    ,	50	    ,	50	    ,	25  	,	0	    ,	0   	,
	0	    ,	0	    ,	25	    ,	50	    ,	50	    ,	25	    ,	0	    ,	0	    ,
	0	    ,	0	    ,	25	    ,	50	    ,	50	    ,	25	    ,	0	    ,	0	    ,
	90	    ,	90	    ,	90	    ,	90	    ,	90	    ,	90	    ,	90	    ,	90	    ,
	0	    ,	0	    ,	25	    ,	50	    ,	50	    ,	25	    ,	0	    ,	0
};

int KingE[64] = {
	-500    ,	-50	    ,	0   	,	0   	,	0   	,	0   	,	-50	    ,	-500	,
	-50     ,	0	    ,	50 	    ,	50	    ,	50	    ,	50	    ,	0   	,	-50     ,
	0	    ,	50	    ,	90	    ,	90	    ,	90	    ,	90	    ,	50	    ,	0	    ,
	0	    ,	50	    ,	90	    ,	99	    ,	99	    ,	90	    ,	50	    ,	0	    ,
	0	    ,	50	    ,	90	    ,	99	    ,	99	    ,	90	    ,	50	    ,	0	    ,
	0	    ,	50	    ,	90	    ,	90	    ,	90	    ,	90	    ,	50	    ,	0	    ,
	-50     ,	0	    ,	50	    ,	50	    ,	50	    ,	50	    ,	0	    ,	-50	    ,
	-500 	,	-50	    ,	0	    ,	0	    ,	0	    ,	0	    ,	-50	    ,	-500
};

int KingO[64] = {
	0	    ,	25	    ,	25	    ,	-50     ,	-50 	,	0	    ,	50	    ,	25	    ,
	-300	,	-300	,	-300	,	-300	,	-300	,	-300	,	-300	,	-300	,
	-500	,	-500	,	-500	,	-500	,	-500	,	-500	,	-500	,	-500	,
	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,
	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,
	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,
	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,
	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700	,	-700
};

int dC[8][2] = { {-2,+1} , {-1,+2} , {+1,+2} , {+2,+1} , {+2,-1} , {+1,-2} , {-1,-2} , {-2,-1} };
int D[8][2] = { {+1,0} , {+1,+1} , {0,+1} , {-1,+1} , {-1,0} , {-1,-1} , {0,-1} , {+1,-1} }; 

// Add a struct to represent a move in algebraic notation
struct move {
    int from_x, from_y, to_x, to_y;
};

// Helper to convert board coordinates to algebraic notation
void move_to_algebraic(int from_x, int from_y, int to_x, int to_y, char *out) {
    out[0] = from_y + 'a';
    out[1] = (8 - from_x) + '0';
    out[2] = to_y + 'a';
    out[3] = (8 - to_x) + '0';
    out[4] = '\0';
}

int minmax_ab( struct config conf, int mode, int niv, int min, int max, long * nb_noeuds, long * nb_coupes);


void copier( struct config *c1, struct config *c2 ) 
{
	int i, j;

	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			c2->mat[i][j] = c1->mat[i][j];

	c2->val = c1->val;
	c2->xrB = c1->xrB;
	c2->yrB = c1->yrB;
	c2->xrN = c1->xrN;
	c2->yrN = c1->yrN;

	c2->roqueB = c1->roqueB;
	c2->roqueN = c1->roqueN;
} 


int egal(char c1[8][8], char c2[8][8] )
{
	int i, j;

	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			if (c1[i][j] != c2[i][j]) return 0;
	return 1;
} 

int nbrPieces(struct config board, bool type)
{
    int i, j, nbr = 0;
    if(type)
    {
        for(i = 0 ; i <= 7 ; i++)
        {
            for(j = 0 ; j <= 7 ; j++)
            {
                if(board.mat[i][j] > 0)
                {
                    nbr ++;
                }
            }
        }
    }
    else
    {
        for(i = 0 ; i <= 7 ; i++)
        {
            for(j = 0 ; j <= 7 ; j++)
            {
                if(board.mat[i][j] < 0)
                {
                    nbr ++;
                }
            }
        }
    }
    return nbr;
}


int estim(struct config board)
{
    int i, j;
    int matrice = 0;
    int isole = 0, rowB, rowN, nbrPionB = 0, nbrPionN = 0, doubl = 0;
    bool pionPosB_1 = false, pionPosB_2 = false, pionPosB = false;
    bool pionPosN_1 = false, pionPosN_2 = false, pionPosN = false;
    int k, rockB_nbrOpen = 0, rockN_nbrOpen = 0, rockValue = 0;
    int queenN_nbrOpen = 0, queenB_nbrOpen = 0, queenValue = 0;
    int nbrBishopB = 0, nbrBishopN = 0, bishopValue = 0;
    int resultat, materiel=0;
    for(j = 0 ; j <= 7 ; j++)
    {
        for(i = 0 ; i <= 7 ; i++)
        {
            switch(board.mat[i][j])
            {
                case 'p':
                    materiel += 100;

                    matrice += PawnTable[j + i * 8];

                    nbrPionB ++;
                    pionPosB = true;
                    if(!pionPosB_1)
                    {
                        if(!pionPosB_2)
                        {
                            pionPosB_2 = true;
                            rowB = j;
                        }
                        else
                        {
                            if (rowB != j)
                            {
                                pionPosB_1 = true;
                            }
                        }
                    }
                break;
                case -'p':
                    materiel -= 100;

                    matrice -= PawnTable[j + (7 - i) * 8];

                    nbrPionN ++;
                    pionPosN = true;
                    if(!pionPosN_1)
                    {
                        if(!pionPosN_2)
                        {
                            pionPosN_2 = true;
                            rowN = j;
                        }
                        else
                        {
                            if (rowN != j)
                            {
                                pionPosN_1 = true;
                            }
                        }
                    }
                break;
                case 'C':
                    materiel += 300;

                    matrice += KnightTable[j + i * 8];
                break;
                case -'C':
                    materiel -= 300;

                    matrice -= KnightTable[j + (7 - i) * 8];
                break;
                case 'f':
                    materiel += 325;

                    matrice += BishopTable[j + i * 8];

                    nbrBishopB ++;
                break;
                case -'f':
                    materiel -= 325;

                    matrice -= BishopTable[j + (7 - i) * 8];

                    nbrBishopN ++;
                break;
                case 't':
                    materiel += 500;

                    matrice += RookTable[j + i * 8];

                    k = 0;
                    while((k <= 7) && (board.mat[k][j] != 'p'))
                    {
                        if(((board.mat[k][j] == 0) || (board.mat[k][j] == 't')) || (board.mat[k][j] < 0))
                        {
                            rockB_nbrOpen ++;
                        }
                        k++;
                    }
                break;
                case -'t':
                    materiel -= 500;

                    matrice -= RookTable[j + (7 - i) * 8];

                    k = 7;
                    while((k >= 0) && (board.mat[k][j] != -'p'))
                    {
                        if(((board.mat[k][j] == 0) || (board.mat[k][j] == -'t')) || (board.mat[k][j] < 0))
                        {
                            rockN_nbrOpen ++;
                        }
                        k --;
                    }
                break;
                case 'n':
                    materiel += 1000;
                    k = 0;
                    while((k <= 7) && (board.mat[k][j] != 'p'))
                    {
                        if(((board.mat[k][j] == 0) || (board.mat[k][j] == 'n')) || (board.mat[k][j] < 0))
                        {
                            queenB_nbrOpen ++;
                        }
                        k++;
                    }
                break;
                case -'n':
                    materiel -= 1000;
                    k = 7;
                    while((k >= 0) && (board.mat[k][j] != -'p'))
                    {
                        if(((board.mat[k][j] == 0) || (board.mat[k][j] == -'n')) || (board.mat[k][j] < 0))
                        {
                            queenN_nbrOpen ++;
                        }
                        k --;
                    }
                break;
                case 'r':
                    if(nbrPieces(board, true) > 8)
                    {
                        matrice += KingO[j + i * 8];
                    }
                    if(nbrPieces(board, true) < 7)
                    {
                        matrice += KingE[j + i * 8];
                    }
                break;
                case -'r':
                    if(nbrPieces(board, true) > 8)
                    {
                        matrice -= KingO[j + (7 - i) * 8];
                    }
                    if(nbrPieces(board, true) < 7)
                    {
                        matrice -= KingE[j + (7 - i) * 8];
                    }
                break;
            }
        }

        if(nbrPionB > 0)
        {
            doubl = doubl + nbrPionB - 1;
        }
        if(nbrPionN > 0)
        {
            doubl = doubl + nbrPionN - 1;
        }
        nbrPionB = 0;
        nbrPionN = 0;

        if(!pionPosB && !pionPosB_1 && pionPosB_2)
        {
            isole ++;
        }
        if(!pionPosB)
        {
            pionPosB_1 = false;
            pionPosB_2 = false;
        }
        pionPosB = false;

        nbrPionN = 0;
        if(!pionPosN && !pionPosN_1 && pionPosN_2)
        {
            isole --;
        }
        if(!pionPosN)
        {
            pionPosN_1 = false;
            pionPosN_2 = false;
        }
        pionPosN = false;

        if(rockB_nbrOpen == 8)
        {
            rockValue += RookOpenFile;
        }
        else
        {
            if(rockB_nbrOpen > 5)
            {
                rockValue += RookSemiOpenFile;
            }
        }
        if(rockN_nbrOpen == 8)
        {
            rockValue -= RookOpenFile;
        }
        else
        {
            if(rockN_nbrOpen > 5)
            {
                rockValue -= RookSemiOpenFile;
            }
        }
        rockB_nbrOpen = 0;
        rockN_nbrOpen = 0;

        if(queenB_nbrOpen == 8)
        {
            queenValue += QueenOpenFile;
        }
        else
        {
            if(queenB_nbrOpen > 5)
            {
                queenValue += QueenSemiOpenFile;
            }
        }
        if(queenN_nbrOpen == 8)
        {
            queenValue -= QueenOpenFile;
        }
        else
        {
            if(queenN_nbrOpen > 5)
            {
                queenValue -= QueenSemiOpenFile;
            }
        }
        queenB_nbrOpen = 0;
        queenN_nbrOpen = 0;

        if(nbrBishopB == 2)
        {
            bishopValue += BishopPair;
            nbrBishopB = 0;
        }
        if(nbrBishopN == 2)
        {
            bishopValue -= BishopPair;
            nbrBishopN = 0;
        }
    }
    resultat = materiel + doubl * PawnDouble + isole * PawnIsolated + rockValue + queenValue + bishopValue + matrice;

    return resultat;
}

int AucunCoupPossible( struct config conf )
{
	return 0;

}


int feuille( struct config conf, int *cout )
{
	
	*cout = 0;

	if ( conf.xrB == -1 ) { 
	   *cout = -100;
	   return 1; 
	}

	if ( conf.xrN == -1 ) {
	   *cout = +100;
	   return 1;
	}

	if (  conf.xrB != -1 &&  conf.xrN != -1 && AucunCoupPossible( conf ) )
	   return 1;

	return 0;

}



void transformPion( struct config conf, int a, int b, int x, int y, struct config T[], int *n )
{
	int signe = +1;
	if (conf.mat[a][b] < 0 ) signe = -1;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'n';
	(*n)++;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'c';
	(*n)++;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'f';
	(*n)++;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'t';
	(*n)++;

}


int caseMenaceePar( int mode, int x, int y, struct config conf )
{
	int i, a, b, stop;

	for (i=0; i<8; i += 1) {
	   a = x + D[i][0];
	   b = y + D[i][1];	  	 
	   if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 ) 
		if ( conf.mat[a][b]*mode == 'r' ) return 1;
	}

	for (i=0; i<8; i++)
	   if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
		if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] * mode == 'c' )  
		   return 1;

	if ( (x-mode) >= 0 && (x-mode) <= 7 && y > 0 && conf.mat[x-mode][y-1]*mode == 'p' )
	   return 1;
	if ( (x-mode) >= 0 && (x-mode) <= 7 && y < 7 && conf.mat[x-mode][y+1]*mode == 'p' )
	   return 1;

	for (i=0; i<8; i += 1) {
	   stop = 0;
	   a = x + D[i][0];
	   b = y + D[i][1];	  	 
	   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) 
		if ( conf.mat[a][b] != 0 )  stop = 1;
		else {
		    a = a + D[i][0];
		    b = b + D[i][1];
		}
	   if ( stop )  {
		if ( conf.mat[a][b]*mode == 'f' && i % 2 != 0 ) return 1; 
		if ( conf.mat[a][b]*mode == 't' && i % 2 == 0 ) return 1;
		if ( conf.mat[a][b]*mode == 'n' ) return 1;
	   }
	}

	return 0;

}


void deplacementsN(struct config conf, int x, int y, struct config T[], int *n )
{
	int i, a, b, stop;

	switch(conf.mat[x][y]) {
	case -'p' : 
		if ( x > 0 && conf.mat[x-1][y] == 0 ) {
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y] = -'p';
			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y, T, n );
		}
		if ( x == 6 && conf.mat[5][y] == 0 && conf.mat[4][y] == 0) {
			copier(&conf, &T[*n]);
			T[*n].mat[6][y] = 0;
			T[*n].mat[4][y] = -'p';
			(*n)++;
		}
		if ( x > 0 && y >0 && conf.mat[x-1][y-1] > 0 ) {
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y-1] = -'p';
			if (T[*n].xrB == x-1 && T[*n].yrB == y-1) { 
				T[*n].xrB = -1; T[*n].yrB = -1; 
			}

			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y-1, T, n ); 
		}
		if ( x > 0 && y < 7 && conf.mat[x-1][y+1] > 0 ) {
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y+1] = -'p';
			if (T[*n].xrB == x-1 && T[*n].yrB == y+1) { 
				T[*n].xrB = -1; T[*n].yrB = -1; 
			}

			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y+1, T, n );
		}
		break;

	case -'c' : 
		for (i=0; i<8; i++)
		   if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
			if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] >= 0 )  {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[ x+dC[i][0] ][ y+dC[i][1] ] = -'c';
			   if (T[*n].xrB == x+dC[i][0] && T[*n].yrB == y+dC[i][1]) { 
				T[*n].xrB = -1; T[*n].yrB = -1; 
			   }

			   (*n)++;
			}
		break;

	case -'f' : 
		for (i=1; i<8; i += 2) {
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf.mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] > 0 ) stop = 1;
			   T[*n].mat[a][b] = -'f';
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   }
		}
		break;

	case -'t' : 
		for (i=0; i<8; i += 2) {
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf.mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] > 0 ) stop = 1;
			   T[*n].mat[a][b] = -'t';
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   if ( conf.roqueN != 'e' && conf.roqueN != 'n' ) {
			      if ( x == 7 && y == 0 && conf.roqueN != 'p')
			   	T[*n].roqueN = 'g';
			      else if ( x == 7 && y == 0 )
			   	   T[*n].roqueN = 'n';
			      if ( x == 7 && y == 7 && conf.roqueN != 'g' )
			   	T[*n].roqueN = 'p';
			      else if ( x == 7 && y == 7 )
			   	   T[*n].roqueN = 'n';
			   }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   }
		}
		break;

	case -'n' : 
		for (i=0; i<8; i += 1) {
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf.mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] > 0 ) stop = 1;
			   T[*n].mat[a][b] = -'n';
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   }
		}
		break;

	case -'r' : 
		if ( conf.roqueN != 'n' && conf.roqueN != 'e' ) {
		   if ( conf.roqueN != 'g' && conf.mat[7][1] == 0 && conf.mat[7][2] == 0 && conf.mat[7][3] == 0 )
		      if ( !caseMenaceePar( MAX, 7, 1, conf ) && !caseMenaceePar( MAX, 7, 2, conf ) && \
			   !caseMenaceePar( MAX, 7, 3, conf ) && !caseMenaceePar( MAX, 7, 4, conf ) )  {
			copier(&conf, &T[*n]);
			T[*n].mat[7][4] = 0;
			T[*n].mat[7][0] = 0;
			T[*n].mat[7][2] = -'r'; T[*n].xrN = 7; T[*n].yrN = 2;
			T[*n].mat[7][3] = -'t';
			T[*n].roqueN = 'e';
			(*n)++;
		      }
		   if ( conf.roqueN != 'p' && conf.mat[7][5] == 0 && conf.mat[7][6] == 0 )
		      if ( !caseMenaceePar( MAX, 7, 4, conf ) && !caseMenaceePar( MAX, 7, 5, conf ) && \
			   !caseMenaceePar( MAX, 7, 6, conf ) )  {
			copier(&conf, &T[*n]);
			T[*n].mat[7][4] = 0;
			T[*n].mat[7][7] = 0;
			T[*n].mat[7][6] = -'r'; T[*n].xrN = 7; T[*n].yrN = 6;
			T[*n].mat[7][5] = -'t';
			T[*n].roqueN = 'e';
			(*n)++;

		      }
		}
			
		for (i=0; i<8; i += 1) {
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 ) 
			if ( conf.mat[a][b] >= 0 ) {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[a][b] = -'r'; T[*n].xrN = a; T[*n].yrN = b;
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   T[*n].roqueN = 'n';
			   (*n)++;
			}
		}
		break;

	}

}


void deplacementsB(struct config conf, int x, int y, struct config T[], int *n )
{
	int i, a, b, stop;

	switch(conf.mat[x][y]) {
	case 'p' :  
		if ( x <7 && conf.mat[x+1][y] == 0 ) {
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y] = 'p';
			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y, T, n );
		}
		if ( x == 1 && conf.mat[2][y] == 0 && conf.mat[3][y] == 0) {
			copier(&conf, &T[*n]);
			T[*n].mat[1][y] = 0;
			T[*n].mat[3][y] = 'p';
			(*n)++;
		}
		if ( x < 7 && y > 0 && conf.mat[x+1][y-1] < 0 ) {
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y-1] = 'p';
			if (T[*n].xrN == x+1 && T[*n].yrN == y-1) { 
				T[*n].xrN = -1; T[*n].yrN = -1; 
			}

			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y-1, T, n );
		}
		if ( x < 7 && y < 7 && conf.mat[x+1][y+1] < 0 ) {
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y+1] = 'p';
			if (T[*n].xrN == x+1 && T[*n].yrN == y+1) { 
				T[*n].xrN = -1; T[*n].yrN = -1; 
			}

			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y+1, T, n );
		}
		break;

	case 'c' : 
		for (i=0; i<8; i++)
		   if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
			if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] <= 0 )  {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[ x+dC[i][0] ][ y+dC[i][1] ] = 'c';
			   if (T[*n].xrN == x+dC[i][0] && T[*n].yrN == y+dC[i][1]) { 
				T[*n].xrN = -1; T[*n].yrN = -1; 
			   }

			   (*n)++;
			}
		break;

	case 'f' : 
		for (i=1; i<8; i += 2) {
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf.mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] < 0 ) stop = 1;
			   T[*n].mat[a][b] = 'f';
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   }
		}
		break;

	case 't' : 
		for (i=0; i<8; i += 2) {
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf.mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] < 0 ) stop = 1;
			   T[*n].mat[a][b] = 't';
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   if ( conf.roqueB != 'e' && conf.roqueB != 'n' ) {
			     if ( x == 0 && y == 0 && conf.roqueB != 'p')
			   	T[*n].roqueB = 'g';
			     else if ( x == 0 && y == 0 )
			   	   T[*n].roqueB = 'n';
			     if ( x == 0 && y == 7 && conf.roqueB != 'g' )
			   	T[*n].roqueB = 'p';
			     else if ( x == 0 && y == 7 )
			   	   T[*n].roqueB = 'n';
			   }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   }
		}
		break;

	case 'n' : 
		for (i=0; i<8; i += 1) {
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf.mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] < 0 ) stop = 1;
			   T[*n].mat[a][b] = 'n';
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   }
		}
		break;

	case 'r' : 
		if ( conf.roqueB != 'n' && conf.roqueB != 'e' ) {
		   if ( conf.roqueB != 'g' && conf.mat[0][1] == 0 && conf.mat[0][2] == 0 && conf.mat[0][3] == 0 )
		      if ( !caseMenaceePar( MIN, 0, 1, conf ) && !caseMenaceePar( MIN, 0, 2, conf ) && \
			   !caseMenaceePar( MIN, 0, 3, conf ) && !caseMenaceePar( MIN, 0, 4, conf ) )  {
			copier(&conf, &T[*n]);
			T[*n].mat[0][4] = 0;
			T[*n].mat[0][0] = 0;
			T[*n].mat[0][2] = 'r'; T[*n].xrB = 0; T[*n].yrB = 2;
			T[*n].mat[0][3] = 't';
			T[*n].roqueB = 'e';
			(*n)++;
		      }
		   if ( conf.roqueB != 'p' && conf.mat[0][5] == 0 && conf.mat[0][6] == 0 )
		      if ( !caseMenaceePar( MIN, 0, 4, conf ) && !caseMenaceePar( MIN, 0, 5, conf ) && \
			   !caseMenaceePar( MIN, 0, 6, conf ) )  {
			copier(&conf, &T[*n]);
			T[*n].mat[0][4] = 0;
			T[*n].mat[0][7] = 0;
			T[*n].mat[0][6] = 'r'; T[*n].xrB = 0; T[*n].yrB = 6;
			T[*n].mat[0][5] = 't';
			T[*n].roqueB = 'e';
			(*n)++;

		      }
		}
			
		for (i=0; i<8; i += 1) {
		   a = x + D[i][0];
		   b = y + D[i][1];	  	 
		   if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 ) 
			if ( conf.mat[a][b] <= 0 ) {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[a][b] = 'r'; T[*n].xrB = a; T[*n].yrB = b;
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   T[*n].roqueB = 'n';
			   (*n)++;
			}
		}
		break;

	}

}


void generer_succ( struct config conf, int mode, struct config T[], int *n )
{
	int i, j, k;

	*n = 0;

	if ( mode == MAX ) 
	{
	   for (i=0; i<8; i++)
	      for (k=0; k<8; k++)
		 if ( conf.mat[i][k] > 0 )
		    deplacementsB(conf, i, k, T, n );

	   for (k=0; k < *n; k++) {
	      	i = T[k].xrB; j = T[k].yrB;
		if ( caseMenaceePar( MIN, i, j, T[k] ) ) {
		    T[k] = T[(*n)-1];
		    (*n)--;
		    k--;
		}
	    }
	}

	else 
	{
	   for (i=0; i<8; i++)
	      for (k=0; k<8; k++)
		 if ( conf.mat[i][k] < 0 )
		    deplacementsN(conf, i, k, T, n );

	   for (k=0; k < *n; k++) 
	   {
		i = T[k].xrN; j = T[k].yrN;
		if ( caseMenaceePar( MAX, i, j, T[k] ) ) {
		    T[k] = T[(*n)-1];
		    (*n)--;
		    k--;
		}
	   }
	}

}


// Modified minmax_ab to track principal variation
int minmax_ab_pv(struct config conf, int mode, int niv, int alpha, int beta, long * nb_noeuds, long * nb_coupes, struct move *pv, int *pv_length) {
    int n, i, score, score2, best_index = -1;
    struct config T[100];
    struct move *child_pv = malloc(100 * sizeof(struct move));
    int child_pv_length;
    *nb_noeuds += 1;

    if (feuille(conf, &score)) {
        if (pv && pv_length) *pv_length = 0;
        free(child_pv);
        return score;
    }
    if (niv == 0) {
        if (pv && pv_length) *pv_length = 0;
        free(child_pv);
        return estim(conf);
    }
    if (mode == MAX) {
        generer_succ(conf, MAX, T, &n);
        score = alpha;
        for (i = 0; i < n; i++) {
            child_pv_length = 0;
            score2 = minmax_ab_pv(T[i], MIN, niv - 1, score, beta, nb_noeuds, nb_coupes, pv ? child_pv : NULL, pv_length ? &child_pv_length : NULL);
            if (score2 > score) {
                score = score2;
                best_index = i;
                if (pv && pv_length) {
                    *pv_length = child_pv_length + 1;
                    pv[0].from_x = -1; // will fill below
                    pv[0].from_y = -1;
                    pv[0].to_x = -1;
                    pv[0].to_y = -1;
                    for (int k = 0; k < child_pv_length; k++) pv[k + 1] = child_pv[k];
                }
            }
            if (score > beta) {
                *nb_coupes += 1;
                free(child_pv);
                return beta;
            }
        }
        if (best_index != -1 && pv && pv_length && *pv_length > 0) {
            for (int fx = 0; fx < 8; fx++) {
                for (int fy = 0; fy < 8; fy++) {
                    if (conf.mat[fx][fy] != 0 && T[best_index].mat[fx][fy] == 0) {
                        for (int tx = 0; tx < 8; tx++) {
                            for (int ty = 0; ty < 8; ty++) {
                                if (conf.mat[fx][fy] != 0 && conf.mat[fx][fy] == T[best_index].mat[tx][ty] && (fx != tx || fy != ty)) {
                                    pv[0].from_x = fx; pv[0].from_y = fy; pv[0].to_x = tx; pv[0].to_y = ty;
                                }
                            }
                        }
                    }
                }
            }
        } else if (pv && pv_length) {
            *pv_length = 0;
        }
    } else {
        generer_succ(conf, MIN, T, &n);
        score = beta;
        for (i = 0; i < n; i++) {
            child_pv_length = 0;
            score2 = minmax_ab_pv(T[i], MAX, niv - 1, alpha, score, nb_noeuds, nb_coupes, pv ? child_pv : NULL, pv_length ? &child_pv_length : NULL);
            if (score2 < score) {
                score = score2;
                best_index = i;
                if (pv && pv_length) {
                    *pv_length = child_pv_length + 1;
                    pv[0].from_x = -1; // will fill below
                    pv[0].from_y = -1;
                    pv[0].to_x = -1;
                    pv[0].to_y = -1;
                    for (int k = 0; k < child_pv_length; k++) pv[k + 1] = child_pv[k];
                }
            }
            if (score < alpha) {
                *nb_coupes += 1;
                free(child_pv);
                return alpha;
            }
        }
        if (best_index != -1 && pv && pv_length && *pv_length > 0) {
            for (int fx = 0; fx < 8; fx++) {
                for (int fy = 0; fy < 8; fy++) {
                    if (conf.mat[fx][fy] != 0 && T[best_index].mat[fx][fy] == 0) {
                        for (int tx = 0; tx < 8; tx++) {
                            for (int ty = 0; ty < 8; ty++) {
                                if (conf.mat[fx][fy] != 0 && conf.mat[fx][fy] == T[best_index].mat[tx][ty] && (fx != tx || fy != ty)) {
                                    pv[0].from_x = fx; pv[0].from_y = fy; pv[0].to_x = tx; pv[0].to_y = ty;
                                }
                            }
                        }
                    }
                }
            }
        } else if (pv && pv_length) {
            *pv_length = 0;
        }
    }
    if (score == +INFINI) score = +100;
    if (score == -INFINI) score = -100;
    free(child_pv);
    return score;
}

int minmax( struct config conf, int mode, int niv)
{
 	int n, i, score, score2;
 	struct config T[100];

   	if ( feuille(conf, &score) ) 
		return score;

   	if ( niv == 0 ) 
		return estim( conf );

   	if ( mode == MAX ) {

	   generer_succ( conf, MAX, T, &n );

	   score = -INFINI;
	   for ( i=0; i<n; i++ ) 
	   {
   	    	score2 = minmax( T[i], MIN, niv-1);
			if (score2 > score) score = score2;
		
	   } 
	}
	else  { 

	   generer_succ( conf, MIN, T, &n );

	   score = +INFINI;
	   for ( i=0; i<n; i++ ) {
   	    	score2 = minmax( T[i], MAX, niv-1);
		if (score2 < score) score = score2;
	
	   }
	}

        if ( score == +INFINI ) score = +100;
        if ( score == -INFINI ) score = -100;

	return score;

}
void init( struct config *conf )
{
   	int i, j;

    	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			conf->mat[i][j] = 0;

	conf->mat[0][0] =  't'; conf->mat[0][1] =  'c'; conf->mat[0][2] = 'f'; conf->mat[0][3] =  'n';
	conf->mat[0][4] =  'r'; conf->mat[0][5] =  'f'; conf->mat[0][6] = 'c'; conf->mat[0][7] =  't';

	for (j=0; j<8; j++) {
		conf->mat[1][j] = 'p';
 		conf->mat[6][j] = -'p'; 
		conf->mat[7][j] = -conf->mat[0][j];
	}

	conf->xrB = 0; conf->yrB = 4;
	conf->xrN = 7; conf->yrN = 4;

	conf->roqueB = 'r';
	conf->roqueN = 'r';

	conf->val = 0;

}

void affich( struct config conf )
{
	system("clear");
	int i, j, k;
	for (i=0;  i<8; i++)
		printf("\t  %c", i+'a');
   	printf("\n");

	for (i=0;  i<8; i++)
		printf("\t----- ");
   	printf("\n");

	for(i=8; i>0; i--)  {
		printf("    %d", i);
		for (j=0; j<8; j++)
			if ( conf.mat[i-1][j] < 0 ) {
				printf("\t %s-%c",KRED, -conf.mat[i-1][j]);
				printf("%s", KWHT);
			}
			else if ( conf.mat[i-1][j] > 0 ) {
				printf("\t%s +%c",KGRN,  conf.mat[i-1][j]);
				printf("%s", KWHT);
			}
				  else printf("\t  ");
		printf("\n");

		for (k=0;  k<8; k++)
			printf("\t----- ");
   		printf("\n");

	}
	printf("\n");

}



int main( int argc, char *argv[] )
{
	char ch[10];
	int n, i, j, score, stop, cout, mode, cpt;
   	double stats1[100];
	double stats2[100];
	long nb_noeuds1=0;
	long nb_noeuds2=0;
	long nb_coupes1 = 0;
	long nb_coupes2 = 0;
	int hauteur;


   struct config T[100], conf;

   if ( argc == 1 ) 
	hauteur = 5;
   else
	hauteur = atoi( argv[1] );

   init( &conf );
  
   

   stop = 0;
   mode = MAX;
   
   struct timeval begin, end;
   int alpha, beta;
   double result;
   cpt = 0;
   nb_noeuds1=0;
   nb_noeuds2=0;
   long local_nb_noeuds1 = 0, local_nb_coupes1= 0, local_nb_coupes2 = 0, local_nb_noeuds2 = 0;

   while (!stop && (cpt < 50))
   {
	    alpha= -INFINI;
		beta= +INFINI;

		
		generer_succ(conf, mode, T, &n);
		
		score = -INFINI*mode;
		j = -1;
		gettimeofday(&begin, NULL);
		#pragma omp parallel private (local_nb_noeuds2, local_nb_coupes2, local_nb_noeuds1, local_nb_coupes1) if (mode == MIN)
		{
		
			#pragma omp for  schedule (dynamic) 
			for (i=0; i<n; i++) 
			{
				
				if (mode == MAX)
				{

					local_nb_coupes1 = 0;
					local_nb_noeuds1 = 0;
					
					cout = minmax_ab_pv(T[i], MIN, hauteur-1, alpha, beta, &local_nb_noeuds1, &local_nb_coupes1, NULL, NULL);
					#pragma omp critical
					{
						if (cout > score) 
						{  		
							alpha =  cout;
							score = cout;
							j = i;
						}
						nb_noeuds1 += local_nb_noeuds1;
						nb_coupes1 += local_nb_coupes1;
						
					}
				}
				else
				{
					local_nb_coupes2 = 0;
					local_nb_noeuds2 = 0;
					cout = minmax_ab_pv(T[i], MAX, hauteur-1, alpha, beta, &local_nb_noeuds2, &local_nb_coupes2, NULL, NULL);
					
					#pragma omp critical
					{
						if ( cout < score ) 
						{  
							beta = cout;
							score = cout;
							j = i;
						}
						nb_coupes2 += local_nb_coupes2;
						nb_noeuds2 += local_nb_noeuds2;
					}
					
				}
				
			}
		}
		gettimeofday(&end, NULL);
		result = (double)(end.tv_usec - begin.tv_usec)/1000000 + end.tv_sec - begin.tv_sec;
		if (mode == MAX)
			stats1[cpt] = result;
		else
		{
			stats2[cpt] = result;
			cpt++;
		}


		if ( j != -1 ) 
		{
			copier( &T[j], &conf );
			conf.val = score;
		}
		else 
		{
			stop = 1;
		}
	   mode *= -1;
	  
   }
   
	int iiii;
	char res[30];
	FILE * f = fopen("results.txt", "w");

	snprintf(res, 30, "%ld", nb_noeuds1);
	fputs(res, f);
	fputs("\n", f);

	snprintf(res, 30, "%ld", nb_noeuds2);
	fputs(res, f);
	fputs("\n", f);



	for (iiii = 0;iiii < cpt; iiii++)
	{
		snprintf(res, 30, "%f", stats1[iiii]);
		fputs(res, f);
		fputs(" ", f);
	}

	fputs("\n", f);
	for (iiii = 0;iiii < cpt; iiii++)
	{
		snprintf(res, 30, "%f", stats2[iiii]);
		fputs(res, f);
		fputs(" ", f);
	}

	fclose(f);

    // Print formatted summary to terminal
    printf("\n==================== Chess Engine Search Summary ====================\n");
    printf("%-25s %-15s %-15s\n", "Metric", "Sequential", "Parallel");
    printf("-------------------------------------------------------------------\n");
    printf("%-25s %-15ld %-15ld\n", "Node Count", nb_noeuds1, nb_noeuds2);
    double total_seq = 0.0, total_par = 0.0;
    for (iiii = 0; iiii < cpt; iiii++) {
        total_seq += stats1[iiii];
        total_par += stats2[iiii];
    }
    printf("%-25s %-15.4f %-15.4f\n", "Total Time (s)", total_seq, total_par);
    if (total_par > 0.0) {
        printf("%-25s %-15.2f\n", "Speedup", total_seq / total_par);
    } else {
        printf("%-25s %-15s\n", "Speedup", "N/A");
    }
    printf("===================================================================\n\n");

    // Print principal variation (best move sequence)
    struct move pv[100];
    int pv_length = 0;
    long dummy_nodes = 0, dummy_cuts = 0;
    minmax_ab_pv(conf, MAX, hauteur, -INFINI, +INFINI, &dummy_nodes, &dummy_cuts, pv, &pv_length);
    printf("Best move sequence (principal variation): ");
    char move_alg[5];
    for (int k = 0; k < pv_length; k++) {
        move_to_algebraic(pv[k].from_x, pv[k].from_y, pv[k].to_x, pv[k].to_y, move_alg);
        printf("%s ", move_alg);
    }
    printf("\n\n");

    // Also write PV to results.txt for UI parsing
    f = fopen("results.txt", "a");
    if (f) {
        fputs("\n", f);
        for (int k = 0; k < pv_length; k++) {
            move_to_algebraic(pv[k].from_x, pv[k].from_y, pv[k].to_x, pv[k].to_y, move_alg);
            fputs(move_alg, f);
            fputs(" ", f);
        }
        fputs("\n", f);
        fclose(f);
    }

    return 0;
}


