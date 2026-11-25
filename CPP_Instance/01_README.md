# C++ Chess Engine  
	A modern, modular, testable C++ chess engine built entirely from scratch using CMake.  
	This engine implements a complete and correct representation of chess rules, including legal move generation, SAN/FEN support, and a CLI interface.

# Current Capabilities
	- Fully functional board representation
	- Complete legal move generator
	- SAN and FEN support
	- Game controller with checkmate/stalemate detection
	- CLI UI
	- Qt GUI
	- Unit testing with Catch2
	- Perft Validation
	- Expert Virtual Player (EVP) suite

✔ Fully Functional Board Representation
	8×8 piece array
	Proper initialization in starting position
	Complete FEN‑compatible game state
	Halfmove clock and fullmove number tracking

✔ Complete Legal Move Generator
	Pawns (push, capture, promotion, en passant)
	En passant square computation
	Promotion reversal
	Knights
	Bishops
	Rooks
	Queens
	Kings
	Pseudolegal → Legal filtering path
	Castling rights tracking
	Full reversible move system via MoveState snapshots
	Ensurance of perfect apply/undo symmetry
	Accurate attack detection
	Legal move validation through simulation (leavesKingInCheck)
	Draw conditions (dead position, 50 move rule, threefold repetition)

✔ Notation System
	Coordinate notation (e.g., "e2e4")
	SAN parsing (e.g., "Nf3", "exd5", "O‑O", "e8=Q")
	SAN generation with check/checkmate annotation
	SAN disambiguation (file, rank, or both)

✔ FEN Loader / Writer
	Fully standard FEN parsing
	Complete state restoration (pieces, castling, EP, clocks, ply counters)
	Round‑trip FEN export

✔ Game Controller
	Move application
	Game termination detection (checkmate, stalemate)
	Human vs. human mode via CLI
	Legal move validation

✔ CLI UI
	ASCII board representation
	Move input
	User messaging

✔ Qt GUI
	Lichess-esque Board Representation
	Legal Move highlighting
	Game result animation overlay

✔ Unit Testing (Catch2)
	Board initialization
	FEN load/save
	Move generation correctness for starting position
	Undo/redo behavior
	Coordinate/SAN conversion tests
	Castling
	En Passant

✔ Perft Validation
	Perft search with depth control  
	Optional perft divide (`--divide`)  
	SAN or coordinate output (`--san`)
	Node attribute statistics (`--stats`)
	FEN input support  
	Timing and nodes/sec reporting  
	Integrated with the engine’s apply/undo system

✔ Expert Virtual Player (EVP) suite
	Agent_01 - random mover (no eval, no search)															(already added)
	Agent_02 - material eval + minimax		 																(already added)
	Agent_03 - material eval + King Endgame Pressure + minimax + αβ											(already added)
	Agent_04 - eval + PST + KEP + MMwABP + move ordering													(already added)
	Agent_05 - eval (material + PST + king safety + KEP) + MMwABP + move ordering + ID						(not added yet)
	Agent_06 - eval (material + PST + king safety + KEP) + MMwABP + move ordering + ID						(not added yet)
	Agent_07 - eval (material + PST + mobility + king safety + KEP) + move ordering + MCTS					(not added yet)
	Agent_08 - eval (material + PST + mobility + king safety + KEP) + MMwABP + move ordering + MCTS			(not added yet)
	Agent_09 - eval (material + PST + mobility + king safety + KEP) + MMwABP + move ordering + MCTS + ID	(not added yet)
	Agent tournament systems

## Project Structure
Chess/
	│
	│   CMakeLists.txt
	│   01_README.md
	│   02_roadmap.md
	│
	├── include/
	│	│
	│   ├── engine/
	│	│	│
	│   │	│	board.h
	│   │	│   FEN.h
	│   │	│   game.h
	│   │	│   move_generator.h
	│   │	│   move_state.h
	│   │	└── notation.h
	│   │
	│   ├── tools/
	│	│	│
	│	│	│   perft.h
	│	│	└── legal_move_FEN_analysis.h
	│   │
	│   ├── tournament/
	│	│	│
	│	│	│	agent_tournament_v2.h
	│	│	└── agent_tournament.h
	│   │
	│   ├── agents/
	│	│	│
	│	│	│   Agent_01.h
	│	│	└── IChessAgent.h
	│   │
	│   └── ui/
	│		│
	│		│   cli_ui.h
	│		│	ui_interface.h
	│   	│
	│   	└── gui_qt/
	│       	│
	│       	├── adapters/
	│       	│	│
	│           │	└── engine_adapter.h
	│       	│
	│       	└── widgets/
	│			│	│
	│       	│	│	BoardWidget.h
	│       	│	│	SquareWidget.h (optional)
	│       	│	│	MoveListWidget.h (later)
	│       	│	│	CapturedPiecesWidget.h (later)
	│			│	└── ControlsPanelWidget.h (later)
	│       	│
	│       	└── windows/
	│       		│
	│				└── ChessWindow.h
	│
	├── src/
	│	│
	│	│	main.cpp
	│	│	perft_main.cpp
	│	│	legal_move_FEN_analysis_main.cpp
	│	│
	│	├── engine/
	│	│	│
	│   │   │	board.cpp
	│   │   │   FEN.cpp
	│   │   │   game.cpp
	│   │   │   move_generator.cpp
	│   │   └── notation.cpp
	│   │
	│   ├── tools/
	│	│	│
	│   │	│   perft.cpp
	│   │	└── legal_move_FEN_analysis.cpp
	│   │
	│   ├── tournament/
	│	│	│
	│	│	│	agent_tournament_v2.cpp
	│	│	└── agent_tournament.cpp
	│   │
	│   ├── agents/
	│	│	│
	│   │	└── Agent_01.cpp
	│   │
	│   └── ui/
	│		│
	│		│	CMakeLists.txt
	│   	│	cli_ui.cpp
	│   	│
	│   	└── gui_qt/
	│   		│
	│			│	qt_main.cpp
	│   		│
	│       	├── resources/
	│       	│	│
	│           │	├── pieces/
	│       	│	│	│
	│           │ 	│	└── (PNG/SVG piece images)
	│       	│	│
	│           │ 	└── board_theme/
	│       	│		│
	│			│   	└── (light.png, dark.png, highlight.png, etc.)
	│       	│
	│       	├── widgets/
	│       	│	│
	│       	│   └── BoardWidget.cpp
	│       	│
	│       	├── adapters/
	│       	│	│
	│           │	└── engine_adapter.cpp
	│			│
	│       	└── windows/
	│       		│
	│				└── ChessWindow.cpp
	│
	├── tests/
	│	│
	│	│   test_board.cpp
	│	│   test_castling.cpp
	│	│   test_en_passant.cpp
	│	│   test_fen.cpp
	│	│   test_isSquareAttacked.cpp
	│	│   test_king_safety.cpp
	│	│   test_movegen.cpp
	│	│   test_perft.cpp
	│	│   test_promotion.cpp
	│   │	test_undo.cpp
	│	│
	│   └── Catch2/
	│
	└── build/

### Dependency Diagram
							   +---------------+
							   |    board.h    |
							   +-------+-------+
                                       |
                                       v
                             +------------------+
                             |   move_state.h   |
                             +---------+--------+
                                       |
                                       v
                        +---------------------------+
                        |         board.cpp         |
                        |  (apply/undo, attacks)    |
                        +--------------+------------+
                                       |
                                       v
          -----------------------------------------------------------------
          |                	    |                   |                     |
          v              	    v                   v                     v
+--------------------+   +-------------+   +----------------+    +-----------------+
| move_generator.h   |   |    FEN.h    |   |  notation.h    |    |     game.h      |
+---------+----------+   +------+------+   +-------+--------+    +--------+--------+
          |                     |                  |                      |
          v                     v                  v                      v
+--------------------+   +-------------+   +----------------+    +-----------------+
| move_generator.cpp |   |   FEN.cpp   |   |  notation.cpp  |    |    game.cpp     |
+--------------------+   +-------------+   +----------------+    +-----------------+

					ENGINE TOOLS LAYER (ANALYSIS & DEBUGGING)
					-----------------------------------------
		+-----------------------+    	+-------------------------------+
		|      perft.h          |    	|  legal_move_FEN_analysis.h    |
		|  (Perft interface)    |    	|  (FEN debug interface)        |
		+----------+------------+    	+---------------+---------------+
				   |                                 	|
				   v                                 	v
		+-----------------------+    	+-------------------------------+
		|      perft.cpp        |    	| legal_move_FEN_analysis.cpp   |
		| (perft implementation)|    	| (FEN move analysis logic)     |
		+----------+------------+    	+---------------+---------------+
				   |                                 	|
				   v                                 	v
		+---------------------------+   +----------------------------------+
		|      perft_main.cpp       |   | legal_move_FEN_analysis_main.cpp |
		| (perft CLI tool driver)   |   |     (FEN analysis CLI tool)      |
		+---------------------------+   +----------------------------------+

							 UI ABSTRACTION LAYER
						     --------------------
                             +------------------+
                             |  ui_interface.h  |
                             |  (abstract UI)   |
                             +---------+--------+
                                       |
                                       v
                             +------------------+
                             |    cli_ui.h      |
                             |  CLI UI (header) |
                             +---------+--------+
                                       |
                                       v
                             +------------------+
                             |   cli_ui.cpp     |
                             |   CLI UI impl    |
                             +------------------+

                         APPLICATION LAYER (TOP LEVEL)
                         -----------------------------
                             +------------------+
                             |    main.cpp      |
                             |   (game CLI)     |
                             +--------+---------+

                      TESTS (depend on engine + Catch2)
                      ---------------------------------
     test_*.cpp  --->  board.h, move.h, move_generator.h, notation.h, FEN.h, game.h

### Windows Build Instructions
	```
	cd ../../
	rmdir -Recurse build
	mkdir build
	cd build
	cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Apps/Qt/6.11.1/msvc2022_64"
	cmake --build . --config Release
	cd Release
	mkdir platforms
	Copy-Item -Path "C:\Apps\Qt\6.11.1\msvc2022_64\plugins\platforms\qwindows.dll" -Destination "C:\Apps\code\CPP_Training\Chess\Chess_in_CPP_by_MS_Pilot\build\Release\platforms\qwindows.dll"
	Copy-Item -Path "C:\Apps\Qt\Tools\QtCreator\bin\Qt6Widgets.dll" -Destination "C:\Apps\code\CPP_Training\Chess\Chess_in_CPP_by_MS_Pilot\build\Release\Qt6Widgets.dll"
	Copy-Item -Path "C:\Apps\Qt\6.11.1\msvc2022_64\bin\Qt6Gui.dll" -Destination "C:\Apps\code\CPP_Training\Chess\Chess_in_CPP_by_MS_Pilot\build\Release\Qt6Gui.dll"
	Copy-Item -Path "C:\Apps\Qt\6.11.1\msvc2022_64\bin\Qt6Core.dll" -Destination "C:\Apps\code\CPP_Training\Chess\Chess_in_CPP_by_MS_Pilot\build\Release\Qt6Core.dll"
	
	```

	Run with:
		```
		.\Chess.exe
		.\chess_gui.exe
		
		```
### Tests and Perft Validation
	```
	.\legalmoves.exe "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" --san
	.\legalmoves.exe "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" --san
	.\chess_tests.exe
	
	Initial Position
	.\perft.exe 1 "startpos" --stats
	.\perft.exe 2 "startpos" --stats
	.\perft.exe 3 "startpos" --stats
	.\perft.exe 4 "startpos" --stats
	.\perft.exe 5 "startpos" --stats
	.\perft.exe 6 "startpos" --stats
	.\perft.exe 7 "startpos" --stats

	Position 2
	.\perft.exe 1 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" --stats
	.\perft.exe 2 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" --stats
	.\perft.exe 3 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" --stats
	.\perft.exe 4 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1" --stats
	
	Position 3
	.\perft.exe 1 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" --stats
	.\perft.exe 2 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" --stats
	.\perft.exe 3 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" --stats
	.\perft.exe 4 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" --stats
	.\perft.exe 5 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" --stats
	.\perft.exe 6 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1" --stats
	
	```

## Roadmap
	See roadmap.md for development phases and future work.