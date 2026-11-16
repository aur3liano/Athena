# -*- coding: utf-8 -*-
"""
Created on Tue Aug 17 22:04:48 2021

Chess Engine Tournament script

@author: Justin Paul
"""

import Official_Game_State as Game_State_Classes
import Athena_0_Alpha_Beta_Pruning as AI_engine_1
import Athena_1_0 as AI_engine_2
import time

def Engine_Game():
    gs = Game_State_Classes.Game_State()
    Valid_Moves = gs.Get_Valid_Moves()
    Game_Over = False
    #Assume a draw
    end_result_w = 0.5
    end_result_b = 0.5
    white_time = 0
    black_time = 0
    while (not Game_Over):
        if (not gs.White_to_Move):
            mv_start_time = time.time()
            AI_move = AI_engine_2.Best_Move(gs, Valid_Moves)
            mv_finsh_time = time.time()
            black_time += (mv_finsh_time - mv_start_time)
            gs.Make_Move(AI_move)
        elif (gs.White_to_Move):
            mv_start_time = time.time()
            AI_move = AI_engine_1.Best_Move(gs, Valid_Moves)
            mv_finsh_time = time.time()
            white_time += (mv_finsh_time - mv_start_time)
            gs.Make_Move(AI_move)
        Valid_Moves = gs.Get_Valid_Moves()
        if ((gs.Checkmate) and (gs.White_to_Move)):
            Game_Over = True
            end_result_w = 0
            end_result_b = 1
            draw_type = 'No Draw'
            #print("Checkmate, Black wins (0-1)")
        elif ((gs.Checkmate) and (not gs.White_to_Move)):
            Game_Over = True
            end_result_w = 1
            end_result_b = 0
            draw_type = 'No Draw'
            #print("Checkmate, White wins (1-0)")
        elif ((gs.Stalemate) and (not gs.Checkmate)):
            #Check if it is special draw
            draw_type = 'Stalemate'
            if (gs.Three_Move_Rep):
                draw_type = 'Three Fold Repetition'
            elif (gs.Dead_Position):
                draw_type = 'Dead Position' 
            elif (gs.Fifty_Move_Rule):
                draw_type = 'Fifty Move Rule'
            Game_Over = True
            stalemate_prnt = "Draw by " + draw_type + " (0.5-0.5)"
            #print(stalemate_prnt)
    return(end_result_w,end_result_b,draw_type,white_time,black_time)

def Engine_Tournament():
    num_of_games = ((60) * 1)
    total_time_start = time.time()
    total_white_time = 0
    total_black_time = 0
    prog_prnt_inc = int(600)
    prnt_time = 0
    total_tour_pnts_w = 0
    draws = 0
    total_tour_pnts_b = 0
    draw_dict = {}
    for nog in range(num_of_games):
        [end_result_w,end_result_b,draw_type,white_time,black_time] = Engine_Game()
        total_white_time += white_time
        total_black_time += black_time
        if ((end_result_w == 0.5) and (end_result_b == 0.5)):
            draws += (end_result_w + end_result_b)
            if (draw_type not in draw_dict):
                draw_dict[draw_type] = 1
            else:
                draw_dict[draw_type] += 1
        else:
            total_tour_pnts_w += end_result_w
            total_tour_pnts_b += end_result_b
        if (prnt_time < (time.time() - total_time_start)):
            #Calculate progress
            prog = round(((nog+1)/num_of_games)*100,3)
            prog_prnt_stmt = "Progress at: " + str(prog) + "%"
            print(prog_prnt_stmt)
            #Increase print time
            prnt_time += prog_prnt_inc
            w_tour_res = "White current result: " + str(int(total_tour_pnts_w))
            b_tour_res = "Black current result: " + str(int(total_tour_pnts_b))
            draws_res = "current Draws result: " + str(int(draws))
            print(w_tour_res)
            print(b_tour_res)
            print(draws_res)
            print()
    total_time_end = round(time.time() - total_time_start,3)
    w_tour_res = "White tournament result: " + str(int(total_tour_pnts_w))
    b_tour_res = "Black tournament result: " + str(int(total_tour_pnts_b))
    draws_res = "Tournament Draws result: " + str(int(draws))
    print(w_tour_res)
    print(b_tour_res)
    print(draws_res)
    print()
    #Print draw type frequencies
    for draw_type in draw_dict:
        draw_freq = round((draw_dict[draw_type]/draws)*100,2)
        draw_freq_prnt_stmt = draw_type + " with frequency of " + str(draw_freq) + "%"
        print(draw_freq_prnt_stmt)
    print()
    time_prnt_stmt = "at " + str(total_time_end) + " seconds"
    white_time_prnt_stmt = "Engine for white took " + str(round(total_white_time,3)) + " seconds"
    black_time_prnt_stmt = "Engine for black took " + str(round(total_black_time,3)) + " seconds"
    print(white_time_prnt_stmt)
    print(black_time_prnt_stmt)
    print(time_prnt_stmt)
    return(1)

Engine_Tournament()