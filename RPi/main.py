import wobbleBoard
import sys
import signal

board = wobbleBoard.WobbleBoard()


def sigint_handler(signal, frame):
        print('Exiting...')
	board.SIGINT(signal,frame)
        sys.exit(0)

signal.signal(signal.SIGINT, sigint_handler)
signal.signal(signal.SIGTERM,sigint_handler)

def main():
	frequency = int(sys.argv[1])
	log_name = sys.argv[2]
	board.enableLogging(log_name)
	board.run(frequency)
	
main()
