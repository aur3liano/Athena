# Chess Engine Roadmap
A forward-looking guide for the development of the C++ chess engine.

✅ Phase 1 — Core Architecture (Completed)
	Board
		Full piece grid representation
		Correct FEN-compatible state
		Proper halfmove/fullmove tracking
		Reversible applyMove / undoMove
		Accurate isSquareAttacked
		EP and castling state logic corrected
		Fully validated move legality via simulation
	Move System
		Full support for captures, promotions, EP, castling
		MoveState snapshot for reversible operations
	Move Generator
		Complete pseudolegal generator
		Legal filtering with king-safety enforcement
		Correct castling legality (no square under attack)
	Notation
		SAN parsing + SAN generation
		Disambiguation
		Promotion syntax
		Check and checkmate indicators
	FEN
		Full load/save
		Correct piece parsing
		Correct castling, EP, and clock handling
		Fully consistent with Board’s internal state
	Game Controller
		Turn tracking
		Game over detection (checkmate/stalemate)
	CMake Build System
	Catch2 test harness with perft validation tool

✅ Phase 2 — GUI Frontend
	Technologies:
		Qt6
	Current Features:
		Drag-and-drop move input
	Future Features:
		Move Log
		Control Panel
		Clock

🚧 Phase 3 — Agent Development
	Goals:
		Create a static evaluation functions:
			Material
			Piece-square tables
			Piece Mobility
			King safety
			Pawn structure (isolated, doubled, passed)
			Pluggable architecture for future tuning
		Create search systems:
			Classic minimax search
			Alpha-beta pruning
			Move ordering:
			MVV-LVA
			Killer heuristic
			History heuristic
			Iterative deepening
			Monte Carlo Tree Search
			Transposition tables
	Deliverables:
		evaluation.h/.cpp
		search.h/.cpp
		Principal Variation extraction
		Time control system
		Test suite for eval consistency
		Per-piece square tables

🚧 Phase 4 — PGN Support
	Goals:
		Parse PGN files
		Export PGN from Game history
		Support SAN annotations, comments

🚀 Long-Term Vision
	Is modular, maintainable, and educational
	Provides perfect legal move generation
	Supports PGN/FEN
	Has a clean UI and GUI
	A fully featured C++ chess engine that:
	Plays at a strong club level (1800–2000 Elo)
	Has a complete AI search system
