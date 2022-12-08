		#!/bin/bash

		# bring server to the foreground. Pressing Ctrl-C will impact server. Connected clients will also be killed
		#		First command into background is %1

		./server &
		sleep 1		# Allow server to open server socket
		./client &
		./client &
		fg %1
		echo server exit value $?
		sleep 5
		killall server	# Just to be sure, leaving no baggage

