# -*- coding: utf-8 -*-
"""
Created on Sun Jul 25 17:06:53 2021

This is the Chess Gui main driver file

This code was essentially copied (with small modifications) from
a youtube.com video series by Eddie Sharick

@author: Justin Paul
"""

import pygame as p
import Official_Game_State as Game_State_Classes
import Athena_0_4 as AI_engine_1
import Athena_0_4 as AI_engine_2

DIMENSION = 8    #dimensions of a chess board are 8x8
BOARD_WIDTH = BOARD_HEIGHT = MOVE_LOG_PANEL_HEIGHT = int(64*DIMENSION)
SQ_SIZE = BOARD_HEIGHT//DIMENSION
MOVE_LOG_PANEL_WIDTH = 256
MAX_FPS = 15    #For animation later on
IMAGES = {}

'''
Initialize a global dictionary of images. This will be called exactly once in the main
'''
def loadImages():
    pieces = ["bP","bR","bN","bB","bK","bQ","wP","wR","wN","wB","wK","wQ"]
    for piece in pieces:
        IMAGES[piece] = p.transform.scale(p.image.load("images/" + piece + ".png"), (SQ_SIZE,SQ_SIZE))
    #Note: we can access an image by saying 'IMAGES["wP"]'
    
'''
Draw the squares on the board. The top left square is always light.
'''
def Draw_Board(screen):
    global colors
    colors = [p.Color("white"),p.Color("dark green")]
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            color = colors[((r+c) % 2)]
            p.draw.rect(screen, color, p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))
    
'''
Draw the pieces on the board
'''
def Draw_Pieces(screen,board):
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            piece = board[r][c]
            #Not an empty square
            if (piece != "--"):
                screen.blit(IMAGES[piece], p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))

'''
Highlight squares relevant to user interest
'''
def Highlight_Sqs(screen, gs, Valid_Moves, sq_selected):
    if (sq_selected != ()):
        [r, c] = sq_selected
        if (gs.board[r][c][0] == ('w' if gs.White_to_Move else 'b')):
            #highlight selected square
            s = p.Surface((SQ_SIZE,SQ_SIZE))
            s.set_alpha(100)    #transparency value --> 0 transparent; 255 opaque
            s.fill(p.Color('blue'))
            screen.blit(s, (c*SQ_SIZE,r*SQ_SIZE))
            #highlight moves from that square
            s.fill(p.Color('yellow'))
            for move in Valid_Moves:
                if ((move.Start_Row == r) and (move.Start_Col == c)):
                    screen.blit(s, (move.End_Col*SQ_SIZE, move.End_Row*SQ_SIZE))    

'''
Draw the move log
'''
def Draw_Move_Log(screen, gs, font):
    Move_Log_Rect = p.Rect(BOARD_WIDTH, 0, MOVE_LOG_PANEL_WIDTH, MOVE_LOG_PANEL_HEIGHT)
    p.draw.rect(screen, p.Color("black"), Move_Log_Rect)
    Move_Log = gs.Move_Log
    Move_Texts = []
    for i in range(0, len(Move_Log), 2):
        mv_str = str((i//2)+1) + ". " + str(Move_Log[i]) + " "
        if (i+1 < len(Move_Log)):
            mv_str = mv_str + str(Move_Log[i+1]) + " "
        Move_Texts.append(mv_str)
    moves_per_row = 2
    padding = 5
    line_spacing = 2
    text_y = int(padding)
    for i in range(0, len(Move_Texts), moves_per_row):
        text_str = ""
        for j in range(moves_per_row):
            if ((i+j) < len(Move_Texts)):
                text_str = text_str + Move_Texts[i+j]
        text_object = font.render(text_str, True, p.Color("white"))
        text_loc = Move_Log_Rect.move(padding,text_y)
        text_y += text_object.get_height() + line_spacing
        screen.blit(text_object, text_loc)

'''
Responsible for all the graphics within the current game state
'''

def Draw_Game_State(screen, gs, Valid_Moves, sq_selected, Move_Log_Font):
    Draw_Board(screen)   #draw squares on the board
    Highlight_Sqs(screen, gs, Valid_Moves, sq_selected)
    Draw_Pieces(screen, gs.board)    #draw pieces on top of those squares
    Draw_Move_Log(screen, gs, Move_Log_Font)
    
'''
Move animation function
'''
def Animate_Move(move,screen, board, clock):
    global colors
    #coords = []    #list of coords that the animation will move through
    dR = move.End_Row - move.Start_Row
    dC = move.End_Col - move.Start_Col
    Frames_Per_Square = 10    #Frames to move one square
    Frame_Count = (abs(dR)+abs(dC))*Frames_Per_Square
    for frame in range(Frame_Count+1):
        [r,c] = (move.Start_Row + dR*(frame/Frame_Count)),(move.Start_Col + dC*(frame/Frame_Count))
        Draw_Board(screen)
        Draw_Pieces(screen,board)
        #Erase the piece moved from its ending square
        color = colors[(move.End_Row + move.End_Col) % 2]
        End_Square = p.Rect((move.End_Col*SQ_SIZE), (move.End_Row*SQ_SIZE), SQ_SIZE, SQ_SIZE)
        p.draw.rect(screen, color, End_Square)
        #Draw the captured piece onto the rectangle
        if (move.Piece_Captured != "--"):
            screen.blit(IMAGES[move.Piece_Captured],End_Square)
        #Draw moving piece
        screen.blit(IMAGES[move.Piece_Moved], p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))
        p.display.flip()
        clock.tick(60)

'''
Draw text for end of game scenario
'''
def Draw_End_Game_Text(screen,text_str):
    font = p.font.SysFont("Helvitca", 32, True, False)
    text_object_secondary = font.render(text_str, 0, p.Color("Red"))
    text_loc = p.Rect(0,0,BOARD_WIDTH,BOARD_HEIGHT).move((BOARD_WIDTH/2)-(text_object_secondary.get_width()/2),(BOARD_HEIGHT/2)-(text_object_secondary.get_height()/2))
    screen.blit(text_object_secondary, text_loc)
    text_object_primary = font.render(text_str, 0, p.Color("Black"))
    screen.blit(text_object_primary,text_loc.move(2,2))

'''
The main driver for our code. This will handle user input and updating the graphics
'''
def main():
    p.init()
    screen = p.display.set_mode((BOARD_WIDTH + MOVE_LOG_PANEL_WIDTH,BOARD_HEIGHT))
    clock = p.time.Clock()
    screen.fill(p.Color("white"))
    Move_Log_Font = p.font.SysFont("Arial", 12, False, False)
    gs = Game_State_Classes.Game_State()
    Valid_Moves = gs.Get_Valid_Moves()
    Move_Made = False    #Flag variable for when a move is made
    Animate_flg = False    #Flag variable for when we should animate a move
    loadImages()    #Only do this once, before the while loop
    running = True
    sq_selected = ()    #No square selected initially, keep track of the last click of the user (tuple: (row, col))
    Player_Clicks = []  #Keep track of player clicks (two tuples: [(6,4), (4,4)])
    Game_Over = False
    Player_one = True    #If a human is playing white, then this will be true. If an AI is playing, then this will be False
    Player_two = False    #Same as above but for black
    while running:
        human_turn = (gs.White_to_Move and Player_one) or (not gs.White_to_Move and Player_two)
        for e in p.event.get():
            if (e.type == p.QUIT):
                running = False
            #Mouse handler
            elif (e.type == p.MOUSEBUTTONDOWN):
                if ((not Game_Over) and human_turn):
                    location = p.mouse.get_pos()    #(x,y) location of mouse
                    col = location[0]//SQ_SIZE
                    row = location[1]//SQ_SIZE
                    if ((sq_selected == (row, col)) or (col >= 8)):
                        sq_selected = ()    #deselect
                        Player_Clicks = []  #reset player clicks
                    else:
                        sq_selected = (row,col)
                        Player_Clicks.append(sq_selected)   #append for both first and second clicks
                    if (len(Player_Clicks) == 2):
                        #After the second click
                        move = Game_State_Classes.Move(Player_Clicks[0],Player_Clicks[1],gs.board)
                        for i in range(len(Valid_Moves)):
                            if (move == Valid_Moves[i]):
                                gs.Make_Move(Valid_Moves[i])
                                Move_Made = True
                                Animate_flg = True
                                #reset user clicks
                                sq_selected = ()
                                Player_Clicks = []
                        if (not Move_Made):
                            Player_Clicks = [sq_selected]
            #Key handlers
            elif (e.type == p.KEYDOWN):
                if (e.key == p.K_z):  #Undo when 'z' is pressed
                    print("Move undone")
                    print()
                    gs.Undo_Move()
                    Move_Made = True
                    Animate_flg = False
                    Game_Over = False
                elif (e.key == p.K_n):
                    print("Board reset")
                    print()
                    gs = Game_State_Classes.Game_State()
                    Valid_Moves = gs.Get_Valid_Moves()
                    sq_selected = ()
                    Player_Clicks = []
                    Move_Made = False
                    Animate_flg = False
                    Game_Over = False
        if ((not Game_Over) and (not human_turn) and (not gs.White_to_Move)):
            AI_move = AI_engine_2.Best_Move(gs, Valid_Moves)
            gs.Make_Move(AI_move)
            Move_Made = True
            Animate_flg = True
        elif ((not Game_Over) and (not human_turn) and (gs.White_to_Move)):
            AI_move = AI_engine_1.Best_Move(gs, Valid_Moves)
            gs.Make_Move(AI_move)
            Move_Made = True
            Animate_flg = True
        if (Move_Made):
            if (Animate_flg):
                Animate_Move(gs.Move_Log[-1],screen,gs.board,clock)
            Valid_Moves = gs.Get_Valid_Moves()
            Move_Made = False
            if ((gs.Checkmate) and (gs.White_to_Move)):
                Game_Over = True
                gm_ovr_txt = "Black wins by Checkmate (0-1)"
                print("Checkmate, Black wins (0-1)")
            elif ((gs.Checkmate) and (not gs.White_to_Move)):
                Game_Over = True
                gm_ovr_txt = "White wins by Checkmate (1-0)"
                print("Checkmate, White wins (1-0)")
            elif ((gs.Stalemate) and (not gs.Checkmate)):
                #Check if it is special draw
                special_draw = ' Stalemate '
                if (gs.Three_Move_Rep):
                    special_draw = ' Three Fold Repetition '
                elif (gs.Dead_Position):
                    special_draw = ' Dead Position '
                elif (gs.Fifty_Move_Rule):
                    special_draw = ' Fifty Move Rule '
                Game_Over = True
                gm_ovr_txt = "Draw by" + special_draw + "(0.5-0.5)"
                print("Stalemate, draw (0.5-0.5)")
        Draw_Game_State(screen, gs, Valid_Moves, sq_selected, Move_Log_Font)
        if (Game_Over):
            Draw_End_Game_Text(screen,gm_ovr_txt)
        clock.tick(MAX_FPS)
        p.display.flip()
    
if __name__ == "__main__":
    main()