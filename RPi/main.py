import wobbleBoard
import sys


def main():
	frequency = int(sys.argv[1])
	log_name = sys.argv[2]
	board = wobbleBoard.WobbleBoard()
	board.enableLogging(log_name)
	board.run(frequency)
	
main()
