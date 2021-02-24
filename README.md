# piperoller
Linux pipe that rolls through log file versions in response to signal HUP.

Usage: piperoller file_base_name

Example:

bigprogram | piperoller big.log

This would direct output from bigprogam to a series of files big.log.0, 
big.log.1,and so forth. The output switches files each time piperoller receives
a HUP signal: killall -HUP piperoller
