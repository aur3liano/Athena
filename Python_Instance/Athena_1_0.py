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
max_depth = 4

'''
Piece positional scoring
'''

Knight_Pos_Score = [[-1,1,1,1,1,1,1,-1],
                    [1, 1,2,2,2,2,1, 1],
                    [1, 2,3,4,4,3,2, 1],
                    [1, 2,4,5,5,4,2, 1],
                    [1, 3,4,5,5,4,2, 1],
                    [1, 2,3,4,4,3,2, 1],
                    [1, 1,2,1,1,2,1, 1],
                    [-1,1,1,1,1,1,1,-1]]

Bishop_Pos_Score = [[2,1,1,2,2,1,1,2],
                    [1,3,2,1,1,2,3,1],
                    [1,2,4,4,4,4,2,1],
                    [2,1,4,5,5,4,1,2],
                    [2,1,4,5,5,4,1,2],
                    [1,2,4,4,4,4,2,1],
                    [1,3,2,1,1,2,3,1],
                    [2,1,1,2,2,1,1,2]]

White_Pawn_Pos_Score = [[9,9,9,9,9,9,9,9],
                        [4,5,7,8,8,7,5,4],
                        [3,4,5,6,6,5,4,3],
                        [2,3,3,4,4,3,3,2],
                        [2,2,2,1,1,2,2,2],
                        [1,1,1,1,1,1,1,1],
                        [0,0,0,0,0,0,0,0],
                        [0,0,0,0,0,0,0,0]]

White_Rook_Pos_Score = [[2,3,4,5,5,4,3,2],
                        [7,7,7,7,7,7,7,7],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [2,3,4,5,5,4,3,2]]

White_Queen_Pos_Score = [[1,2,3,3,3,3,2,1],
                         [2,5,6,7,7,6,5,2],
                         [1,1,2,4,4,2,1,1],
                         [1,1,3,3,3,3,1,1],
                         [1,1,2,3,3,2,1,1],
                         [1,1,3,2,2,3,1,1],
                         [2,4,4,5,5,4,4,2],
                         [1,2,3,4,4,3,2,1]]

White_King_Pos_Score = [[0,0, 0, 0, 0, 0,0,0],
                        [0,0, 0, 0, 0, 0,0,0],
                        [0,0, 0, 0, 0, 0,0,0],
                        [0,0, 0, 0, 0, 0,0,0],
                        [0,0, 0,-3,-3, 0,0,0],
                        [0,0, 0,-2,-2, 0,0,0],
                        [1,1,-1,-1,-1,-1,1,1],
                        [2,4, 3, 1, 1, 2,4,2]]

Black_Pawn_Pos_Score = [[0,0,0,0,0,0,0,0],
                        [0,0,0,0,0,0,0,0],
                        [1,1,1,1,1,1,1,1],
                        [2,2,2,1,1,2,2,2],
                        [2,3,3,4,4,3,3,2],
                        [3,4,5,6,6,5,4,3],
                        [4,5,7,8,8,7,5,4],
                        [9,9,9,9,9,9,9,9]]

Black_Rook_Pos_Score = [[2,3,4,5,5,4,3,2],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [1,2,2,4,4,2,2,1],
                        [7,7,7,7,7,7,7,7],
                        [2,3,4,5,5,4,3,2]]

Black_Queen_Pos_Score = [[1,2,3,4,4,3,2,1],
                         [2,4,4,5,5,4,4,2],
                         [1,1,3,2,2,3,1,1],
                         [1,1,2,3,3,2,1,1],
                         [1,1,3,3,3,3,1,1],
                         [1,1,2,4,4,2,1,1],
                         [2,5,6,7,7,6,5,2],
                         [1,2,3,3,3,3,2,1]]

Black_King_Pos_Score = [[2,4, 3, 1, 1, 2,4,2],
                        [1,1,-1,-1,-1,-1,1,1],
                        [0,0, 0,-2,-2, 0,0,0],
                        [0,0, 0,-3,-3, 0,0,0],
                        [0,0, 0, 0, 0, 0,0,0],
                        [0,0, 0, 0, 0, 0,0,0],
                        [0,0, 0, 0, 0, 0,0,0],
                        [0,0, 0, 0, 0, 0,0,0]]


White_Pos_Score_Dict = {"wK":White_King_Pos_Score,"wP":White_Pawn_Pos_Score,"wN":Knight_Pos_Score,"wB":Bishop_Pos_Score,"wR":White_Rook_Pos_Score,"wQ":White_Queen_Pos_Score}
Black_Pos_Score_Dict = {"bK":Black_King_Pos_Score,"bP":Black_Pawn_Pos_Score,"bN":Knight_Pos_Score,"bB":Bishop_Pos_Score,"bR":Black_Rook_Pos_Score,"bQ":Black_Queen_Pos_Score}

'''
Move Ordering Values
and
Depth-Based-Moves-Evaluated-Fraction-Reduction (DBMEFR)
'''
MO_Piece_Valuation_A = {"P":1,"K":2,"N":3.05,"B":3.33,"R":5.63,"Q":9.5}
MO_Piece_Valuation_O = {"P":1,"N":3.05,"B":3.33,"K":4,"R":5.63,"Q":9.5}
DBMEFR_vect = [0.55,0.65,0.75,0.85]

'''
Sorting a 1-dimensional list
'''
def Sort_1D_List(Vect):
    #Preserve original unsorted data structures
    old_dynamic_unsort_val = Vect
    #Assign identity to each Vect item
    old_dynamic_unsort_ide = [0]*len(Vect)
    for v in range(len(Vect)):
        old_dynamic_unsort_ide[v] = v
    #Initialize lists to hold sorted values
    sort_ide_vect = [0]*len(Vect)
    sort_val_vect = [0]*len(Vect)
    #Initialize sorted values counter
    val_sort_counter = 0
    #Sort the vector
    while (old_dynamic_unsort_ide != []):
        #Initialize the dynamic minimum
        dynamic_min = float("inf")
        #Find the actual dynamic minimum(s) of the old data structures
        for j in range(len(old_dynamic_unsort_val)):
            if (old_dynamic_unsort_val[j] < dynamic_min):
                dynamic_min = old_dynamic_unsort_val[j]
        #Collect all indicies for values equal to dynamic minimum
        dynamic_min_indices = []
        for j in range(len(old_dynamic_unsort_val)):
            if (old_dynamic_unsort_val[j] == dynamic_min):
                dynamic_min_indices.append(j)
        #Initialize new dynamic data structures for building
        new_dynamic_unsort_val = [0]*(len(old_dynamic_unsort_val)-len(dynamic_min_indices))
        new_dynamic_unsort_ide = [0]*(len(old_dynamic_unsort_ide)-len(dynamic_min_indices))
        #Build the new dynamic data structures
        build_counter = 0
        for k in range(len(old_dynamic_unsort_val)):
            #Exclude the actual dynamic minimum
            if (old_dynamic_unsort_val[k] == dynamic_min):
                continue
            else:
                new_dynamic_unsort_val[build_counter] = old_dynamic_unsort_val[k]
                new_dynamic_unsort_ide[build_counter] = old_dynamic_unsort_ide[k]
                build_counter += 1
        #Pass the actual dynamic minimum to the sorted data structures
        for m in range(len(dynamic_min_indices)):
            sort_ide_vect[val_sort_counter] = old_dynamic_unsort_ide[dynamic_min_indices[m]]
            sort_val_vect[val_sort_counter] = dynamic_min
            #Increment the sorted values counter
            val_sort_counter += 1
        #Reset the old dynamic data structures
        old_dynamic_unsort_ide = new_dynamic_unsort_ide
        old_dynamic_unsort_val = new_dynamic_unsort_val
    return(sort_val_vect,sort_ide_vect)

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
    pos_amp_fact = 0.1
    score = 0
    if ((gs.Checkmate) and (gs.White_to_Move)):
        return(b_checkmate)
    elif ((gs.Checkmate) and (not gs.White_to_Move)):
        return(w_checkmate)
    elif ((gs.Stalemate) and (not gs.Checkmate)):
        return(stalemate)
    else:
        board = gs.board
        for r in range(len(board)):
            row = board[r]
            for c in range(len(row)):
                square = board[r][c]
                if (square == "--"):
                    continue
                elif (square[0] == "w"):
                    pos_score = (White_Pos_Score_Dict[square][r][c]*pos_amp_fact)
                    score += (Opening_Piece_Valuation[square[1]] + pos_score)
                elif (square[0] == "b"):
                    pos_score = (Black_Pos_Score_Dict[square][r][c]*pos_amp_fact)
                    score += -(Opening_Piece_Valuation[square[1]] + pos_score)
    return(score)

'''
Order a list of moves
'''
def Order_Moves(gs, Valid_Moves):
    #First check for moves that deilver checkmate
    for Player_Move in Valid_Moves:
        gs.Make_Move(Player_Move)
        if (gs.Checkmate == True):
            piece_diff_values = float("inf")
        else:
            #Acquire list of piece values for each move
            piece_diff_values = []
            for vm in range(len(Valid_Moves)):
                piece = Valid_Moves[vm].Piece_Moved
                piece_capt = Valid_Moves[vm].Piece_Captured
                if (piece_capt == "--"):
                    piece_capt = 0
                else:
                    piece_capt = MO_Piece_Valuation_O[piece_capt[1]]
                piece = MO_Piece_Valuation_A[piece[1]]
                piece_diff = (piece_capt - piece)
                piece_diff_values.append(piece_diff)
            #Order the list of piece values
            [sort_val_vect,sort_ide_vect] = Sort_1D_List(piece_diff_values)
            Ordered_Moves = []
            for vm in range(len(Valid_Moves)):
                Ordered_Moves.append(Valid_Moves[sort_ide_vect[vm]])
    return(Ordered_Moves)

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
        Ordered_Moves = Order_Moves(Valid_Moves)
        #Quantity of ordered moves
        Num_moves_ordered = len(Ordered_Moves)
        MEF_for_Eval = int(round((Num_moves_ordered*DBMEFR_vect[depth-1]),0))
        for mfe in range(MEF_for_Eval):
            Player_Move = Ordered_Moves[mfe]
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
        Ordered_Moves = Order_Moves(Valid_Moves)
        #Quantity of ordered moves
        Num_moves_ordered = len(Ordered_Moves)
        MEF_for_Eval = int(round((Num_moves_ordered*DBMEFR_vect[depth-1]),0))
        for mfe in range(MEF_for_Eval):
            Player_Move = Ordered_Moves[mfe]
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
    print(time_prnt_stmt)
    return(next_move)