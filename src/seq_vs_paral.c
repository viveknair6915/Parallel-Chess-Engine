#include<omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include<sys/time.h>
#include <string.h>
#include <ctype.h>


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
	char move_uci[6]; // UCI move that led to this position
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
    char move_uci[6]; // UCI notation for this move
};

// Helper to convert board coordinates to algebraic notation
void move_to_algebraic(int from_x, int from_y, int to_x, int to_y, char *out) {
    out[0] = from_y + 'a';
    out[1] = (8 - from_x) + '0';
    out[2] = to_y + 'a';
    out[3] = (8 - to_x) + '0';
    out[4] = '\0';
}

// Helper to set move_uci in struct config
void set_move_uci(struct config *child, int from_x, int from_y, int to_x, int to_y) {
    child->move_uci[0] = from_y + 'a';
    child->move_uci[1] = (8 - from_x) + '0';
    child->move_uci[2] = to_y + 'a';
    child->move_uci[3] = (8 - to_x) + '0';
    child->move_uci[4] = '\0';
}

int minmax_ab( struct config conf, int mode, int niv, int min, int max, long * nb_noeuds, long * nb_coupes);

// Forward declarations for piece move generators (fixes warnings in estim)
void deplacementsB(struct config conf, int x, int y, struct config T[], int *n);
void deplacementsN(struct config conf, int x, int y, struct config T[], int *n);

int caseMenaceePar(int mode, int x, int y, struct config conf);

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
	for (int k = 0; k < 6; k++) c2->move_uci[k] = c1->move_uci[k];
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
    int passed_pawn_bonus = 50;
    int center_control_bonus = 10;
    int passed_pawns = 0;
    int center_control = 0;
    // Center squares: d4, e4, d5, e5 (indices: [3][3], [3][4], [4][3], [4][4])
    int center_squares[4][2] = { {3,3}, {3,4}, {4,3}, {4,4} };
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
                    // Passed pawn check for white
                    {
                        int is_passed = 1;
                        for (int r = i+1; r < 8; r++) {
                            if (j > 0 && board.mat[r][j-1] == -'p') is_passed = 0;
                            if (board.mat[r][j] == -'p') is_passed = 0;
                            if (j < 7 && board.mat[r][j+1] == -'p') is_passed = 0;
                        }
                        if (is_passed) passed_pawns++;
                    }
                    // Center control for white
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control++;
                    }
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
                    // Passed pawn check for black
                    {
                        int is_passed = 1;
                        for (int r = i-1; r >= 0; r--) {
                            if (j > 0 && board.mat[r][j-1] == 'p') is_passed = 0;
                            if (board.mat[r][j] == 'p') is_passed = 0;
                            if (j < 7 && board.mat[r][j+1] == 'p') is_passed = 0;
                        }
                        if (is_passed) passed_pawns--;
                    }
                    // Center control for black
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control--;
                    }
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
                    // Center control for white knight
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control++;
                    }
                break;
                case -'C':
                    materiel -= 300;
                    matrice -= KnightTable[j + (7 - i) * 8];
                    // Center control for black knight
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control--;
                    }
                break;
                case 'f':
                    materiel += 325;
                    matrice += BishopTable[j + i * 8];
                    nbrBishopB ++;
                    // Center control for white bishop
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control++;
                    }
                break;
                case -'f':
                    materiel -= 325;
                    matrice -= BishopTable[j + (7 - i) * 8];
                    nbrBishopN ++;
                    // Center control for black bishop
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control--;
                    }
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
                    // Center control for white rook
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control++;
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
                    // Center control for black rook
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control--;
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
                    // Center control for white queen
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control++;
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
                    // Center control for black queen
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control--;
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
                    // Center control for white king
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control++;
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
                    // Center control for black king
                    for (int c = 0; c < 4; c++) {
                        if (i == center_squares[c][0] && j == center_squares[c][1]) center_control--;
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
    // --- King safety penalty ---
    int king_safety_penalty = 20;
    int white_king_pawns = 0, black_king_pawns = 0;
    // Find white king
    int wkx = -1, wky = -1, bkx = -1, bky = -1;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] == 'r') { wkx = i; wky = j; }
            if (board.mat[i][j] == -'r') { bkx = i; bky = j; }
        }
    }
    // Count pawns around white king
    if (wkx != -1 && wky != -1) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                int nx = wkx + dx, ny = wky + dy;
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                    if (board.mat[nx][ny] == 'p') white_king_pawns++;
                }
            }
        }
    }
    // Count pawns around black king
    if (bkx != -1 && bky != -1) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                int nx = bkx + dx, ny = bky + dy;
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                    if (board.mat[nx][ny] == -'p') black_king_pawns++;
                }
            }
        }
    }
    // Subtract penalty for missing pawns (max 8)
    int king_safety_score = 0;
    king_safety_score -= king_safety_penalty * (8 - white_king_pawns);
    king_safety_score += king_safety_penalty * (8 - black_king_pawns);
    // --- Piece mobility bonus ---
    int mobility_bonus = 5;
    int white_mobility = 0, black_mobility = 0;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            char piece = board.mat[i][j];
            if (piece == 'C' || piece == 'f' || piece == 't' || piece == 'n') {
                // White piece mobility
                struct config T[28]; // Max possible moves for queen
                int n = 0;
                // Generate moves for this piece only
                struct config temp;
                copier(&board, &temp);
                if (piece == 'C') deplacementsB(temp, i, j, T, &n);
                if (piece == 'f') deplacementsB(temp, i, j, T, &n);
                if (piece == 't') deplacementsB(temp, i, j, T, &n);
                if (piece == 'n') deplacementsB(temp, i, j, T, &n);
                white_mobility += n;
            } else if (piece == -'C' || piece == -'f' || piece == -'t' || piece == -'n') {
                // Black piece mobility
                struct config T[28];
                int n = 0;
                struct config temp;
                copier(&board, &temp);
                if (piece == -'C') deplacementsN(temp, i, j, T, &n);
                if (piece == -'f') deplacementsN(temp, i, j, T, &n);
                if (piece == -'t') deplacementsN(temp, i, j, T, &n);
                if (piece == -'n') deplacementsN(temp, i, j, T, &n);
                black_mobility += n;
            }
        }
    }
    int mobility_score = mobility_bonus * (white_mobility - black_mobility);
    resultat = materiel + doubl * PawnDouble + isole * PawnIsolated + rockValue + queenValue + bishopValue + matrice;
    // Add passed pawn and center control bonuses
    resultat += passed_pawn_bonus * passed_pawns;
    resultat += center_control_bonus * center_control;
    // Add king safety score
    resultat += king_safety_score;
    // --- Knight outpost bonus ---
    int knight_outpost_bonus = 40;
    int knight_outpost_score = 0;
    // White knights on ranks 4-6 (i=3,4,5)
    for (i = 3; i <= 5; i++) {
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] == 'C') {
                // Protected by a white pawn
                int protected_by_pawn = 0;
                if (i > 0) {
                    if (j > 0 && board.mat[i-1][j-1] == 'p') protected_by_pawn = 1;
                    if (j < 7 && board.mat[i-1][j+1] == 'p') protected_by_pawn = 1;
                }
                // No black pawn can attack this square
                int attacked_by_pawn = 0;
                if (i < 7) {
                    if (j > 0 && board.mat[i+1][j-1] == -'p') attacked_by_pawn = 1;
                    if (j < 7 && board.mat[i+1][j+1] == -'p') attacked_by_pawn = 1;
                }
                if (protected_by_pawn && !attacked_by_pawn) knight_outpost_score += knight_outpost_bonus;
            }
        }
    }
    // Black knights on ranks 1-3 (i=2,1,0)
    for (i = 0; i <= 2; i++) {
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] == -'C') {
                // Protected by a black pawn
                int protected_by_pawn = 0;
                if (i < 7) {
                    if (j > 0 && board.mat[i+1][j-1] == -'p') protected_by_pawn = 1;
                    if (j < 7 && board.mat[i+1][j+1] == -'p') protected_by_pawn = 1;
                }
                // No white pawn can attack this square
                int attacked_by_pawn = 0;
                if (i > 0) {
                    if (j > 0 && board.mat[i-1][j-1] == 'p') attacked_by_pawn = 1;
                    if (j < 7 && board.mat[i-1][j+1] == 'p') attacked_by_pawn = 1;
                }
                if (protected_by_pawn && !attacked_by_pawn) knight_outpost_score -= knight_outpost_bonus;
            }
        }
    }
    // Add mobility score
    resultat += mobility_score;
    // Add knight outpost score
    resultat += knight_outpost_score;
    // --- Improved rook open/semi-open file and bishop pair logic ---
    int white_bishops = 0, black_bishops = 0;
    int rook_open_bonus = 40;
    int rook_semiopen_bonus = 20;
    int bishop_pair_bonus = 50;
    for (j = 0; j < 8; j++) {
        int white_pawn_in_file = 0, black_pawn_in_file = 0;
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == 'p') white_pawn_in_file = 1;
            if (board.mat[i][j] == -'p') black_pawn_in_file = 1;
        }
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == 't') {
                if (!white_pawn_in_file && !black_pawn_in_file) matrice += rook_open_bonus;
                else if (!white_pawn_in_file && black_pawn_in_file) matrice += rook_semiopen_bonus;
            }
            if (board.mat[i][j] == -'t') {
                if (!white_pawn_in_file && !black_pawn_in_file) matrice -= rook_open_bonus;
                else if (!black_pawn_in_file && white_pawn_in_file) matrice -= rook_semiopen_bonus;
            }
            if (board.mat[i][j] == 'f') white_bishops++;
            if (board.mat[i][j] == -'f') black_bishops++;
        }
    }
    // Bishop pair bonus
    if (white_bishops >= 2) matrice += bishop_pair_bonus;
    if (black_bishops >= 2) matrice -= bishop_pair_bonus;
    // --- Pawn structure penalties: doubled and isolated pawns ---
    int doubled_pawn_penalty = 20;
    int isolated_pawn_penalty = 15;
    // Doubled pawns
    for (j = 0; j < 8; j++) {
        int white_pawns = 0, black_pawns = 0;
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == 'p') white_pawns++;
            if (board.mat[i][j] == -'p') black_pawns++;
        }
        if (white_pawns > 1) resultat -= doubled_pawn_penalty * (white_pawns - 1);
        if (black_pawns > 1) resultat += doubled_pawn_penalty * (black_pawns - 1);
    }
    // Isolated pawns
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == 'p') {
                int has_left = 0, has_right = 0;
                if (j > 0) {
                    for (int k = 0; k < 8; k++) if (board.mat[k][j-1] == 'p') has_left = 1;
                }
                if (j < 7) {
                    for (int k = 0; k < 8; k++) if (board.mat[k][j+1] == 'p') has_right = 1;
                }
                if (!has_left && !has_right) resultat -= isolated_pawn_penalty;
            }
            if (board.mat[i][j] == -'p') {
                int has_left = 0, has_right = 0;
                if (j > 0) {
                    for (int k = 0; k < 8; k++) if (board.mat[k][j-1] == -'p') has_left = 1;
                }
                if (j < 7) {
                    for (int k = 0; k < 8; k++) if (board.mat[k][j+1] == -'p') has_right = 1;
                }
                if (!has_left && !has_right) resultat += isolated_pawn_penalty;
            }
        }
    }
    // --- Queen activity bonus ---
    int queen_activity_bonus = 10;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] == 'n') {
                struct config T[28];
                int n = 0;
                deplacementsB(board, i, j, T, &n); // Use white move generator for queen
                resultat += queen_activity_bonus * n;
            }
            if (board.mat[i][j] == -'n') {
                struct config T[28];
                int n = 0;
                deplacementsN(board, i, j, T, &n); // Use black move generator for queen
                resultat -= queen_activity_bonus * n;
            }
        }
    }
    // --- Endgame scaling: king activity bonus in endgame ---
    int endgame_material = 0;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            char p = board.mat[i][j];
            if (p == 'C' || p == 'f' || p == 't' || p == 'n') endgame_material += 1;
            if (p == -'C' || p == -'f' || p == -'t' || p == -'n') endgame_material += 1;
        }
    }
    if (endgame_material <= 6) { // Endgame threshold: 6 or fewer non-pawn pieces
        int king_central_bonus = 20;
        // White king
        int wkx = -1, wky = -1;
        for (i = 0; i < 8; i++) for (j = 0; j < 8; j++) if (board.mat[i][j] == 'r') { wkx = i; wky = j; }
        if (wkx >= 2 && wkx <= 5 && wky >= 2 && wky <= 5) resultat += king_central_bonus;
        // Black king
        int bkx = -1, bky = -1;
        for (i = 0; i < 8; i++) for (j = 0; j < 8; j++) if (board.mat[i][j] == -'r') { bkx = i; bky = j; }
        if (bkx >= 2 && bkx <= 5 && bky >= 2 && bky <= 5) resultat -= king_central_bonus;
    }
    // --- Bishop outpost bonus ---
    int bishop_outpost_bonus = 30;
    for (i = 3; i <= 5; i++) { // White bishops on ranks 4-6
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] == 'f') {
                int protected_by_pawn = 0;
                if (i > 0) {
                    if (j > 0 && board.mat[i-1][j-1] == 'p') protected_by_pawn = 1;
                    if (j < 7 && board.mat[i-1][j+1] == 'p') protected_by_pawn = 1;
                }
                int attackable_by_enemy_pawn = 0;
                if (i < 7) {
                    if (j > 0 && board.mat[i+1][j-1] == -'p') attackable_by_enemy_pawn = 1;
                    if (j < 7 && board.mat[i+1][j+1] == -'p') attackable_by_enemy_pawn = 1;
                }
                if (protected_by_pawn && !attackable_by_enemy_pawn) resultat += bishop_outpost_bonus;
            }
        }
    }
    for (i = 2; i >= 0; i--) { // Black bishops on ranks 1-3
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] == -'f') {
                int protected_by_pawn = 0;
                if (i < 7) {
                    if (j > 0 && board.mat[i+1][j-1] == -'p') protected_by_pawn = 1;
                    if (j < 7 && board.mat[i+1][j+1] == -'p') protected_by_pawn = 1;
                }
                int attackable_by_enemy_pawn = 0;
                if (i > 0) {
                    if (j > 0 && board.mat[i-1][j-1] == 'p') attackable_by_enemy_pawn = 1;
                    if (j < 7 && board.mat[i-1][j+1] == 'p') attackable_by_enemy_pawn = 1;
                }
                if (protected_by_pawn && !attackable_by_enemy_pawn) resultat -= bishop_outpost_bonus;
            }
        }
    }
    // --- Pawn storm bonus ---
    int pawn_storm_bonus = 15;
    // Use existing wkx, wky, bkx, bky from above
    // White pawn storm (pawns on 5-7th rank in front of black king)
    if (bkx != -1 && bky != -1) {
        for (i = 4; i <= 6; i++) {
            for (j = 0; j < 8; j++) {
                if (board.mat[i][j] == 'p' && abs(j - bky) <= 1) resultat += pawn_storm_bonus;
            }
        }
    }
    // Black pawn storm (pawns on 1-3rd rank in front of white king)
    if (wkx != -1 && wky != -1) {
        for (i = 1; i <= 3; i++) {
            for (j = 0; j < 8; j++) {
                if (board.mat[i][j] == -'p' && abs(j - wky) <= 1) resultat -= pawn_storm_bonus;
            }
        }
    }
    // --- Penalty for trapped pieces (rooks/bishops with zero mobility) ---
    int trapped_piece_penalty = 25;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] == 't' || board.mat[i][j] == 'f') {
                struct config T[14];
                int n = 0;
                if (board.mat[i][j] == 't') deplacementsB(board, i, j, T, &n);
                if (board.mat[i][j] == 'f') deplacementsB(board, i, j, T, &n);
                if (n == 0) resultat -= trapped_piece_penalty;
            }
            if (board.mat[i][j] == -'t' || board.mat[i][j] == -'f') {
                struct config T[14];
                int n = 0;
                if (board.mat[i][j] == -'t') deplacementsN(board, i, j, T, &n);
                if (board.mat[i][j] == -'f') deplacementsN(board, i, j, T, &n);
                if (n == 0) resultat += trapped_piece_penalty;
            }
        }
    }
    // --- Connected passed pawns bonus ---
    int connected_passed_pawn_bonus = 40;
    // White
    for (j = 0; j < 7; j++) {
        int is_passed1 = 1, is_passed2 = 1;
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == -'p') is_passed1 = 0;
            if (board.mat[i][j+1] == -'p') is_passed2 = 0;
        }
        int found1 = 0, found2 = 0;
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == 'p') found1 = 1;
            if (board.mat[i][j+1] == 'p') found2 = 1;
        }
        if (is_passed1 && is_passed2 && found1 && found2) resultat += connected_passed_pawn_bonus;
    }
    // Black
    for (j = 0; j < 7; j++) {
        int is_passed1 = 1, is_passed2 = 1;
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == 'p') is_passed1 = 0;
            if (board.mat[i][j+1] == 'p') is_passed2 = 0;
        }
        int found1 = 0, found2 = 0;
        for (i = 0; i < 8; i++) {
            if (board.mat[i][j] == -'p') found1 = 1;
            if (board.mat[i][j+1] == -'p') found2 = 1;
        }
        if (is_passed1 && is_passed2 && found1 && found2) resultat -= connected_passed_pawn_bonus;
    }
    // --- Tempo bonus: more developed minor pieces ---
    int tempo_bonus = 10;
    int white_developed = 0, black_developed = 0;
    // White minor pieces not on home rank (rank 1)
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if ((board.mat[i][j] == 'C' || board.mat[i][j] == 'f') && i != 7) white_developed++;
            if ((board.mat[i][j] == -'C' || board.mat[i][j] == -'f') && i != 0) black_developed++;
        }
    }
    if (white_developed > black_developed) resultat += tempo_bonus;
    if (black_developed > white_developed) resultat -= tempo_bonus;
    // --- King Tropism bonus (proximity to enemy king) ---
    int king_tropism_bonus = 10;
    if (wkx != -1 && wky != -1 && bkx != -1 && bky != -1) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                // White queen/knight tropism to black king
                if (board.mat[i][j] == 'n' || board.mat[i][j] == 'C') {
                    int dist = abs(i - bkx) + abs(j - bky);
                    int bonus = king_tropism_bonus - dist;
                    if (bonus > 0) resultat += bonus;
                }
                // Black queen/knight tropism to white king
                if (board.mat[i][j] == -'n' || board.mat[i][j] == -'C') {
                    int dist = abs(i - wkx) + abs(j - wky);
                    int bonus = king_tropism_bonus - dist;
                    if (bonus > 0) resultat -= bonus;
                }
            }
        }
    }
    // --- Threat Detection bonus (undefended pieces under attack) ---
    int threat_bonus = 30;
    // For each black piece, check if attacked by white and not defended by black
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            if (board.mat[i][j] < 0) { // Black piece
                int attacked = caseMenaceePar(MAX, i, j, board);
                int defended = caseMenaceePar(MIN, i, j, board);
                if (attacked && !defended) resultat += threat_bonus;
            }
            if (board.mat[i][j] > 0) { // White piece
                int attacked = caseMenaceePar(MIN, i, j, board);
                int defended = caseMenaceePar(MAX, i, j, board);
                if (attacked && !defended) resultat -= threat_bonus;
            }
        }
    }
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
    if (*n >= 100) return;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe *'n';
    set_move_uci(&T[*n], a, b, x, y);
    (*n)++;
    if (*n >= 100) return;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe *'c';
    set_move_uci(&T[*n], a, b, x, y);
    (*n)++;
    if (*n >= 100) return;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe *'f';
    set_move_uci(&T[*n], a, b, x, y);
    (*n)++;
    if (*n >= 100) return;
    copier(&conf, &T[*n]);
    T[*n].mat[a][b] = 0;
    T[*n].mat[x][y] = signe *'t';
    set_move_uci(&T[*n], a, b, x, y);
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
    if (conf.mat[x][y] >= 0) return;
    switch(conf.mat[x][y]) {
    case -'p' : 
        if (x > 0 && conf.mat[x-1][y] == 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[x][y] = 0;
            T[*n].mat[x-1][y] = -'p';
            set_move_uci(&T[*n], x, y, x-1, y);
            (*n)++;
            if (x-1 == 0) transformPion(conf, x, y, x-1, y, T, n);
        }
        if (x == 6 && conf.mat[5][y] == 0 && conf.mat[4][y] == 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[6][y] = 0;
            T[*n].mat[4][y] = -'p';
            set_move_uci(&T[*n], x, y, 4, y);
            (*n)++;
        }
        if (x > 0 && y > 0 && conf.mat[x-1][y-1] > 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[x][y] = 0;
            T[*n].mat[x-1][y-1] = -'p';
            if (T[*n].xrB == x-1 && T[*n].yrB == y-1) { T[*n].xrB = -1; T[*n].yrB = -1; }
            set_move_uci(&T[*n], x, y, x-1, y-1);
            (*n)++;
            if (x-1 == 0) transformPion(conf, x, y, x-1, y-1, T, n);
        }
        if (x > 0 && y < 7 && conf.mat[x-1][y+1] > 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[x][y] = 0;
            T[*n].mat[x-1][y+1] = -'p';
            if (T[*n].xrB == x-1 && T[*n].yrB == y+1) { T[*n].xrB = -1; T[*n].yrB = -1; }
            set_move_uci(&T[*n], x, y, x-1, y+1);
            (*n)++;
            if (x-1 == 0) transformPion(conf, x, y, x-1, y+1, T, n);
        }
        break;

    case -'c' : 
        for (i=0; i<8; i++)
           if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
            if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] >= 0 )  { // Only to empty or white
               if (*n >= 100) return;
               copier(&conf, &T[*n]);
               T[*n].mat[x][y] = 0;
               T[*n].mat[ x+dC[i][0] ][ y+dC[i][1] ] = -'c';
               if (T[*n].xrB == x+dC[i][0] && T[*n].yrB == y+dC[i][1]) { 
                T[*n].xrB = -1; T[*n].yrB = -1; 
               }
               set_move_uci(&T[*n], x, y, x+dC[i][0], y+dC[i][1]);
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
               if (*n >= 100) return;
               copier(&conf, &T[*n]);
               T[*n].mat[x][y] = 0;
               if ( T[*n].mat[a][b] > 0 ) stop = 1;
               T[*n].mat[a][b] = -'f';
               if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }
               set_move_uci(&T[*n], x, y, a, b);
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
            if ( conf.mat[ a ] [ b ] < 0 )  stop = 1; // Blocked by black piece
            else {
               if (*n >= 100) return;
               copier(&conf, &T[*n]);
               T[*n].mat[x][y] = 0;
               if ( T[*n].mat[a][b] > 0 ) stop = 1; // Capture white, then stop
               T[*n].mat[a][b] = -'t';
               if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }
               set_move_uci(&T[*n], x, y, a, b);
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
               if (*n >= 100) return;
               copier(&conf, &T[*n]);
               T[*n].mat[x][y] = 0;
               if ( T[*n].mat[a][b] > 0 ) stop = 1;
               T[*n].mat[a][b] = -'n';
               if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }
               set_move_uci(&T[*n], x, y, a, b);
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
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[7][4] = 0;
            T[*n].mat[7][0] = 0;
            T[*n].mat[7][2] = -'r'; T[*n].xrN = 7; T[*n].yrN = 2;
            T[*n].mat[7][3] = -'t';
            T[*n].roqueN = 'e';
            set_move_uci(&T[*n], 7, 4, 7, 0);
            (*n)++;
          }
           if ( conf.roqueN != 'p' && conf.mat[7][5] == 0 && conf.mat[7][6] == 0 )
              if ( !caseMenaceePar( MAX, 7, 4, conf ) && !caseMenaceePar( MAX, 7, 5, conf ) && \
                   !caseMenaceePar( MAX, 7, 6, conf ) )  {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[7][4] = 0;
            T[*n].mat[7][7] = 0;
            T[*n].mat[7][6] = -'r'; T[*n].xrN = 7; T[*n].yrN = 6;
            T[*n].mat[7][5] = -'t';
            T[*n].roqueN = 'e';
            set_move_uci(&T[*n], 7, 4, 7, 7);
            (*n)++;

          }
        }
        for (i=0; i<8; i += 1) {
           a = x + D[i][0];
           b = y + D[i][1];	  	 
           if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 ) 
            if ( conf.mat[a][b] >= 0 ) {
               if (*n >= 100) return;
               copier(&conf, &T[*n]);
               T[*n].mat[x][y] = 0;
               T[*n].mat[a][b] = -'r'; T[*n].xrN = a; T[*n].yrN = b;
               if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }
               set_move_uci(&T[*n], x, y, a, b);
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
    if (conf.mat[x][y] <= 0) return;
    switch(conf.mat[x][y]) {
    case 'p' :  
        if (x < 7 && conf.mat[x+1][y] == 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[x][y] = 0;
            T[*n].mat[x+1][y] = 'p';
            set_move_uci(&T[*n], x, y, x+1, y);
            (*n)++;
            if (x+1 == 7) transformPion(conf, x, y, x+1, y, T, n);
        }
        if (x == 1 && conf.mat[2][y] == 0 && conf.mat[3][y] == 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[1][y] = 0;
            T[*n].mat[3][y] = 'p';
            set_move_uci(&T[*n], x, y, 3, y);
            (*n)++;
        }
        if (x < 7 && y > 0 && conf.mat[x+1][y-1] < 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[x][y] = 0;
            T[*n].mat[x+1][y-1] = 'p';
            if (T[*n].xrN == x+1 && T[*n].yrN == y-1) { T[*n].xrN = -1; T[*n].yrN = -1; }
            set_move_uci(&T[*n], x, y, x+1, y-1);
            (*n)++;
            if (x+1 == 7) transformPion(conf, x, y, x+1, y-1, T, n);
        }
        if (x < 7 && y < 7 && conf.mat[x+1][y+1] < 0) {
            if (*n >= 100) return;
            copier(&conf, &T[*n]);
            T[*n].mat[x][y] = 0;
            T[*n].mat[x+1][y+1] = 'p';
            if (T[*n].xrN == x+1 && T[*n].yrN == y+1) { T[*n].xrN = -1; T[*n].yrN = -1; }
            set_move_uci(&T[*n], x, y, x+1, y+1);
            (*n)++;
            if (x+1 == 7) transformPion(conf, x, y, x+1, y+1, T, n);
        }
        break;

    case 'c' : 
        for (i=0; i<8; i++)
           if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
            if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] <= 0 )  { // Only to empty or black
               copier(&conf, &T[*n]);
               T[*n].mat[x][y] = 0;
               T[*n].mat[ x+dC[i][0] ][ y+dC[i][1] ] = 'c';
               if (T[*n].xrN == x+dC[i][0] && T[*n].yrN == y+dC[i][1]) { 
                T[*n].xrN = -1; T[*n].yrN = -1; 
               }
               set_move_uci(&T[*n], x, y, x+dC[i][0], y+dC[i][1]);
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
               set_move_uci(&T[*n], x, y, a, b);
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
               set_move_uci(&T[*n], x, y, a, b);
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
               set_move_uci(&T[*n], x, y, a, b);
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
            set_move_uci(&T[*n], 0, 4, 0, 0);
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
            set_move_uci(&T[*n], 0, 4, 0, 7);
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
               if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrB = -1; }
               set_move_uci(&T[*n], x, y, a, b);
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
                    strncpy(pv[0].move_uci, T[i].move_uci, 6);
                    for (int k = 0; k < child_pv_length; k++) pv[k + 1] = child_pv[k];
                }
            }
            if (score > beta) {
                *nb_coupes += 1;
                free(child_pv);
                return beta;
            }
        }
        if (best_index == -1 && pv && pv_length) {
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
                    strncpy(pv[0].move_uci, T[i].move_uci, 6);
                    for (int k = 0; k < child_pv_length; k++) pv[k + 1] = child_pv[k];
                }
            }
            if (score < alpha) {
                *nb_coupes += 1;
                free(child_pv);
                return alpha;
            }
        }
        if (best_index == -1 && pv && pv_length) {
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
    // Clear board
    for (i=0; i<8; i++)
        for (j=0; j<8; j++)
            conf->mat[i][j] = 0;
    // Standard chess starting position
    // White pieces (bottom)
    conf->mat[0][0] = 't'; conf->mat[0][1] = 'c'; conf->mat[0][2] = 'f'; conf->mat[0][3] = 'n';
    conf->mat[0][4] = 'r'; conf->mat[0][5] = 'f'; conf->mat[0][6] = 'c'; conf->mat[0][7] = 't';
    for (j=0; j<8; j++) conf->mat[1][j] = 'p';
    // Black pieces (top)
    conf->mat[7][0] = -'t'; conf->mat[7][1] = -'c'; conf->mat[7][2] = -'f'; conf->mat[7][3] = -'n';
    conf->mat[7][4] = -'r'; conf->mat[7][5] = -'f'; conf->mat[7][6] = -'c'; conf->mat[7][7] = -'t';
    for (j=0; j<8; j++) conf->mat[6][j] = -'p';
    // Empty squares
    for (i=2; i<=5; i++)
        for (j=0; j<8; j++)
            conf->mat[i][j] = 0;
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

// --- BEGIN: Robust FEN parsing helper ---
void parse_fen(const char *fen, struct config *conf) {
    int rank = 0, file = 0;
    memset(conf->mat, 0, sizeof(conf->mat));
    conf->xrB = conf->yrB = conf->xrN = conf->yrN = -1;
    conf->roqueB = 'n';
    conf->roqueN = 'n';
    // 1. Piece placement
    const char *p = fen;
    while (*p && rank < 8) {
        if (*p >= '1' && *p <= '8') {
            file += *p - '0';
        } else if (*p == '/') {
            rank++;
            file = 0;
        } else if (isalpha(*p)) {
            char piece = *p;
            int is_black = (piece >= 'a' && piece <= 'z');
            char code = 0;
            switch (tolower(piece)) {
                case 'p': code = 'p'; break;
                case 'n': code = 'c'; break;
                case 'b': code = 'f'; break;
                case 'r': code = 't'; break;
                case 'q': code = 'n'; break;
                case 'k': code = 'r'; break;
            }
            if (code && file < 8) {
                conf->mat[rank][file] = is_black ? -code : code;
                // Track king positions
                if (tolower(piece) == 'k') {
                    if (is_black) { conf->xrN = rank; conf->yrN = file; }
                    else { conf->xrB = rank; conf->yrB = file; }
                }
            }
            file++;
        } else {
            break;
        }
        p++;
    }
    // 2. Active color (skip)
    while (*p && *p == ' ') p++;
    if (*p) p++;
    // 3. Castling rights
    while (*p && *p == ' ') p++;
    if (*p) {
        if (*p == '-') {
            conf->roqueB = 'n';
            conf->roqueN = 'n';
            p++;
        } else {
            for (; *p && *p != ' '; p++) {
                if (*p == 'K') conf->roqueB = 'r';
                if (*p == 'Q') conf->roqueB = 'r';
                if (*p == 'k') conf->roqueN = 'r';
                if (*p == 'q') conf->roqueN = 'r';
            }
        }
    }
    conf->val = 0;
}
// --- END: Robust FEN parsing helper ---

// --- BEGIN: Debug print for board after FEN parsing ---
void print_board(struct config *conf) {
    printf("\nBoard after FEN parsing:\n");
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            char c = conf->mat[r][f];
            if (c == 0) printf(" . ");
            else if (c > 0) printf(" %c ", c);
            else printf(" %c ", -c);
        }
        printf("\n");
    }
    printf("\n");
}
// --- END: Debug print ---

// --- BEGIN: Output best continuation tree with stats and legality check ---
void output_best_tree(FILE *f, struct config conf, int mode, int depth, int max_depth) {
    if (depth > max_depth) return;
    struct config T[100];
    int n = 0;
    generer_succ(conf, mode, T, &n);
    if (n == 0) return;
    // Find best score among all moves
    int best_score = (mode == MAX) ? -INFINI : +INFINI;
    int scores[100];
    long node_counts[100];
    long dummy_cuts;
    for (int i = 0; i < n; i++) {
        node_counts[i] = 0; dummy_cuts = 0;
        scores[i] = minmax_ab_pv(T[i], -mode, max_depth - depth, -INFINI, +INFINI, &node_counts[i], &dummy_cuts, NULL, NULL);
        if (mode == MAX && scores[i] > best_score) best_score = scores[i];
        if (mode == MIN && scores[i] < best_score) best_score = scores[i];
    }
    // Output all moves with best score, but only if legal from current conf
    for (int i = 0; i < n; i++) {
        if (scores[i] == best_score) {
            // Check legality: T[i].move_uci must be a legal move from conf
            int found = 0;
            for (int j = 0; j < n; j++) {
                if (strncmp(T[j].move_uci, T[i].move_uci, 5) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                for (int d = 0; d < depth; d++) fputs("  ", f);
                fprintf(f, "# ERROR: Illegal move in TREE: %s\n", T[i].move_uci);
                return;
            }
            for (int d = 0; d < depth; d++) fputs("  ", f); // Indentation
            fprintf(f, "%s [score=%d, nodes=%ld]\n", T[i].move_uci, scores[i], node_counts[i]);
            output_best_tree(f, T[i], -mode, depth+1, max_depth);
        }
    }
}
// --- END: Output best continuation tree with stats and legality check ---

// --- BEGIN: Print root move table to terminal ---
// Forward declaration to avoid implicit declaration warning
void print_board_after_move(struct config *conf);
void print_root_move_table(struct config conf, int mode, int hauteur) {
    struct config T[100];
    int n = 0;
    generer_succ(conf, mode, T, &n);
    if (n == 0) {
        printf("No legal root moves found.\n");
        return;
    }
    printf("\nRoot Move Table (side: %s):\n", mode == MAX ? "White" : "Black");
    printf("%-8s %-8s %-8s\n", "Move", "Score", "Nodes");
    for (int i = 0; i < n; i++) {
        long nodes = 0, cuts = 0;
        int score = minmax_ab_pv(T[i], -mode, hauteur-1, -10000, 10000, &nodes, &cuts, NULL, NULL);
        printf("%-8s %-8d %-8ld\n", T[i].move_uci, score, nodes);
        // Print board after this move
        printf("Board after move %s:\n", T[i].move_uci);
        print_board_after_move(&T[i]);
    }
}
// --- END: Print root move table ---

// --- BEGIN: Print board after a move ---
void print_board_after_move(struct config *conf) {
    for (int r = 0; r < 8; r++) {
        for (int f = 0; f < 8; f++) {
            char c = conf->mat[r][f];
            if (c == 0) printf(" . ");
            else if (c > 0) printf(" %c ", c);
            else printf(" %c ", -c);
        }
        printf("\n");
    }
    printf("\n");
}
// --- END: Print board after a move ---

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

   // --- FEN reading logic ---
    FILE *fenfile = fopen("fen.txt", "r");
    char fenstr[128] = {0};
    if (fenfile) {
        if (fgets(fenstr, sizeof(fenstr), fenfile)) {
            size_t len = strlen(fenstr);
            if (len > 0 && fenstr[len-1] == '\n') fenstr[len-1] = 0;
            parse_fen(fenstr, &conf);
            print_board(&conf); // Debug print
        } else {
            init(&conf);
        }
        fclose(fenfile);
    } else {
        init(&conf);
    }
    // Print root move table for both sides for debugging
    print_root_move_table(conf, MAX, hauteur);
    print_root_move_table(conf, MIN, hauteur);
  
   

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
        if (n == 0) {
            printf("[ERROR] No legal moves found for side %s. Stopping search.\n", mode == MAX ? "White" : "Black");
            // Write error to results.txt for UI
            FILE *f = fopen("results.txt", "w");
            if (f) {
                fprintf(f, "# ERROR: No legal moves found for side %s\n", mode == MAX ? "White" : "Black");
                fclose(f);
            }
            stop = 1;
            break;
        }
        score = -INFINI*mode;
        j = -1;
        gettimeofday(&begin, NULL);
        long local_nb_noeuds1 = 0, local_nb_coupes1 = 0, local_nb_coupes2 = 0, local_nb_noeuds2 = 0;
        #pragma omp parallel private (local_nb_noeuds2, local_nb_coupes2, local_nb_noeuds1, local_nb_coupes1) if (mode == MIN)
        {
            #pragma omp for  schedule (dynamic)
            for (i=0; i<n; i++)
            {
                if (mode == MAX)
                {
                    local_nb_coupes1 = 0;
                    local_nb_noeuds1 = 0;
                    int local_score = minmax_ab_pv(T[i], MIN, hauteur-1, alpha, beta, &local_nb_noeuds1, &local_nb_coupes1, NULL, NULL);
                    #pragma omp critical
                    {
                        if (local_score > score)
                        {
                            alpha =  local_score;
                            score = local_score;
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
                    int local_score = minmax_ab_pv(T[i], MAX, hauteur-1, alpha, beta, &local_nb_noeuds2, &local_nb_coupes2, NULL, NULL);
                    #pragma omp critical
                    {
                        if (local_score < score)
                        {
                            beta = local_score;
                            score = local_score;
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
    memset(pv, 0, sizeof(pv)); // Zero-initialize PV array
    int pv_length = 0;
    long dummy_nodes = 0, dummy_cuts = 0;
    minmax_ab_pv(conf, MAX, hauteur, -INFINI, +INFINI, &dummy_nodes, &dummy_cuts, pv, &pv_length);
    printf("Best move sequence (principal variation): ");
    char move_alg[5];
    for (int k = 0; k < pv_length; k++) {
        printf("%s ", pv[k].move_uci);
    }
    printf("\n\n");

    // --- BEGIN: PV legality and board print check ---
    printf("[DEBUG] pv_length = %d\n", pv_length);
    for (int k = 0; k < pv_length; k++) {
        printf("[DEBUG] pv[%d].move_uci = '%s'\n", k, pv[k].move_uci);
    }
    struct config pv_board;
    copier(&conf, &pv_board); // Start from the current board
    int pv_legal = 1;
    printf("Stepping through PV moves and board states:\n");
    if (pv_length < 0 || pv_length > 100) {
        printf("[ERROR] pv_length out of bounds: %d\n", pv_length);
        pv_legal = 0;
    } else {
        for (int k = 0; k < pv_length; k++) {
            printf("[DEBUG] k = %d, pv[k].move_uci = '%s'\n", k, pv[k].move_uci);
            // Defensive: check move_uci is not empty and looks like a UCI move
            int len = strlen(pv[k].move_uci);
            if (len < 4 || len > 5) {
                printf("[ERROR] pv[%d].move_uci has invalid length (%d): '%s'\n", k, len, pv[k].move_uci);
                pv_legal = 0;
                break;
            }
            for (int ch = 0; ch < len; ch++) {
                if (!( (pv[k].move_uci[ch] >= 'a' && pv[k].move_uci[ch] <= 'h') || (pv[k].move_uci[ch] >= '1' && pv[k].move_uci[ch] <= '8') )) {
                    printf("[ERROR] pv[%d].move_uci contains invalid char: '%s'\n", k, pv[k].move_uci);
                    pv_legal = 0;
                    break;
                }
            }
            if (!pv_legal) break;
            // Generate all legal moves from current position
            struct config T[100];
            int n_moves = 0;
            if ((k % 2) == 0) // White to move
                generer_succ(pv_board, MAX, T, &n_moves);
            else // Black to move
                generer_succ(pv_board, MIN, T, &n_moves);
            printf("[DEBUG] n_moves = %d\n", n_moves);
            if (n_moves < 0 || n_moves > 100) {
                printf("[ERROR] n_moves out of bounds: %d\n", n_moves);
                pv_legal = 0;
                break;
            }
            int found = 0;
            for (int m = 0; m < n_moves; m++) {
                if (strncmp(T[m].move_uci, pv[k].move_uci, 5) == 0) {
                    found = 1;
                    copier(&T[m], &pv_board); // Play the move
                    break;
                }
            }
            if (!found) {
                printf("Warning: Illegal move in PV at ply %d: %s\n", k+1, pv[k].move_uci);
                pv_legal = 0;
                break;
            }
            printf("Board after move %s:\n", pv[k].move_uci);
            print_board_after_move(&pv_board);
        }
    }
    if (pv_legal) {
        printf("PV is legal.\n");
    } else {
        printf("PV is NOT legal.\n");
    }
    // --- END: PV legality and board print check ---

    // Also write PV to results.txt for UI parsing
    // LEGALITY CHECK: Only output PV moves that are legal from the current board state
    f = fopen("results.txt", "w"); // Overwrite file, not append
    if (f) {
        struct config pv_board;
        copier(&conf, &pv_board); // Start from final board after search
        int legal = 1;
        fputs("\n", f);
        for (int k = 0; k < pv_length; k++) {
            // Generate all legal moves from current position
            struct config T[100];
            int n_moves = 0;
            if ((k % 2) == 0) // White to move
                generer_succ(pv_board, MAX, T, &n_moves);
            else // Black to move
                generer_succ(pv_board, MIN, T, &n_moves);
            int found = 0;
            for (int m = 0; m < n_moves; m++) {
                if (strncmp(T[m].move_uci, pv[k].move_uci, 5) == 0) { // Compare full UCI move
                    found = 1;
                    copier(&T[m], &pv_board); // Play the move
                    break;
                }
            }
            if (!found) {
                fprintf(stderr, "Warning: Illegal move in PV at ply %d: %s\n", k+1, pv[k].move_uci);
                fputs("# ERROR: Illegal move in PV\n", f);
                legal = 0;
                break;
            }
            fputs(pv[k].move_uci, f);
            fputs(" ", f);
        }
        fputs("\n", f);
        fclose(f);
    }

    // Write all required lines to results.txt (overwrite)
    f = fopen("results.txt", "w");
    if (f) {
        // Node counts
        snprintf(res, 30, "%ld", nb_noeuds1);
        fputs(res, f);
        fputs("\n", f);
        snprintf(res, 30, "%ld", nb_noeuds2);
        fputs(res, f);
        fputs("\n", f);
        // Sequential times
        for (iiii = 0; iiii < cpt; iiii++) {
            snprintf(res, 30, "%f", stats1[iiii]);
            fputs(res, f);
            fputs(" ", f);
        }
        fputs("\n", f);
        // Parallel times
        for (iiii = 0; iiii < cpt; iiii++) {
            snprintf(res, 30, "%f", stats2[iiii]);
            fputs(res, f);
            fputs(" ", f);
        }
        fputs("\n", f);
        // Output all best PVs from the root
        struct config root_T[100];
        int root_n = 0;
        generer_succ(conf, MAX, root_T, &root_n);
        int best_score = -INFINI;
        int scores[100];
        for (int i = 0; i < root_n; i++) {
            long dummy_nodes = 0, dummy_cuts = 0;
            int pv_length = 0;
            struct move pv[100];
            int score = minmax_ab_pv(root_T[i], MIN, hauteur-1, -INFINI, +INFINI, &dummy_nodes, &dummy_cuts, pv, &pv_length);
            scores[i] = score;
            if (score > best_score) best_score = score;
        }
        for (int i = 0; i < root_n; i++) {
            if (scores[i] == best_score) {
                // Write PV for this root move
                struct config pv_board;
                copier(&conf, &pv_board);
                // Play the root move
                copier(&root_T[i], &pv_board);
                fputs("PV:", f);
                fputs(root_T[i].move_uci, f);
                fputs(" ", f);
                long dummy_nodes = 0, dummy_cuts = 0;
                int pv_length = 0;
                struct move pv[100];
                minmax_ab_pv(root_T[i], MIN, hauteur-1, -INFINI, +INFINI, &dummy_nodes, &dummy_cuts, pv, &pv_length);
                for (int k = 0; k < pv_length; k++) {
                    // Generate all legal moves from current position
                    struct config T[100];
                    int n_moves = 0;
                    if ((k % 2) == 0)
                        generer_succ(pv_board, MIN, T, &n_moves);
                    else
                        generer_succ(pv_board, MAX, T, &n_moves);
                    int found = 0;
                    for (int m = 0; m < n_moves; m++) {
                        if (strncmp(T[m].move_uci, pv[k].move_uci, 5) == 0) {
                            found = 1;
                            copier(&T[m], &pv_board);
                            break;
                        }
                    }
                    if (!found) break;
                    fputs(pv[k].move_uci, f);
                    fputs(" ", f);
                }
                fputs("\n", f);
            }
        }
        // --- Output the best continuation tree ---
        fputs("TREE\n", f);
        output_best_tree(f, conf, MAX, 0, hauteur);
        // --- End tree output ---
        fclose(f);
    }

    // ... existing code ...
    minmax_ab_pv(conf, MAX, hauteur, -INFINI, +INFINI, &dummy_nodes, &dummy_cuts, pv, &pv_length);
    printf("[DEBUG] After minmax_ab_pv: pv_length = %d\n", pv_length);
    for (int k = 0; k < pv_length && k < 100; k++) {
        printf("[DEBUG] pv[%d].move_uci = '%s'\n", k, pv[k].move_uci);
    }
    if (pv_length < 0 || pv_length > 100) {
        printf("[FATAL] pv_length out of bounds: %d\n", pv_length);
        exit(1);
    }
    // ... existing code ...

    return 0;
}



