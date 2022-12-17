		#!/bin/bash

		# bring server to the foreground. Pressing Ctrl-C will impact server. Connected clients will also be killed
		#		First command into background is %1

		echo ">>> Starting server in the background"
		./server &
		sleep 1		# Allow server to open server socket
		echo ">>> Starting clients in parallel"
		./client &
		./client &
		echo ">>> Waiting for clients to complete"
		jobs
		# sleep 5	# Allow all programs to complete
		wait %2 %3
		echo ">>> Killing server and clients"
		killall server	client # Just to be sure, leaving no baggage

