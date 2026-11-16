# -*- coding: utf-8 -*-
"""
Created on Tue Aug 17 10:50:08 2021

Chess player for mini, offical and Grand chess

@author: Justin Paul
"""

import random

Opening_Piece_Valuation = {"K":0,"P":1,"N":3.05,"B":3.33,"R":5.63,"Q":9.5}  #Values taken from alpha zero
w_checkmate = float("inf")
stalemate = 0    #A small amount of contempt will be added for respective players because draws are boring
b_checkmate = float("-inf")

'''
Rather self explanatory, a random move is generated
'''
def Make_Rand_Move(Valid_Moves):
    move_selected = Valid_Moves[random.randint(0,(len(Valid_Moves)-1))]
    return(move_selected)

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
def Minimax_Move(gs, Valid_Moves):
    #Initialize best move as a random move to avoid errors
    Best_Move = Make_Rand_Move(Valid_Moves)
    if (gs.White_to_Move):
        draw_contempt = 0.1
        minmax_score = b_checkmate
        score_init = w_checkmate
        checkmate_score = b_checkmate
        attempt_to_1 = "minimize"
        attempt_to_2 = "maximize"
    else:
        draw_contempt = -0.1
        minmax_score = w_checkmate
        score_init = b_checkmate
        checkmate_score = w_checkmate
        attempt_to_1 = "maximize"
        attempt_to_2 = "minimize"
    #Introduce variety of play
    random.shuffle(Valid_Moves)
    for P1_Move in Valid_Moves:
        gs.Make_Move(P1_Move)
        P2_Moves = gs.Get_Valid_Moves()
        P2_score = score_init
        for P2_Move in P2_Moves:
            gs.Make_Move(P2_Move)
            if (gs.Checkmate):
                score = checkmate_score
            elif (gs.Stalemate):
                score = (stalemate - draw_contempt)
            else:
                score = Score_Material(gs.board)
            if ((score < P2_score) and (attempt_to_2 == "minimize")):
                P2_score = score
            elif ((score > P2_score) and (attempt_to_2 == "maximize")):
                P2_score = score
            gs.Undo_Move
        if ((P2_score < minmax_score) and (attempt_to_1 == "minimize")):
            minmax_score = P2_score
            Best_Move = P1_Move
        elif ((P2_score > minmax_score) and (attempt_to_1 == "maximize")):
            minmax_score = P2_score
            Best_Move = P1_Move
        gs.Undo_Move
    return(Best_Move)
'''

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