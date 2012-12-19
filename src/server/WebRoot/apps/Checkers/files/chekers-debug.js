var Checkers = new function() {
	var kBoardWidth = 8;
	var kBoardHeight = 8;
	var kPieceWidth = Math.min(window.innerWidth, window.innerHeight - 60) / 8;
	var kPieceHeight = Math.min(window.innerWidth, window.innerHeight - 60) / 8;
	var kPixelWidth = 1 + (kBoardWidth * kPieceWidth);
	var kPixelHeight = 1 + (kBoardHeight * kPieceHeight);

	var gCanvasElement;
	var gDrawingContext;
	var gPattern;

	var redTurn;

	var redPieces;
	var blackPieces;
	var gNumPieces;
	var gSelectedPieceIndex;
	var gSelectedPieceHasMoved;
	var redMoveCount;
	var blackMoveCount;
	var gMoveCount;
	var gMoveCountElem;
	var redMoveCountElem;
	var blackMoveCountElem;
	var gWhosTurnElem;
	var gGameInProgress;
	var gTheresAnotherMove;

	/*
	 * Color constants
	 */
	colorSpaceRed = "#000000";
	colorSpaceBlack = "lightgray";

	var Cell = function(row, column, king) {
		this.row = row;
		this.column = column;
		this.isKing = king;
	};

	var getCursorPosition = function(e) {
		/* returns Cell with .row and .column properties */
		var x;
		var y;
		if (e.pageX != undefined && e.pageY != undefined) {
			x = e.pageX;
			y = e.pageY;
		} else {
			x = e.clientX + document.body.scrollLeft
					+ document.documentElement.scrollLeft;
			y = e.clientY + document.body.scrollTop
					+ document.documentElement.scrollTop;
		}
		x -= gCanvasElement.offsetLeft;
		y -= gCanvasElement.offsetTop;
		x = Math.min(x, kBoardWidth * kPieceWidth);
		y = Math.min(y, kBoardHeight * kPieceHeight);
		var cell = new Cell(Math.floor(y / kPieceHeight), Math.floor(x
						/ kPieceWidth));
		return cell;
	};

	var endGame = function() {
		gSelectedPieceIndex = -1;
		gGameInProgress = false;
	};

	/*
	 * Is there a piece on this cell? param cell: the cell to check param red:
	 * true if you're looking for a red piece false if you're looking for a
	 * black piece
	 * 
	 * returns true if there is a piece of the specified color on that spot.
	 */
	var isThereAPieceHere = function(cell, red) {
		if (red) {
			for (var i = 0; i < gNumPieces; i++) {
				if ((redPieces[i].row == cell.row)
						&& (redPieces[i].column == cell.column)) {
					return true;
				}
			}
		} else {
			/* black piece here? */
			for (var i = 0; i < gNumPieces; i++) {
				if ((blackPieces[i].row == cell.row)
						&& (blackPieces[i].column == cell.column)) {
					return true;
				}
			}
		}

		return false;
	};

	var isTheGameOver = function() {

		var redLeft = false;
		var blackLeft = false;

		for (var i = 0; i < gNumPieces; i++) {
			if (redPieces[i].row > 0) {
				/* We have at least one valid piece left */
				redLeft = true;
				break;
			}
			if (redPieces[i].column > 0) {
				/* We have at least one valid piece left */
				redLeft = true;
				break;
			}
		}

		for (var i = 0; i < gNumPieces; i++) {
			if (blackPieces[i].row > 0) {
				/* We have at least one valid piece left */
				blackLeft = true;
				break;
			}
			if (blackPieces[i].column > 0) {
				/* We have at least one valid piece left */
				blackLeft = true;
				break;
			}
		}

		if (!redLeft) {
			console.debug("BLACK WON THIS ONE!!!!");
			gWhosTurnElem.innerHTML = "Black Wins!!";
			return true;
		}

		if (!blackLeft) {
			console.debug("RED WON THIS ONE!!!!");
			gWhosTurnElem.innerHTML = "Red Wins!!";
			return true;
		}

		return false;
	};

	var checkAndMakeKing = function() {
		if (redTurn) {
			/* red's turn */
			/* If we're already a king - stay a king! */
			if (redPieces[gSelectedPieceIndex].isKing) {
				console.debug("We got ourselves a King!");
				return true;
			}
			/* Check if we hit the last row */
			if (redPieces[gSelectedPieceIndex].row == 0) {
				console.debug("We got ourselves a King!");
				return true;
			}
		} else {
			/* black's turn */
			/* If we're already a king - stay a king! */
			if (blackPieces[gSelectedPieceIndex].isKing) {
				console.debug("We got ourselves a King!");
				return true;
			}
			/* Check if we hit the last row */
			if (blackPieces[gSelectedPieceIndex].row == 7) {
				console.debug("We got ourselves a King!");
				return true;
			}
		}
		console.debug("Not a King");
		return false;
	};

	var drawPiece = function(p, selected, black) {
		var column = p.column;
		var row = p.row;
		var x = (column * kPieceWidth) + (kPieceWidth / 2);
		var y = (row * kPieceHeight) + (kPieceHeight / 2);
		var radius = (kPieceWidth / 2) - (kPieceWidth / 10);
		if ((redTurn && !black) || (!redTurn && black)) {
			gDrawingContext.beginPath();
			gDrawingContext.arc(x, y, radius + 3, 0, Math.PI * 2, false);
			gDrawingContext.closePath();
			gDrawingContext.strokeStyle = "Transparent";
			gDrawingContext.stroke();
			gDrawingContext.fillStyle = "Transparent";
			gDrawingContext.fill();
		}

		gDrawingContext.beginPath();
		gDrawingContext.arc(x, y, radius, 0, Math.PI * 2, false);
		gDrawingContext.closePath();
		gDrawingContext.strokeStyle = "#000";
		gDrawingContext.stroke();
		if (black) {
			gDrawingContext.fillStyle = "#404040";
		} else {
			gDrawingContext.fillStyle = "#FF0000";
		}
		gDrawingContext.fill();

		if (selected) {
			gDrawingContext.fillStyle = "White";
			gDrawingContext.fill();
		}
		
		if(p.isKing){
			var crown  = new Image();
			crown.src = "../img/crown.png";
			crown.onload = function(){
				gDrawingContext.drawImage(crown, (column * kPieceWidth)+7, (row * kPieceHeight)+6,35,35);
				
			}			
		}
		
	};

	var fillCellBg = function(row, column, red) {
		var x = (column * kPieceWidth);
		var y = (row * kPieceHeight);

		if (red) {
			gDrawingContext.fillStyle = colorSpaceRed;
		} else {
			gDrawingContext.fillStyle = colorSpaceBlack;
		}

		gDrawingContext.fillRect(x, y, kPieceWidth, kPieceHeight);

	};

	var saveGameState = function() {
		if (typeof resumeGame != "function") {

			resumeGame = function() {
				return false;
			};
			return false;
		}
	};

	var isARealCell = function(cell) {

		if (cell.row < 0 || cell.column < 0
			|| cell.row > 7 || cell.column > 7) {
			return false;
		}

		return true;
	};

	var drawBoard = function(){
	

		gDrawingContext.clearRect(0, 0, kPixelWidth, kPixelHeight);
			
		gDrawingContext.beginPath();
			
		/* vertical lines */
		for (var x = 0; x <= kPixelWidth; x += kPieceWidth) {
			gDrawingContext.moveTo(0.5 + x, 0);
			gDrawingContext.lineTo(0.5 + x, kPixelHeight);
		}
			
		/* horizontal lines */
		for (var y = 0; y <= kPixelHeight; y += kPieceHeight) {
			gDrawingContext.moveTo(0, 0.5 + y);
			gDrawingContext.lineTo(kPixelWidth, 0.5 + y);
		}
			
		/* draw it! */
		gDrawingContext.strokeStyle = "#ccc";
		gDrawingContext.stroke();
			
		/* Checkerboard fill style */
		for (var i = 0; i < kBoardHeight; i++) {
			// loop through all rows
			
			for (var j = 0; j < kBoardWidth; j++) {
				/*
		 * Loop through all cells of current row Shade every other cell
		 * for that checkerboard effect.
		 */
				if (i % 2 != j % 2) {
					/* Fill her black */
					fillCellBg(i, j, false);
				}
				else {
					/* Fill her red */
					fillCellBg(i, j, true);
				}
				
			}
		}
			
		for (var i = 0; i < 12; i++) {
			drawPiece(redPieces[i], (i == gSelectedPieceIndex && redTurn), false);
		}
		
		for (var i = 0; i < 12; i++) {
			drawPiece(blackPieces[i], (i == gSelectedPieceIndex && !redTurn), true);
		}
		
		gMoveCountElem.innerHTML = gMoveCount;
		redMoveCountElem.innerHTML = redMoveCount;
		blackMoveCountElem.innerHTML = blackMoveCount;
		
		var turnText = "Black's Turn!";
		if (redTurn) {
			turnText = "Red's Turn!";
		}
		
		gWhosTurnElem.innerHTML = turnText;
		
		saveGameState();

		if (gGameInProgress && isTheGameOver()) {
			endGame();
		}
	};

	/*
	 * Given a cell which has a selected piece, this method determines if there
	 * is another valid jump available. Returns True if so, false otherwise.
	 */
	var isThereAnotherJump = function(cell1) {

		if (!redTurn || redPieces[gSelectedPieceIndex].isKing) {
			/* black's turn */
			console.debug("It's black's turn.  Checking if we have another jump.");
			/* red piece 1 is */
			var cellSE = new Cell(cell1.row + 1, cell1.column + 1);
			var cellSW = new Cell(cell1.row + 1, cell1.column - 1);
			var destSE = new Cell(cellSE.row + 1, cellSE.column + 1);
			var destSW = new Cell(cellSW.row + 1, cellSW.column - 1);

			console.debug("Selected cell: ( " + cell1.row + " , "
					+ cell1.column + " )\n" + "Between SE: ( " + cellSE.row
					+ " , " + cellSE.column + " )\n" + "Between SW: ( "
					+ cellSW.row + " , " + cellSW.column + " )\n"
					+ "Dest SE: ( " + destSE.row + " , " + destSE.column
					+ " )\n" + "Dest SW: ( " + destSW.row + " , "
					+ destSW.column + " )\n");
			

			/* red's turn */
				var pieceColor = !redTurn;
	
				if ((isThereAPieceHere(cellSE, pieceColor))) {
					if(pieceColor)
						console.debug("Red Piece detected at Between SE");

					if ((!(isThereAPieceHere(destSE, true)) && !(isThereAPieceHere(
							destSE, false)))
							&& (isARealCell(destSE))) {
						console.debug("There is not a piece at the Dest SE cell and Dest SE is a real cell.");
						return true;
					}
				}

				if ((isThereAPieceHere(cellSW, pieceColor))) {
					if(pieceColor)
						console.debug("Red Piece detected at Between SW");
					if ((!(isThereAPieceHere(destSW, true)) && !(isThereAPieceHere(
							destSW, false)))
							&& (isARealCell(destSW))) {
						console.debug("There is not a piece at the Dest SW cell and Dest SW is a real cell.");
						return true;
					}
				}
			
			
		} 
		
		if(redTurn || blackPieces[gSelectedPieceIndex].isKing){

			/* red's turn */
			console.debug("It's red's turn.  Checking if we have another jump.");
			/* red piece 1 is */
			var cellNE = new Cell(cell1.row - 1, cell1.column + 1);
			var destNE = new Cell(cellNE.row - 1, cellNE.column + 1);
			var cellNW = new Cell(cell1.row - 1, cell1.column - 1);
			var destNW = new Cell(cellNW.row - 1, cellNW.column - 1);

			console.debug("Selected cell: ( " + cell1.row + " , "
					+ cell1.column + " )\n" + "Between NE: ( " + cellNE.row
					+ " , " + cellNE.column + " )\n" + "Between NW: ( "
					+ cellNW.row + " , " + cellNW.column + " )\n"
					+ "Dest NE: ( " + destNE.row + " , " + destNE.column
					+ " )\n" + "Dest NW: ( " + destNW.row + " , "
					+ destNW.column + " )\n");
			

			var pieceColor = !redTurn

			/* red's turn */
			if ((isThereAPieceHere(cellNE, pieceColor))) {
				if(pieceColor)
					console.debug("Black Piece detected at Between NE");

				if ((!(isThereAPieceHere(destNE, true)) && !(isThereAPieceHere(
						destNE, false)))
						&& (isARealCell(destNE))) {
					console.debug("There is not a piece at the Dest NE cell and Dest NE is a real cell.");
					return true;
				}
			}
			if ((isThereAPieceHere(cellNW, pieceColor))) {
				if(pieceColor)
					console.debug("Red Piece detected at Between NW");
				if ((!(isThereAPieceHere(destNW, true)) && !(isThereAPieceHere(
						destNW, false)))
						&& (isARealCell(destNW))) {
					console.debug("There is not a piece at the Dest NW cell and Dest NW is a real cell.");
					return true;
				}
			}

		}

		console.debug("No more jumps available. changing turns");

		return false;
	};

	// Maybe call this is jump?
	var isThereAPieceBetween = function(cell1, cell2) {
		/*
		 * note: assumes cell1 and cell2 are 2 squares away either vertically,
		 * horizontally, or diagonally
		 */
		var rowBetween = (cell1.row + cell2.row) / 2;
		var columnBetween = (cell1.column + cell2.column) / 2;

		console.debug("Checking between cell ( " + rowBetween + " , " + columnBetween + " ).");

		if (!redTurn) {
			for (var i = 0; i < gNumPieces; i++) {
				console.debug("red piece[" + i + "]: ( " + redPieces[i].row + " , " + redPieces[i].column + " )");
				if ((redPieces[i].row == rowBetween)
						&& (redPieces[i].column == columnBetween)) {
					// redPieces[i].row = -1;
					// redPieces[i].column = -1;
					return i;
				}
			}
			return false;
		} else {
			for (var i = 0; i < gNumPieces; i++) {
				console.debug("black piece[" + i + "]: ( " + blackPieces[i].row + " , " + blackPieces[i].column + " )");
				if ((blackPieces[i].row == rowBetween)
						&& (blackPieces[i].column == columnBetween)) {
					// blackPieces[i].row = -1;
					// blackPieces[i].column = -1;

					return i;
				}
			}
			return -1;
		}
	};

	var clickOnEmptyCell = function(cell) {
		if (gSelectedPieceIndex == -1) {
			return;
		}

		if (redTurn) {
			console.debug("Clicked on Empty Cell.  Red Turn");
			/*
			 * Red can only move up. A Red in Row 3 can move to Row 4 or Row 5
			 * if jumping
			 */
			var rowDiff = redPieces[gSelectedPieceIndex].row - cell.row;
			if (redPieces[gSelectedPieceIndex].isKing) {
				/*
				 * Since we already checked in one direction, we can use the
				 * whole same code if we take the absolute value and try again..
				 */
				rowDiff = Math.abs(cell.row - redPieces[gSelectedPieceIndex].row);
				
			}
			var columnDiff = Math.abs(cell.column - redPieces[gSelectedPieceIndex].column);

			if ((rowDiff == 1)
					&& /* Make sure that this is a valid square. Has to b */
					(columnDiff == 1)
					&& !gSelectedPieceHasMoved
					&& !isThereAPieceHere(cell, true)
					&& !isThereAPieceHere(cell, false)) {
				console.debug("Empty Cell: Valid One Space move for Red.");
				/* we already know that this click was on an empty square */
				/* so that must mean this was a valid single-square move */
				redPieces[gSelectedPieceIndex].row = cell.row;
				redPieces[gSelectedPieceIndex].column = cell.column;
				redPieces[gSelectedPieceIndex].isKing = checkAndMakeKing();
				gMoveCount += 1;
				redMoveCount += 1;
				gSelectedPieceIndex = -1;
				gSelectedPieceHasMoved = false;
				redTurn = false;
				console.debug("Black's turn.")
				drawBoard();
				return;
			}


/*
			console.debug("rowDiff = " + rowDiff + " columnDiff = " + columnDiff);
			if (!isThereAPieceHere(cell, true))
				console.debug("There is Not a red piece at your dest cell");
			else
				console.debug("There is a red piece at your dest cell");

			if (!isThereAPieceHere(cell, false))
				console.debug("There is Not a Black piece at your dest cell");
			else
				console.debug("There is a Black piece at your dest cell");

			if (isThereAPieceBetween(redPieces[gSelectedPieceIndex], cell) >= 0)
				console.debug("There is a Black piece between the piece and your dest cell");
			else
				console.debug("There is not a Black piece between the piece and your dest cell");
*/
			if (((rowDiff == 2) && (columnDiff == 2))
					&& (isThereAPieceBetween(redPieces[gSelectedPieceIndex],cell) >= 0) 
					&& !isThereAPieceHere(cell, true)
					&& !isThereAPieceHere(cell, false)) {
				/* this was a valid jump */
				console.debug("Empty Cell: Valid Jump for Red.");

				/* Remove the piece between */
				var blackBtwInx = isThereAPieceBetween(
						redPieces[gSelectedPieceIndex], cell);
				blackPieces[blackBtwInx].row = -1;
				blackPieces[blackBtwInx].column = -1;

				/*
				 * We up the move counts if this was our first move in the leap
				 * series
				 */
				if (!gSelectedPieceHasMoved) {
					gMoveCount += 1;
					redMoveCount += 1;
				}

				/* Move the piece to the clicked location */
				redPieces[gSelectedPieceIndex].row = cell.row;
				redPieces[gSelectedPieceIndex].column = cell.column;
				redPieces[gSelectedPieceIndex].isKing = checkAndMakeKing();

				/*
				 * If there's a valid second jump available, we keep our turn
				 * and we set the piece moved to true.
				 */
				if (isThereAnotherJump(cell)) {
					gSelectedPieceHasMoved = true;
					redTurn = true;
					console.debug("Remains Red's turn.");
					drawBoard();
				} else {
					gSelectedPieceHasMoved = false;
					gSelectedPieceIndex = -1;
					redTurn = false;

					console.debug("Black's turn");
					drawBoard();
				}
				return;
			}
			
			
				
		} else {
			/* black's turn */
			console.debug("Clicked on Empty Cell.  Black Turn");
			/*
			 * Black can only move down. I.e. A black piece in Row 5 can move to
			 * Row 6 or Row 7 if jumping
			 */

			var rowDiff = cell.row - blackPieces[gSelectedPieceIndex].row;
			if(blackPieces[gSelectedPieceIndex].isKing){
				rowDiff =  Math.abs(cell.row - blackPieces[gSelectedPieceIndex].row);
			}
			var columnDiff = Math.abs(cell.column
					- blackPieces[gSelectedPieceIndex].column);

			if ((rowDiff == 1)
					&& (columnDiff == 1)
					&& !gSelectedPieceHasMoved
					&& (!isThereAPieceHere(cell, true) 
					&& !isThereAPieceHere(cell, false))) {
				console.debug("Empty Cell: Valid One Space move for Black.");

				/*
				 * we already know that this click was on an empty square, so
				 * that must mean this was a valid single-square move
				 */
				blackPieces[gSelectedPieceIndex].row = cell.row;
				blackPieces[gSelectedPieceIndex].column = cell.column;
				blackPieces[gSelectedPieceIndex].isKing = checkAndMakeKing();
				gMoveCount += 1;
				blackMoveCount += 1;
				gSelectedPieceIndex = -1;
				gSelectedPieceHasMoved = false;
				redTurn = true;
				console.debug("Red's Turn.");
				drawBoard();
				return;
			}



			if ((((rowDiff == 2) && (columnDiff == 2)) 
					&& (isThereAPieceBetween(blackPieces[gSelectedPieceIndex], cell) >= 0)
					&& !isThereAPieceHere(cell, true)
					&& !isThereAPieceHere(cell, false))) {
				/* this was a valid jump */
				console.debug("Empty Cell: Valid Jump for Black.");

				/* Remove the piece between */
				var redBtwInx = isThereAPieceBetween(
						blackPieces[gSelectedPieceIndex], cell);
				redPieces[redBtwInx].row = -1;
				redPieces[redBtwInx].column = -1;

				/*
				 * We up the move counts if this was our first move in the leap
				 * series
				 */
				if (!gSelectedPieceHasMoved) {
					gMoveCount += 1;
					blackMoveCount += 1;
				}

				/* Move the piece to the clicked location */
				blackPieces[gSelectedPieceIndex].row = cell.row;
				blackPieces[gSelectedPieceIndex].column = cell.column;
				blackPieces[gSelectedPieceIndex].isKing = checkAndMakeKing();

				/*
				 * If there's a valid available, we keep our turn and we set the
				 * piece moved to true.
				 */
				if (isThereAnotherJump(cell)) {
					gSelectedPieceHasMoved = true;
					redTurn = false;
					console.debug("Remains Black Turn.");
					drawBoard();
				} else {
					gSelectedPieceHasMoved = false;
					gSelectedPieceIndex = -1;
					redTurn = true;
					console.debug("Red's Turn.");
					drawBoard();
				}
				return;
			}
					
		}

		/* And if neither of the above worked, nothing happened. */
		gSelectedPieceIndex = -1;
		if (gSelectedPieceHasMoved)
			redTurn = !redTurn;

		console.debug("Not a valid move.");

		gSelectedPieceHasMoved = false;
		drawBoard();

	};

	var clickOnPiece = function(pieceIndex, isBlack) {
		if (gSelectedPieceIndex == pieceIndex) {
			return;
		}
		gSelectedPieceIndex = pieceIndex;
		gSelectedPieceHasMoved = false;
		drawBoard();
	};

	var checkerOnClick = function(e) {
		var cell = getCursorPosition(e);
		for (var i = 0; i < gNumPieces; i++) {
			if (redTurn && !gSelectedPieceHasMoved
					&& (redPieces[i].row == cell.row)
					&& (redPieces[i].column == cell.column)) {
				clickOnPiece(i, false);
				return;
			} else if (!redTurn && (blackPieces[i].row == cell.row)
					&& (blackPieces[i].column == cell.column)) {
				clickOnPiece(i, true);
				return;
			}

		}
		clickOnEmptyCell(cell);
	};

	/*
	 * New game function starts a new game.
	 */
	var newGame = function() {
		/*
		 * First it initializes the redPieces and blackPieces lists with the
		 * proper checkers starting positions for each piece The pieces are set
		 */
		redPieces = [new Cell(kBoardHeight - 3, 0, false),
				new Cell(kBoardHeight - 3, 2, false),
				new Cell(kBoardHeight - 3, 4, false),
				new Cell(kBoardHeight - 3, 6, false),
				new Cell(kBoardHeight - 2, 1, false),
				new Cell(kBoardHeight - 2, 3, false),
				new Cell(kBoardHeight - 2, 5, false),
				new Cell(kBoardHeight - 2, 7, false),
				new Cell(kBoardHeight - 1, 0, false),
				new Cell(kBoardHeight - 1, 2, false),
				new Cell(kBoardHeight - 1, 4, false),
				new Cell(kBoardHeight - 1, 6, false)];
		blackPieces = [new Cell(0, 1, false), new Cell(0, 3, false),
				new Cell(0, 5, false), new Cell(0, 7, false),
				new Cell(1, 0, false), new Cell(1, 2, false),
				new Cell(1, 4, false), new Cell(1, 6, false),
				new Cell(2, 1, false), new Cell(2, 3, false),
				new Cell(2, 5, false), new Cell(2, 7, false)];

		/*
		 * Game's number of pieces is the same at the start - 12 pieces per
		 * player
		 */
		gNumPieces = redPieces.length;
		/* initial value -1 is treated as "nothing selected" */
		gSelectedPieceIndex = -1;
		/*
		 * after the piece moves, if it was a jump, and there is another valid
		 * checker jump available, we set this value to true so we know we're in
		 * a must-jump phase. Initially this value is false.
		 */
		gSelectedPieceHasMoved = false;
		/*
		 * Move counts to tell you how many moves it took to whoop your opponent
		 */
		gMoveCount = 0;
		redMoveCount = 0;
		blackMoveCount = 0;
		gGameInProgress = true;
		redTurn = true;
		drawBoard();
	};

	/**
	 * This is the only public method. It is what initializes the game board on
	 * the canvas and gets ready to handle clicks and preform game logic
	 */
	this.initCheckersGame = function(canvasElement, moveCountElement,
			whosTurnElement, redCountElement, blackCountElement) {

		window.onresize = function(){
			kPieceWidth = Math.min(window.innerWidth, window.innerHeight - 60) / 8;
			kPieceHeight = Math.min(window.innerWidth, window.innerHeight - 60) / 8;
			kPixelWidth = 1 + (kBoardWidth * kPieceWidth);
			kPixelHeight = 1 + (kBoardHeight * kPieceHeight);
			gCanvasElement.width = kPixelWidth;
			gCanvasElement.height = kPixelHeight;

			drawBoard();
		}
		/*
		 * If a canvas element was not passed in, create one and append it to
		 * the DOM.
		 */
		if (!canvasElement) {
			canvasElement = document.createElement("canvas");
			canvasElement.id = "checkerscanvas";
			document.body.appendChild(canvasElement);
		}
		/*
		 * If a move count element was not passed in, create one and append it
		 * to the DOM.
		 */
		if (!moveCountElement) {
			moveCountElement = document.createElement("p");
			document.body.appendChild(moveCountElement);
		}
		/*
		 * If a move count element was not passed in, create one and append it
		 * to the DOM.
		 */
		if (!whosTurnElement) {
			whosTurnElement = document.createElement("p");
			document.body.appendChild(whosTurnElement);
			alert("Creating it");
		}
		gCanvasElement = canvasElement;
		gCanvasElement.width = kPixelWidth;
		gCanvasElement.height = kPixelHeight;
		/* Register the click handler */
		gCanvasElement.addEventListener("click", checkerOnClick, false);
		gMoveCountElem = moveCountElement;
		redMoveCountElem = redCountElement;
		blackMoveCountElem = blackCountElement;
		gWhosTurnElem = whosTurnElement;
		gDrawingContext = gCanvasElement.getContext("2d");

		/* Lets give all of our our lines a cool shadow effect */
		/*
		 * gDrawingContext.shadowOffsetX=2; gDrawingContext.shadowOffsetY=2;
		 * gDrawingContext.shadowBlur=50; gDrawingContext.shadowColor="gray";
		 */

		/* And finally, start the game */
		if (!resumeGame()) {
			newGame();
		}
	};

};
