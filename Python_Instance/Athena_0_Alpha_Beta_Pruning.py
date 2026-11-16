# -*- coding: utf-8 -*-
"""
Created on Tue Aug 17 10:50:08 2021

Chess player for mini, offical and Grand chess

@author: Justin Paul
"""

import random
import time

Opening_Piece_Valuation = {"K":0,"P":1,"N":3.05,"B":3.33,"R":5.63,"Q":9.5}  #Values taken from alpha zero
w_checkmate = float("inf")
stalemate = 0    #A small amount of contempt will be added for respective players because draws are boring
b_checkmate = float("-inf")
checkmate_score = float("inf")
max_depth = 3

'''
Rather self explanatory, a random move is generated
'''
def Make_Rand_Move(Valid_Moves):
    move_selected = Valid_Moves[random.randint(0,(len(Valid_Moves)-1))]
    return(move_selected)

'''
Score the board based on gamestate
'''
def Score_Board(gs):
    score = 0
    if ((gs.Checkmate) and (gs.White_to_Move)):
        return(b_checkmate)
    elif ((gs.Checkmate) and (not gs.White_to_Move)):
        return(w_checkmate)
    elif ((gs.Stalemate) and (not gs.Checkmate)):
        return(stalemate)
    else:
        board = gs.board
        for row in board:
            for square in row:
                if (square[0] == "w"):
                    score += Opening_Piece_Valuation[square[1]]
                elif (square[0] == "b"):
                    score += -Opening_Piece_Valuation[square[1]]
    return(score)

'''
Uses minimax algorithm recurrsively with alpha-beta pruning
'''
def Minimax_Alpha_Beta_Pruning_Move(gs, Valid_Moves, depth, alpha, beta, White_to_Move):
    global next_move
    global counter
    counter += 1
    #Determine if game state is terminal
    term_gs_flg = False
    if ((gs.Checkmate) or (gs.Stalemate)):
        term_gs_flg = True
    if ((depth == 0) or term_gs_flg):
        score = Score_Board(gs)
        return(score)
    elif (White_to_Move):
        max_score = b_checkmate
        for Player_Move in Valid_Moves:
            gs.Make_Move(Player_Move)
            next_moves = gs.Get_Valid_Moves()
            score = Minimax_Alpha_Beta_Pruning_Move(gs, next_moves, depth-1, alpha, beta, False)
            if (score > max_score):
                max_score = score
                if (depth == max_depth):
                    next_move = Player_Move
            gs.Undo_Move()
            if (max_score >= beta):
                break
            #Fail hard
            alpha = max(alpha,max_score)
        return(max_score)
    else:
        min_score = w_checkmate
        for Player_Move in Valid_Moves:
            gs.Make_Move(Player_Move)
            next_moves = gs.Get_Valid_Moves()
            score = Minimax_Alpha_Beta_Pruning_Move(gs, next_moves, depth-1, alpha, beta, True)
            if (score < min_score):
                min_score = score
                if (depth == max_depth):
                    next_move = Player_Move
            gs.Undo_Move()
            if (min_score <= alpha):
                break
            #Fail hard
            beta = min(beta,min_score)
        return(min_score)

'''
Helper method for first search function call
'''
def Best_Move(gs, Valid_Moves):
    global next_move
    global counter
    start_time = time.time()
    counter = 0
    next_move = Make_Rand_Move(Valid_Moves)
    #Introduce variety of play
    random.shuffle(Valid_Moves)
    score = Minimax_Alpha_Beta_Pruning_Move(gs, Valid_Moves, max_depth,
                                            -checkmate_score, checkmate_score,
                                            gs.White_to_Move)
    end_time = round(time.time() - start_time,3)
    time_prnt_stmt = str(round(score,4)) + " at " + str(end_time)
    time_prnt_stmt = time_prnt_stmt + " seconds considering " + str(counter) + " board positions"
    #print(time_prnt_stmt)
    return(next_move)