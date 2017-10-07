import wobbleBoard

def main():
	board = wobbleBoard.WobbleBoard()
	board.enableLogging("log.csv")
	board.run(10)
	
main()
