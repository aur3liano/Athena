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
Returns what Athena thinks is the 2ply minimax move

def Minimax_Move(gs, Valid_Moves):
    #Initialize a random move to avoid errors
    Best_Move = Make_Rand_Move(Valid_Moves)
    #Clarify objective depending on who the AI is playing for
    if (gs.White_to_Move):
        best_score = b_checkmate
        obj_1 = b_checkmate
        attempt_to_1 = "maximize"
        draw_contempt_1 = -0.1   #Make draw slightly less desirable
        obj_2 = w_checkmate
        attempt_to_2 = "minimize"
        draw_contempt_2 = 0.1   #Make draw slightly less desirable
    else:
        best_score = w_checkmate
        obj_1 = w_checkmate
        attempt_to_1 = "minimize"
        draw_contempt_1 = 0.1    #Make draw slightly less desirable
        obj_2 = b_checkmate
        attempt_to_2 = "maximize"
        draw_contempt_2 = -0.1   #Make draw slightly less desirable
    #Find the least best move the opponent can make
    Oppo_Best_Scores = []
    #Introduce variety of play
    #random.shuffle(Valid_Moves)
    for Player_Move in Valid_Moves:
        gs.Make_Move(Player_Move)
        Oppo_Moves = gs.Get_Valid_Moves()
        if (gs.Checkmate):
            gs.Undo_Move()
            return(Player_Move)
        elif (gs.Stalemate):
            gs.Undo_Move()
            score = stalemate + draw_contempt_1
            Oppo_Best_Scores.append(score)
            continue
        else:
            oppo_best_score = obj_2    #Initialize the best score as the worst outcome
            oppo_scores = []
            for Oppo_Move in Oppo_Moves:    
                gs.Make_Move(Player_Move)
                #Check if the move results in checkmate or stalemate
                if (gs.Checkmate):
                    score = obj_2
                elif (gs.Stalemate):
                    score = stalemate + draw_contempt_2
                else:
                    score = Score_Material(gs.board)
                #Track all scores 2 ply deep
                oppo_scores.append(score)
                gs.Undo_Move()
            #Attempt to improve the best score for the opponent at 2 ply depth
            if (attempt_to_2 == "minimize"):
                oppo_best_test = min(oppo_scores)
                if (oppo_best_test < oppo_best_score):
                    oppo_best_score = oppo_best_test
            elif (attempt_to_1 == "maximize"):
                oppo_best_test = max(oppo_scores)
                if (oppo_best_test > oppo_best_score):
                    oppo_best_score = oppo_best_test
            Oppo_Best_Scores.append(oppo_best_score)
            gs.Undo_Move()
    #Select the best score for self at 1 ply depth given the best scores of the opponent
    if (attempt_to_1 == "maximize"):
        test_score = max(Oppo_Best_Scores)
        if (test_score > best_score):
            best_score = test_score
        best_score_indx = Oppo_Best_Scores.index(best_score)
    elif (attempt_to_1 == "minimize"):
        test_score = min(Oppo_Best_Scores)
        if (test_score > best_score):
            best_score = test_score
        best_score_indx = Oppo_Best_Scores.index(best_score)
    #Select the move with the best score index
    Best_Move = Valid_Moves[best_score_indx]
    return(Best_Move)
'''

def Minimax_Move(gs, Valid_Moves):
    Turn_Multiplier = 1 if gs.White_to_Move else -1
    Oppo_minmax_score = w_checkmate
    Best_Move = Make_Rand_Move(Valid_Moves)
    random.shuffle(Valid_Moves)
    for Player_Move in Valid_Moves:
        gs.Make_Move(Player_Move)
        Oppo_Moves = gs.Get_Valid_Moves()
        Oppo_max_score = -w_checkmate
        for Oppo_Move in Oppo_Moves:
            gs.Make_Move(Oppo_Move)
            if (gs.Checkmate):
                score = -Turn_Multiplier * w_checkmate
            elif (gs.Stalemate):
                score = stalemate
            else:
                score = Score_Material(gs.board)
            if (score > Oppo_max_score):
                Oppo_max_score = score
            gs.Undo_Move()
        if (Oppo_max_score < Oppo_minmax_score):
            Oppo_minmax_score = Oppo_max_score
            Best_Move = Player_Move
        gs.Undo_Move()
    return(Best_Move)

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