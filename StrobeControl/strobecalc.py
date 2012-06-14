#!/usr/bin/python

import sys

clock = 16000000
minTimerVal = 32768

def calcTimerValues(freq):
	timerVal = 65536
	numclocks = int(round(clock/freq))
	lastTimerVal = 0
	numIntervals = int(numclocks / timerVal)
	lastTimerVal = numclocks % timerVal
	if numIntervals == 0:
		return (timerVal, numIntervals, lastTimerVal)
	if lastTimerVal < minTimerVal:

		delta = minTimerVal - lastTimerVal

		decrement = delta / numIntervals + 1;

		timerVal = timerVal - decrement
		numIntervals = int(numclocks / timerVal)
		lastTimerVal = numclocks % timerVal
	return (timerVal, numIntervals, lastTimerVal)


if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'usage ' + sys.argv[0] + ' frequency'
	else:
		freq = float(sys.argv[1])
		print 'Desired Frequency: ' + str(freq)
		(val, num, last) = calcTimerValues(freq)
		print 'Num Clocks: ' + str(val)
		print 'Num 65536 rollovers: ' + str(num)
		print 'Val for last interval: '  + str(last)
		print str(val) + ' ' + str(num) + ' ' + str(last)
		numClocks =  val * num + last
		print numClocks
		actual =  float(clock) / numClocks
		print actual
		print (actual - freq) / freq

