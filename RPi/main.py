import wobbleBoard

def main():
	board = wobbleBoard.wobbleBoard()
	board.enableLogging("log.csv")
	board.run(10)
	
main()
