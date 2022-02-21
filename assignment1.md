#Assignment 1

My Assignment 1 was built with XCode and compiled with AppleClang. If there are compiler issues on another system, let me know and I can try to make changes. 

Once built you can run the executable in the command line, with 4 additional args:

(1)-file name (just the name, not the path)

(2)-filter option, either "fir" or "iir" (don't include the quotes in the command line)

(3)-delay value

(4)-gain value


The test files, along with files you'd like to run are/should be placed in the main directory. Included any test files you'd like there so that you just need the file name in the command line and not complicated paths. If you do need to change the path, you can change the "PATH\_TO\_DIR" var in MUSI6106Exec. 

Unfortunatly, the graphs are not included as Matlab was being ridiculous. Also, sometimes when running test 3 with a success case, there is a malloc error. This appears to be an issue with line 512 in AudioFile.cpp, which I didn't want to edit, but usually it runs without the error. The error for reference is "MUSI6106Exec(43643,0x107cdbdc0) malloc: Incorrect checksum for freed object 0x7fef6380aa00: probably modified after being freed.
Corrupt value: 0x3f7ffe003f7ffe00"

Contact me with any additonal questions or problems.