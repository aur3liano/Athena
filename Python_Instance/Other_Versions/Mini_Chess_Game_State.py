# -*- coding: utf-8 -*-
"""
Created on Sun Jul 25 16:49:56 2021

Chess classes

This code was essentially copied with small modifications made to:
    1. Variable names
    2. Parentheses
    3. Line order
    4. Class attributes

and large modifications made to:
    1. Specific methods (subroutines)
    2. Algorithm for "If King is in check"
    3. En Passant
    4. Pawn promotion
    5. Castling

and with additions made to:
    1. Draw conditions

from a youtube.com video series by Eddie Sharick

@author: Justin Paul
"""

class Game_State():
    def __init__(self):
        #board is an 8x8 2d list, each element of the list has 2 characters.
        #The first character represents the color of the piece, 'b' or 'w'
        #The second character represents the type of the pieces, 'K', 'Q', 'R', 'B', 'K' or 'P'
        #'--' represents an empty space with no piece.
        self.board = [
                ["bR","bN","bQ","bK","bB","bR"],
                ["bP","bP","bP","bP","bP","bP"],
                ["--","--","--","--","--","--"],
                ["--","--","--","--","--","--"],
                ["wP","wP","wP","wP","wP","wP"],
                ["wR","wN","wQ","wK","wB","wR"]]
        self.Move_Functions = {"P": self.Get_Pawn_Moves,
                               "N": self.Get_Knight_Moves,
                               "B": self.Get_Bishop_Moves,
                               "R": self.Get_Rook_Moves,
                               "Q": self.Get_Queen_Moves}
        self.White_to_Move = True
        self.Checkmate = False
        self.Stalemate = False
        self.Three_Move_Rep = False
        self.Dead_Position = False
        self.Fifty_Move_Rule = False
        self.ALL_BOARDS = []
        self.Move_Log = []
        self.White_King_Loc = (5,4)
        self.Black_King_Loc = (0,4)
        self.In_Check = False
    
    '''
    Takes a Move as a parameter and executes it (this will not work for castling, pawn promotion, and en-passant)
    '''
    def Make_Move(self, move):
        #Update King location and check for castling
        if (move.Piece_Moved == "wK"):
            self.White_King_Loc = (move.End_Row,move.End_Col)
            if ((move.Start_Row == 5) and (move.Start_Col == 3) and (move.End_Row == 5) and (move.End_Col == 5)):
                #Short castling so move the King side rook
                self.board[5][5] = "--"
                self.board[5][3] = "wR"
            elif ((move.Start_Row == 5) and (move.Start_Col == 3) and (move.End_Row == 5) and (move.End_Col == 1)):
                #Short castling so move the King side rook
                self.board[5][0] = "--"
                self.board[5][2] = "wR" 
        elif (move.Piece_Moved == "bK"):
            self.Black_King_Loc = (move.End_Row,move.End_Col)
            if ((move.Start_Row == 0) and (move.Start_Col == 3) and (move.End_Row == 0) and (move.End_Col == 5)):
                #Short castling so move the King side rook
                self.board[0][5] = "--"
                self.board[0][3] = "bR"
            elif ((move.Start_Row == 0) and (move.Start_Col == 3) and (move.End_Row == 0) and (move.End_Col == 1)):
                #Short castling so move the King side rook
                self.board[0][0] = "--"
                self.board[0][2] = "bR"
        elif (move.is_en_passant):
            #Remove the pawn that was captured en passant
            self.board[move.Start_Row][move.End_Col] = "--"
        self.board[move.Start_Row][move.Start_Col] = "--"
        self.board[move.End_Row][move.End_Col] = move.Piece_Moved
        self.Move_Log.append(move)   #Log the move for castling and undoing
        self.White_to_Move = not self.White_to_Move    #swap players
        #Pawn promotion
        if (move.Pawn_Promote):
            self.board[move.End_Row][move.End_Col] = move.Piece_Moved[0] + "Q"
        new_board = []
        for b in range(len(self.board)):
            new_row = list(self.board[b])
            new_board.append(new_row)
        self.ALL_BOARDS.append(new_board)
            
    '''
    Undo the last move made
    '''
    def Undo_Move(self):
        if (len(self.Move_Log) != 0):
            board = self.ALL_BOARDS.pop()
            move = self.Move_Log.pop()
            if (move.is_castl_move):
                if (self.White_to_Move):
                    ally_color = "b"
                else:
                    ally_color = "w"
                if (move.End_Col == 4):
                    #Move the short rook back to the original square
                    self.board[move.Start_Row][4] = "--"
                    self.board[move.Start_Row][5] = ally_color + "R"
                elif (move.End_Col == 2):
                    self.board[move.Start_Row][3] = "--"
                    self.board[move.Start_Row][0] = ally_color + "R"
            elif (move.is_en_passant):
                if (self.White_to_Move):
                    oppo_color = "w"
                else:
                    oppo_color = "b"
                #Replace the move captured en passant
                self.board[move.Start_Row][move.End_Col] = oppo_color + "P"
            self.board[move.Start_Row][move.Start_Col] = move.Piece_Moved
            self.board[move.End_Row][move.End_Col] = move.Piece_Captured
            self.White_to_Move = not self.White_to_Move    #Switch turns back
            #Update the kings position if needed
            if (move.Piece_Moved == "wK"):
                self.White_King_Loc = (move.Start_Row,move.Start_Col)
            elif (move.Piece_Moved == "bK"):
                self.Black_King_Loc = (move.Start_Row,move.Start_Col)
    
    '''
    Get all the pawn moves for the pawn located at row, col and add these moves to the list
    '''
    def Get_Pawn_Moves(self, r, c, moves):
        En_passant_left = False
        En_passant_right = False
        if (len(self.Move_Log) != 0):
            prev_move = self.Move_Log[-1]
            #Check if the previous move was a two square advance
            #Check if the current pawn is on the same row
            #Check if the current pawn is in an adjacent column
            if ((prev_move.is_two_sq_adv) and (r == prev_move.End_Row) and (c == (prev_move.End_Col-1))):
                En_passant_right = True
            elif ((prev_move.is_two_sq_adv) and (r == prev_move.End_Row) and (c == (prev_move.End_Col+1))):
                En_passant_left = True
        if (self.White_to_Move):
            oppo_color = "b"
            pawn_rnk_str = 4
            pawn_adv_one = -1
            pawn_adv_two = -2
        else:
            oppo_color = "w"
            pawn_rnk_str = 1
            pawn_adv_one = 1
            pawn_adv_two = 2
        #1 square advance
        if (self.board[r+pawn_adv_one][c] == "--"):
            moves.append(Move((r,c),(r+pawn_adv_one,c),self.board))
            #2 square advance
            if ((r == pawn_rnk_str) and (self.board[r+pawn_adv_two][c] == "--")):
                moves.append(Move((r,c),(r+pawn_adv_two,c),self.board,is_castl_move = False,is_two_sq_adv = True))
        #Captues to the left
        if ((c-1) >= 0):
            #Pawn can attack enemy piece
            if (self.board[r+pawn_adv_one][c-1][0] == oppo_color):
                moves.append(Move((r,c),(r+pawn_adv_one,c-1),self.board))
            elif (En_passant_left):
                moves.append(Move((r,c),(r+pawn_adv_one,c-1),self.board,is_castl_move = False,is_two_sq_adv = False,is_en_passant = True))
        #Captures to the right
        if ((c+1) <= 5):
            #Pawn can attack enemy piece
            if (self.board[r+pawn_adv_one][c+1][0] == oppo_color):
                moves.append(Move((r,c),(r+pawn_adv_one,c+1),self.board))
            elif (En_passant_right):
                moves.append(Move((r,c),(r+pawn_adv_one,c+1),self.board,is_castl_move = False,is_two_sq_adv = False,is_en_passant = True))
        return(moves)
    
    '''
    Get all the knight moves for the knight located at row, col and add these moves to the list
    The moves are denoted as follows (r,c)
    (-,-) 1   8 (-,+)
        2       7
            N
        3       6
    (+,-) 4   5 (+,+)
    '''
    def Get_Knight_Moves(self, r, c, moves):
        nite_coor = ((-2,-1),(-1,-2),(1,-2),(2,-1),
                       (2,1),(1,2),(-1,2),(-2,1))
        ally_color = "w" if self.White_to_Move else "b"
        for nc in nite_coor:
            End_Row = r + nc[0]
            End_Col = c + nc[1]
            if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                End_Piece = self.board[End_Row][End_Col]
                if (End_Piece[0] != ally_color):
                    moves.append(Move((r,c),(End_Row,End_Col),self.board))
        return(moves)
    
    '''
    Get all the bishop moves for the bishop located at row, col and add these moves to the list
    The moves are denoted as follows (r,c)
    (-,-)     (-,+)
           B
    (+,-)     (+,+)
    '''
    def Get_Bishop_Moves(self, r, c, moves):
        directions = ((-1,-1),(1,-1),(1,1),(-1,1))
        oppo_color = "b" if self.White_to_Move else "w"
        for d in directions:
            for i in range(1,6):
                End_Row = r + d[0] * i
                End_Col = c + d[1] * i
                if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                    End_Piece = self.board[End_Row][End_Col]
                    if (End_Piece == "--"):
                        moves.append(Move((r,c),(End_Row,End_Col),self.board))
                    elif (End_Piece[0] == oppo_color):
                        moves.append(Move((r,c),(End_Row,End_Col),self.board))
                        break
                    else:
                        break
                else:
                    break
        return(moves)
    
    '''
    Get all the rook moves for the rook located at row, col and add these moves to the list
    The moves are denoted as follows (r,c)
        (-,0)
    (0,-) R (0,+)
        (+,0)
    '''
    def Get_Rook_Moves(self, r, c, moves):
        directions = ((-1,0),(0,-1),(1,0),(0,1))
        oppo_color = "b" if self.White_to_Move else "w"
        for d in directions:
            for i in range(1,6):
                End_Row = r + d[0] * i
                End_Col = c + d[1] * i
                if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                    End_Piece = self.board[End_Row][End_Col]
                    if (End_Piece == "--"):
                        moves.append(Move((r,c),(End_Row,End_Col),self.board))
                    elif (End_Piece[0] == oppo_color):
                        moves.append(Move((r,c),(End_Row,End_Col),self.board))
                        break
                    else:
                        break
                else:
                    break
        return(moves)
    
    '''
    Get all the queen moves for the queen located at row, col and add these moves to the list
    '''
    def Get_Queen_Moves(self, r, c, moves):
        self.Get_Rook_Moves(r,c,moves)
        self.Get_Bishop_Moves(r,c,moves)
        return(moves)
    
    '''
    All moves without considering checks
    '''
    def Get_All_Possible_Moves(self):
        moves = []
        #Number of rows
        for r in range(len(self.board)):
            #Number of columns in given row
            for c in range(len(self.board[r])):
                turn = self.board[r][c][0]
                if ((turn == "w" and self.White_to_Move) or (turn == "b" and not self.White_to_Move)):
                    piece = self.board[r][c][1]
                    if (piece == "K"):
                        continue
                    else:
                        #Call the appropriate move function for the piece type
                        self.Move_Functions[piece](r,c,moves)
        return(moves)
    
    '''
    Look for knights delivering check to the King
    '''
    def Looking_for_knights(self,King_Row,King_Col):
        nite_checks = []
        nite_coor = ((-2,-1),(-1,-2),(1,-2),(2,-1),
                     (2,1),(1,2),(-1,2),(-2,1))
        ally_color = "w" if self.White_to_Move else "b"
        for nc in nite_coor:
            End_Row = King_Row + nc[0]
            End_Col = King_Col + nc[1]
            if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                End_Piece = self.board[End_Row][End_Col]
                if ((End_Piece[0] != ally_color) and (End_Piece[1] == "N")):
                    nite_checks.append((End_Row, End_Col))
        return(nite_checks)

    '''
    Look for pawns delivering check to the King
    '''
    def Looking_for_pawns(self,King_Row,King_Col):
        #Check for pawns in the appropriate squares
        oppo_color = "b" if self.White_to_Move else "w"
        pwn_checks = []
        pwn_chk_col_1 = King_Col-1
        pwn_chk_col_2 = King_Col+1
        if (self.White_to_Move):
            pwn_chk_row = King_Row-1
        else:
            pwn_chk_row = King_Row+1
        if ((0 <= pwn_chk_row <= 5) and (0 <= pwn_chk_col_1 <= 5)):
            End_Piece_1 = self.board[pwn_chk_row][pwn_chk_col_1]
            if ((End_Piece_1[0] == oppo_color) and (End_Piece_1[1] == "P")):
                pwn_checks.append((pwn_chk_row,pwn_chk_col_1))
        if ((0 <= pwn_chk_row <= 5) and (0 <= pwn_chk_col_2 <= 5)):
            End_Piece_2 = self.board[pwn_chk_row][pwn_chk_col_2]
            if ((End_Piece_2[0] == oppo_color) and (End_Piece_2[1] == "P")):
                pwn_checks.append((pwn_chk_row,pwn_chk_col_2))
        return(pwn_checks)
    
    '''
    Look for other pieces delivering check to the King: Q,R,B
    '''
    def Raycasting(self,King_Row,King_Col,Verbose_flg = False):
        #Initialize ray checks and possible pins, set the opponent color and initialize raycasting directions
        ray_checks = []
        if (self.White_to_Move):
            oppo_color = "b"
            ally_color = "w"
        else:
            oppo_color = "w"
            ally_color = "b"
        bishop_dngr_sqs = [[],[],[],[]]
        rook_dngr_sqs = [[],[],[],[]]
        bishop_dirs = ((-1,-1),(1,-1),(1,1),(-1,1))
        rook_dirs = ((-1,0),(0,-1),(1,0),(0,1))
        #Look for bishops and queens
        for d in range(len(bishop_dirs)):
            for i in range(1,6):
                End_Row = King_Row + bishop_dirs[d][0] * i
                End_Col = King_Col + bishop_dirs[d][1] * i
                if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                    End_Piece = self.board[End_Row][End_Col]
                    if ((End_Piece == "--") or (End_Piece == (ally_color + "K"))):
                        bishop_dngr_sqs[d].append((End_Row,End_Col))
                        continue
                    elif ((End_Piece[0] == oppo_color) and ((End_Piece[1] == "B") or (End_Piece[1] == "Q"))):
                        ray_checks.append((End_Row, End_Col))
                        bishop_dngr_sqs[d].append((End_Row,End_Col))
                        break
                    elif ((End_Piece[0] == oppo_color) and ((End_Piece[1] != "B") and (End_Piece[1] != "Q"))):
                        bishop_dngr_sqs[d] = []
                        break
                    elif (End_Piece[0] != oppo_color):
                        bishop_dngr_sqs[d] = []
                        break
                else:
                    break

        #Look for rooks and queens
        for d in range(len(rook_dirs)):
            for i in range(1,6):
                End_Row = King_Row + rook_dirs[d][0] * i
                End_Col = King_Col + rook_dirs[d][1] * i
                if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                    End_Piece = self.board[End_Row][End_Col]
                    if ((End_Piece == "--") or (End_Piece == (ally_color + "K"))):
                        rook_dngr_sqs[d].append((End_Row,End_Col))
                        continue
                    elif ((End_Piece[0] == oppo_color) and ((End_Piece[1] == "R") or (End_Piece[1] == "Q"))):
                        ray_checks.append((End_Row, End_Col))
                        rook_dngr_sqs[d].append((End_Row,End_Col))
                        break
                    elif ((End_Piece[0] == oppo_color) and ((End_Piece[1] != "R") and (End_Piece[1] != "Q"))):
                        rook_dngr_sqs[d] = []
                        break
                    elif (End_Piece[0] != oppo_color):
                        rook_dngr_sqs[d] = []
                        break
                else:
                    break
        #Consolidate danger squares to a single list
        dngr_sqs = []
        for bds in bishop_dngr_sqs:
            dngr_sqs += bds
        for rds in rook_dngr_sqs:
            dngr_sqs += rds
        return(ray_checks,dngr_sqs)
    
    '''
    Look for the opponent King delivering check to the King
    '''
    def Looking_for_King(self,King_Row,King_Col):
        king_checks = []
        king_coor = ((-1,0),(0,-1),(1,0),(0,1),
                     (-1,-1),(1,-1),(1,1),(-1,1))
        ally_color = "w" if self.White_to_Move else "b"
        for kc in king_coor:
            End_Row = King_Row + kc[0]
            End_Col = King_Col + kc[1]
            if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                End_Piece = self.board[End_Row][End_Col]
                if ((End_Piece[0] != ally_color) and (End_Piece[1] == "K")):
                    king_checks.append((End_Row, End_Col))
        return(king_checks)
    
    '''
    Validate castling moves
    '''
    def Castle_Validation(self,King_Row,King_Col,Castling_Col_Info,Lng_Cas_flg):
        Castl_flg = True
        King_Col_needed = 4
        Rook_Col_needed = Castling_Col_Info[0]
        Potent_sq_Col_1 = Castling_Col_Info[1]
        Potent_sq_Col_2 = Castling_Col_Info[2]
        if (King_Col == King_Col_needed):
            if (self.White_to_Move):
                ally_color = "w"
                King_Row_needed = 7
            else:
                ally_color = "b"
                King_Row_needed = 0
            if ((King_Row == King_Row_needed) and (self.board[King_Row_needed][Rook_Col_needed] == (ally_color + "R"))):
                #Look for king movement invalidating castle
                for ml in range(len(self.Move_Log)):
                    move_id_chk = str(self.Move_Log[ml].Move_ID)
                    if (move_id_chk[0:2] == (str(King_Row) + str(King_Col_needed))):
                        #King has moved
                        return(False)
                #Look for rook movement invalidating castle
                for ml in range(len(self.Move_Log)):
                    move_id_chk = str(self.Move_Log[ml].Move_ID)
                    if (move_id_chk[0:2] == (str(King_Row) + str(Rook_Col_needed))):
                        #Rook has moved
                        return(False)
                #Check if the rook has been captured
                for ml in range(len(self.Move_Log)):
                    move_id_chk = str(self.Move_Log[ml].Move_ID)
                    if (move_id_chk[2:] == (str(King_Row) + str(Rook_Col_needed))):
                        #Rook has been captured
                        return(False)
                #King and rook are positioned correctly and have not moved so check if the King is currently in check
                #Note current check does not require looking for the enemy king
                if (self.In_Check):
                    #King is currently in check
                    return(False)
                #Make sure King crossing squares are empty
                if ((self.board[King_Row][Potent_sq_Col_1] != "--") or (self.board[King_Row][Potent_sq_Col_2] != "--")):
                    return(False)
                if ((Lng_Cas_flg == True) and (self.board[King_Row][1] != "--")):
                    return(False)
                #King crossing squares are empty so check King crossing squares for check
                #Note that potential check requires looking for the enemy king
                king_checks_1 = self.Looking_for_King(King_Row,Potent_sq_Col_1)
                nite_checks_1 = self.Looking_for_knights(King_Row,Potent_sq_Col_1)
                pawn_checks_1 = self.Looking_for_pawns(King_Row,Potent_sq_Col_1)
                [ray_checks_1,dngr_sqs] = self.Raycasting(King_Row,Potent_sq_Col_1)
                if ((king_checks_1 != []) or (nite_checks_1 != []) or (pawn_checks_1 != []) or (ray_checks_1 != [])):
                    #King would be in check on King crossing square 1
                    return(False)
                king_checks_2 = self.Looking_for_King(King_Row,Potent_sq_Col_2)
                nite_checks_2 = self.Looking_for_knights(King_Row,Potent_sq_Col_2)
                pawn_checks_2 = self.Looking_for_pawns(King_Row,Potent_sq_Col_2)
                [ray_checks_2,dngr_sqs] = self.Raycasting(King_Row,Potent_sq_Col_2)
                if ((king_checks_2 != []) or (nite_checks_2 != []) or (pawn_checks_2 != []) or (ray_checks_2 != [])):
                    #King would be in check on King crossing square 2
                    return(False)
            else:
                #King is not in appropriate row or Rook is not in appropriate position (or both)
                return(False)
        else:
            #King is not in appropriate column
            return(False)
        return(Castl_flg)
    
    '''
    Get all the king moves for the king located at row, col and add these moves to the list
    First check standard moves are denoted as follows (r,c):
    (-,-) 2 1 8 (-,+)
          3 K 7
    (+,-) 4 5 6 (+,+)
    
    added "King check must be resolved"
    added "you can not move into squares that put you in check"
    added castling and castling technicalities
        Phase 1
            Check if King is in the right column
        Phase 2
            Check if King is in the right row
            Check if corresponding rook is in the right position
        Phase 3
            Check if the king has moved
            Check if the corresponding rook has moved
            Check if the corresponding rook has been captured
        Phase 4
            Check if the King is currently in check
            Check if the squares between the King and the rook are empty
        Phase 5
            Check if the King crossing squares put the King in check
    '''
    def Get_King_Moves(self,r,c,moves):
        king_coor = ((-1,0),(-1,-1),(0,-1),(1,-1),
                     (1,0),(1,1),(0,1),(-1,1))
        if (self.White_to_Move):
            ally_color = "w"
            castl_row = 5
        else:
            ally_color = "b"
            castl_row = 0
        for kc in range(8):
            End_Row = r + king_coor[kc][0]
            End_Col = c + king_coor[kc][1]
            if ((0 <= End_Row <= 5) and (0 <= End_Col <= 5)):
                End_Piece = self.board[End_Row][End_Col]
                if (End_Piece[0] != ally_color):
                    #look for piece checks on next move
                    king_checks = self.Looking_for_King(End_Row,End_Col)
                    nite_checks = self.Looking_for_knights(End_Row,End_Col)
                    pawn_checks = self.Looking_for_pawns(End_Row,End_Col)
                    [ray_checks,dngr_sqs] = self.Raycasting(End_Row,End_Col)
                    if ((king_checks == []) and (nite_checks == []) and (pawn_checks == []) and (ray_checks == [])):
                        moves.append(Move((r,c),(End_Row,End_Col),self.board))
        Shrt_Castling_Info = [5,4,5]
        Lng_Cas_flg = False
        Shrt_Castle_flg = self.Castle_Validation(r,c,Shrt_Castling_Info,Lng_Cas_flg)
        if (Shrt_Castle_flg == True):
            moves.append(Move((r,c),(castl_row,5),self.board,is_castl_move = True))
        Long_Castling_Info = [0,2,1]
        Lng_Cas_flg = True
        Long_Castle_flg = self.Castle_Validation(r,c,Long_Castling_Info,Lng_Cas_flg)
        if (Long_Castle_flg == True):
            moves.append(Move((r,c),(castl_row,1),self.board,is_castl_move = True))
        return(moves)
        
    '''
    Draw check for dead positions
    '''
    def Dead_Position_Check(self):
        #Collect the pieces for both teams
        white = []
        black = []
        for row in self.board:
            for square in row:
                if (square[0] == "w"):
                    white.append(square[1])
                elif (square[0] == "b"):
                    black.append(square[1])
        if (
            (white.count("Q") == white.count("R") == white.count("B") == white.count("N") == white.count("P") == 0)
            and (black.count("Q") == black.count("R") == black.count("B") == black.count("N") == black.count("P") == 0)
        ):
            #White King vs Black King exclusively
            self.Dead_Position = True
            return(True)
        elif (
            (white.count("Q") == white.count("R") == white.count("B") == white.count("P") == 0)
            and (black.count("Q") == black.count("R") == black.count("B") == black.count("N") == black.count("P") == 0)
            and (white.count("N") == 1)
        ):
            #White King and Knight vs Black King exclusively
            self.Dead_Position = True
            return(True)
        elif (
            (white.count("Q") == white.count("R") == white.count("B") == white.count("N") == white.count("P") == 0)
            and (black.count("Q") == black.count("R") == black.count("B") == black.count("P") == 0)
            and (black.count("N") == 1)
        ):
            #Black King and Knight vs White King exclusively
            self.Dead_Position = True
            return(True)
        elif (
            (white.count("Q") == white.count("R") == white.count("N") == white.count("P") == 0)
            and (black.count("Q") == black.count("R") == black.count("B") == black.count("N") == black.count("P") == 0)
            and (white.count("B") == 1)
        ):
            #White King and Bishop vs Black King exclusively
            self.Dead_Position = True
            return(True)
        elif (
            (white.count("Q") == white.count("R") == white.count("B") == white.count("N") == white.count("P") == 0)
            and (black.count("Q") == black.count("R") == black.count("N") == black.count("P") == 0)
            and (black.count("B") == 1)
        ):
            #White King and Bishop vs Black King exclusively
            self.Dead_Position = True
            return(True)
        else:
            self.Fifty_Move_Rule = False
            return(False)
        '''
        elif ():
            #King and Bishop vs King and Bishop with both bishops on the same color
            self.Dead_Position = True
        '''
    
    '''
    Draw check for three fold repetition
    '''
    def Three_Fold_Rep_Check(self):
        max_count = 0
        for bds in self.ALL_BOARDS:
            bds_count = self.ALL_BOARDS.count(bds)
            if (bds_count > max_count):
                max_count = bds_count
        if (max_count > 2):
            self.Three_Move_Rep = True
            return(True)
        else:
            self.Three_Move_Rep = False
            return(False)
    
    '''
    Draw check for 50 move rule
    '''
    def Fifty_Move_Rule_Check(self):
        #Progress through the move list backwards
        move_count = 0
        for mv in range(len(self.Move_Log)-1,-1,-1):
            curr_move = self.Move_Log[mv]
            piece_mved = curr_move.Piece_Moved
            piece_capt = curr_move.Piece_Captured
            if ((piece_mved[1] != "P") or (piece_capt[1] == "--")):
                #Continue the count if no piece is captured or if the piece moved isnt a pawn
                move_count += 1
            else:
                break
        if (move_count > 49):
            self.Fifty_Move_Rule = True
            return(True)
        else:
            self.Fifty_Move_Rule = False
            return(False)
    
    '''
    All moves considering checks
    '''
    def Get_Valid_Moves(self):
        #Establish King location
        self.In_Check = False
        ray_checking = False
        nite_checking = False
        pawn_checking = False
        moves = self.Get_All_Possible_Moves()
        if (self.White_to_Move):
            King_Row = self.White_King_Loc[0]
            King_Col = self.White_King_Loc[1]
        else:
            King_Row = self.Black_King_Loc[0]
            King_Col = self.Black_King_Loc[1]
        #1. Complete raycasting to look for checks against the King square
        #If there are more than two checks, the king has to move
        [ray_checks,dngr_sqs] = self.Raycasting(King_Row,King_Col)
        if ((ray_checks != []) and (len(ray_checks) < 2)):
            #Change check status
            ray_checking = True
            #Eliminate moves that can not capture the knight delivering Check
            for i in range(len(moves)-1,-1,-1):
                move_end_sq = (moves[i].End_Row,moves[i].End_Col)
                if (move_end_sq not in dngr_sqs):
                    moves.remove(moves[i])
        else:
            ray_checking = False
        #2. Look for knight checking on the King square to determine if the King needs to move
        nite_checks = self.Looking_for_knights(King_Row,King_Col)
        if (nite_checks != []):
            #Change check status
            nite_checking = True
            #Eliminate moves that can not capture the knight delivering Check
            for i in range(len(moves)-1,-1,-1):
                if ((moves[i].End_Row != nite_checks[0][0]) and (moves[i].End_Col != nite_checks[0][1])):
                    moves.remove(moves[i])
        else:
            nite_checking = False
        pawn_checks = self.Looking_for_pawns(King_Row,King_Col)
        #3. Look for pawn checking on the King square to determine if the King needs to move
        if (pawn_checks != []):
            #Change check status
            pawn_checking = True
            #Eliminate moves that can not capture the knight delivering Check
            for i in range(len(moves)-1,-1,-1):
                if ((moves[i].End_Row != pawn_checks[0][0]) and (moves[i].End_Col != pawn_checks[0][1])):
                    moves.remove(moves[i])
        else:
            pawn_checking = False
        if ((ray_checking == True) or (nite_checking == True) or (pawn_checking == True)):
            self.In_Check = True
        else:
            self.In_Check = False
        #4. Evaluate remaining moves for inducing raycasting checks (pins and En passant)    
        for i in range(len(moves)-1,-1,-1):
            #Make the move and switch players back to the original king
            self.Make_Move(moves[i])
            self.White_to_Move = not self.White_to_Move
            #Complete all of the check functions to see if the King is in check
            nite_checks = self.Looking_for_knights(King_Row,King_Col)
            pawn_checks = self.Looking_for_pawns(King_Row,King_Col)
            [ray_checks,dngr_sqs] = self.Raycasting(King_Row,King_Col)
            if ((ray_checks != []) or (nite_checks != []) or (pawn_checks != [])):
                #If the King is now in check remove the move
                moves.remove(moves[i])
            self.White_to_Move = not self.White_to_Move
            #Undo the move
            self.Undo_Move()
        #5. Look for what moves the King can make
        moves = self.Get_King_Moves(King_Row, King_Col, moves)
        #Initialize all stalemate flag scenarios
        thr_fld_rep_flg = False
        ded_pos_flg = False
        fifty_mv_rul_flg = False
        if (len(self.Move_Log) > 2):
            #Check three fold repetition
            thr_fld_rep_flg = self.Three_Fold_Rep_Check()
        if (len(self.Move_Log) > 19):
            #Check for dead positions
            ded_pos_flg = self.Dead_Position_Check()
        if (len(self.Move_Log) > 50):
            #Check for 50 move rule
            fifty_mv_rul_flg = self.Fifty_Move_Rule_Check()
        #Evaluate mate status of players
        if (len(moves) == 0):
            if (self.In_Check):
                self.Checkmate = True
            else:
                self.Stalemate = True
        elif (thr_fld_rep_flg):
            self.Stalemate = True
        elif (ded_pos_flg):
            self.Stalemate = True
        elif (fifty_mv_rul_flg):
            self.Stalemate = True
        else:
            self.Checkmate = False
            self.Stalemate = False
        return(moves)

class Move():
    #maps keys to values
    #key : value
    Ranks_to_Rows = {"1": 5,"2": 4,"3": 3,
                     "4": 2,"5": 1,"6": 0}
    Rows_to_Ranks = {v: k for k, v in Ranks_to_Rows.items()}
    Files_to_Cols = {"a": 0,"b": 1,"c": 2,
                     "d": 3,"e": 4,"f": 5}
    Cols_to_Files = {v: k for k, v in Files_to_Cols.items()}
    
    
    def __init__(self,Start_Sq, End_Sq,board, is_castl_move = False, is_two_sq_adv = False, is_en_passant = False):
        self.is_castl_move = is_castl_move
        self.is_two_sq_adv = is_two_sq_adv
        self.is_en_passant = is_en_passant
        self.Start_Row = Start_Sq[0]
        self.Start_Col = Start_Sq[1]
        self.End_Row = End_Sq[0]
        self.End_Col = End_Sq[1]
        self.Piece_Moved = board[self.Start_Row][self.Start_Col]
        self.Piece_Captured = board[self.End_Row][self.End_Col]
        #Pawn_promote
        self.Pawn_Promote = False
        if ((self.Piece_Moved == "wP") and (self.End_Row == 0)) or ((self.Piece_Moved == "bP") and (self.End_Row == 5)):
            self.Pawn_Promote = True
        self.Move_ID = (self.Start_Row * 1000) + (self.Start_Col * 100) + (self.End_Row * 10) + self.End_Col
    
    '''
    Overriding the equals method
    '''
    def __eq__(self, other):
        if isinstance(other, Move):
            return(self.Move_ID == other.Move_ID)
        return(False)
                
    def Get_Rank_File(self, r, c):
        return(self.Cols_to_Files[c] + self.Rows_to_Ranks[r])
    
    def Get_Chess_Notation(self):
        #Amend later to convert into algebraic notation
        return(self.Get_Rank_File(self.Start_Row,self.Start_Col) + self.Get_Rank_File(self.End_Row,self.End_Col))
    
    def __str__(self):
        #Using long algebraic notation
        if ((self.is_castl_move) and (self.End_Col == 5)):
            return("O-O")
        elif ((self.is_castl_move) and (self.End_Col == 1)):
            return("O-O-O")
        elif (self.Piece_Captured == "--"):
            if (self.Piece_Moved[1] == "P"):
                mv_str = self.Get_Chess_Notation()
            else:
                mv_str = self.Piece_Moved[1] + self.Get_Rank_File(self.Start_Row,self.Start_Col)
                mv_str = mv_str + "-" + self.Get_Rank_File(self.End_Row,self.End_Col)
        elif (self.Piece_Captured != "--"):
            if (self.Piece_Moved[1] == "P"):
                mv_str = self.Get_Rank_File(self.Start_Row,self.Start_Col) + "x"
                mv_str = mv_str + self.Get_Rank_File(self.End_Row,self.End_Col)
            else:
                mv_str = self.Piece_Moved[1] + self.Get_Rank_File(self.Start_Row,self.Start_Col)
                mv_str = mv_str + "x" + self.Get_Rank_File(self.End_Row,self.End_Col)
        return(mv_str)