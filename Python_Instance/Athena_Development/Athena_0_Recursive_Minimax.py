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
max_depth = 3

'''
Rather self explanatory, a random move is generated
'''
def Make_Rand_Move(Valid_Moves):
    move_selected = Valid_Moves[random.randint(0,(len(Valid_Moves)-1))]
    return(move_selected)

'''
Score the board based on material
'''
def Score_Material(board):
    score = 0
    for row in board:
        for square in row:
            if (square[0] == "w"):
                score += Opening_Piece_Valuation[square[1]]
            elif (square[0] == "b"):
                score += -Opening_Piece_Valuation[square[1]]
    return(score)

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
Returns what Athena thinks is the most greedy move
'''
def Greedy_Move(gs, Valid_Moves):
    #Initialize a random move to avoid errors
    Best_Move = Make_Rand_Move(Valid_Moves)
    #Clarify objective depending on who the AI is playing for
    if (gs.White_to_Move):
        best_score = b_checkmate
        obj = w_checkmate
        attempt_to = "maximize"
        draw_contempt = -0.1   #Make draw slightly less desirable
    else:
        best_score = w_checkmate
        obj = b_checkmate
        attempt_to = "minimize"
        draw_contempt = 0.1    #Make draw slightly less desirable
    #Introduce variety of play
    random.shuffle(Valid_Moves)
    #Find the best move
    for Player_Move in Valid_Moves:
        gs.Make_Move(Player_Move)
        #Check if the move results in checkmate or stalemate
        if (gs.Checkmate):
            score = obj
        elif (gs.Stalemate):
            score = stalemate + draw_contempt
        else:
            score = Score_Material(gs.board)
        if ((score > best_score) and (attempt_to == "maximize")):
            best_score = score    
            Best_Move = Player_Move    
        elif ((score < best_score) and (attempt_to == "minimize")):
            best_score = score
            Best_Move = Player_Move
        gs.Undo_Move()
    return(Best_Move)

'''
Minimax for playing as White
'''
def White_Minimax_Move(gs, Valid_Moves):
    #These parameters are currently configured for playing as White
    draw_contempt = 0.1
    Black_minmax_score = b_checkmate
    #Initialize best move as a random move to avoid errors
    Best_Move = Make_Rand_Move(Valid_Moves)
    #Introduce variety of play
    random.shuffle(Valid_Moves)
    for Player_Move in Valid_Moves:
        #Make the move as White
        gs.Make_Move(Player_Move)
        #Get all of Blacks responses
        Black_Moves = gs.Get_Valid_Moves()
        #Initialize Blacks worst response
        Black_min_score = w_checkmate
        for Black_Move in Black_Moves:
            #Test Whites response
            gs.Make_Move(Black_Move)
            #Score the response
            if (gs.Checkmate):
                score = b_checkmate
            elif (gs.Stalemate):
                score = (stalemate - draw_contempt)
            else:
                score = Score_Material(gs.board)
            #Update best score for Black given Whites choice
            if (score < Black_min_score):
                Black_min_score = score
            #Undo Whites response
            gs.Undo_Move()
        #Update best move for White
        if (Black_min_score > Black_minmax_score):
            Black_minmax_score = Black_min_score
            Best_Move = Player_Move
        #Undo Whites move
        gs.Undo_Move()
    return(Best_Move)

'''
Minimax for playing as Black
'''
def Black_Minimax_Move(gs, Valid_Moves):
    #These parameters are currently configured for playing as Black
    draw_contempt = -0.1
    White_minmax_score = w_checkmate
    #Initialize best move as a random move to avoid errors
    Best_Move = Make_Rand_Move(Valid_Moves)
    #Introduce variety of play
    random.shuffle(Valid_Moves)
    for Player_Move in Valid_Moves:
        #Make the move as Black
        gs.Make_Move(Player_Move)
        #Get all of Whites responses
        White_Moves = gs.Get_Valid_Moves()
        #Initialize Whites worst response
        White_max_score = b_checkmate
        for White_Move in White_Moves:
            #Test Whites response
            gs.Make_Move(White_Move)
            #Score the response
            if (gs.Checkmate):
                score = w_checkmate
            elif (gs.Stalemate):
                score = (stalemate - draw_contempt)
            else:
                score = Score_Material(gs.board)
            #Update best score for Black given Whites choice
            if (score > White_max_score):
                White_max_score = score
            #Undo Blacks response
            gs.Undo_Move()
        #Update best move for White
        if (White_max_score < White_minmax_score):
            White_minmax_score = White_max_score
            Best_Move = Player_Move
        #Undo Whites move
        gs.Undo_Move()
    return(Best_Move)

'''
Uses minimax algorithm recurrsively
'''
def Minimax_Recursive_Move(gs, Valid_Moves, depth, White_to_Move):
    global next_move
    if depth == 0:
        score = Score_Board(gs)
        return(score)
    elif (White_to_Move):
        max_score = b_checkmate
        for Player_Move in Valid_Moves:
            gs.Make_Move(Player_Move)
            next_moves = gs.Get_Valid_Moves()
            score = Minimax_Recursive_Move(gs, next_moves, depth-1, False)
            if (score > max_score):
                max_score = score
            elif (score > max_score) and (depth == max_depth):
                max_score = score
                next_move = Player_Move
            gs.Undo_Move()
        return(max_score)
    else:
        min_score = w_checkmate
        for Player_Move in Valid_Moves:
            gs.Make_Move(Player_Move)
            next_moves = gs.Get_Valid_Moves()
            score = Minimax_Recursive_Move(gs, next_moves, depth-1, True)
            if (score < min_score):
                min_score = score
            elif (score < min_score) and (depth == max_depth):
                min_score = score
                next_move = Player_Move
            gs.Undo_Move()
        return(min_score)

'''
Helper method for first recursive minimax function call
'''
def Minimax_Move(gs, Valid_Moves):
    global next_move
    start_time = time.time()
    next_move = Make_Rand_Move(Valid_Moves)
    if (gs.White_to_Move):
        score = b_checkmate
    else:
        score = w_checkmate
    score = Minimax_Recursive_Move(gs, Valid_Moves, max_depth, gs.White_to_Move)
    end_time = round(time.time() - start_time,3)
    time_prnt_stmt = str(round(score,4)) + " at " + str(end_time) + " seconds"
    print(time_prnt_stmt)
    return(next_move)