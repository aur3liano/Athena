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
Recursive Minimax without alpha-beta pruning
Also modified to be only one for loop (negamax)
'''
def Negamax_Move(gs, Valid_Moves, depth, turn_multiplier):
    global next_move
    global counter
    counter += 1
    if (depth == 0):
        score = turn_multiplier * Score_Board(gs)
        return(score)
    else:
        #IMPLEMENT MOVE ORDERING LATER
        #With negamax the initialized max score is the same for both players
        max_score = -checkmate_score
        for Player_Move in Valid_Moves:
            gs.Make_Move(Player_Move)
            next_moves = gs.Get_Valid_Moves()
            score = -Negamax_Move(gs, Valid_Moves, depth-1, -turn_multiplier)
            if (score > max_score):
                max_score = score
            elif (score > max_score) and (depth == max_depth):
                max_score = score
                next_move = Player_Move
            gs.Undo_Move()
        return(max_score)

'''
Recursive Minimax with alpha-beta pruning
Also modified to be only one for loop (negamax)
'''
def Negamax_Alpha_Beta_Pruning_Move(gs, Valid_Moves, depth, alpha, beta, turn_multiplier):
    global next_move
    global counter
    counter += 1
    if (depth == 0):
        score = turn_multiplier * Score_Board(gs)
        return(score)
    else:
        #IMPLEMENT MOVE ORDERING LATER
        #With negamax the initialized max score is the same for both players
        max_score = -checkmate_score
        for Player_Move in Valid_Moves:
            gs.Make_Move(Player_Move)
            next_moves = gs.Get_Valid_Moves()
            score = -Negamax_Alpha_Beta_Pruning_Move(gs, Valid_Moves, depth-1, -beta, -alpha, -turn_multiplier)
            if (score > max_score):
                max_score = score
            elif (score > max_score) and (depth == max_depth):
                max_score = score
                next_move = Player_Move
            gs.Undo_Move()
            #Alpha-Beta pruning
            if (max_score > alpha):
                alpha = max_score
            if (alpha >= beta):
                break
        return(max_score)

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
    if (gs.White_to_Move):
        turn_multiplier = 1
    else:
        turn_multiplier = -1
    #score = Negamax_Move(gs, Valid_Moves, max_depth, turn_multiplier)
    
    score = Negamax_Alpha_Beta_Pruning_Move(gs, Valid_Moves, max_depth,
                                            -checkmate_score, checkmate_score,
                                            turn_multiplier)
    
    end_time = round(time.time() - start_time,3)
    time_prnt_stmt = str(round(score,4)) + " at " + str(end_time)
    time_prnt_stmt = time_prnt_stmt + " seconds considering " + str(counter) + " board positions"
    #print(time_prnt_stmt)
    return(next_move)