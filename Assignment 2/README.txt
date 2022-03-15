README

SYSC 4001 - Assignment 2
Conor Johnson Martin 101106217
Submitted March 15th, 2022

Files included:
(Within Assignment2_101106217.tar):
	-FILTER.c
	-Makefile
-Test_Results.txt
-Design_Goals.txt
-Pseudo_Code.txt

To build project:
-Ensure that you are in the correct directory
-Run the following command in the terminal: "make -f Makefile"
-Run the following command in the terminal: "./FILTER"
-Follow the promt to enter either "yes" or "no" if you would like to enter debug mode. The prompt should read: "Enter Debug Mode? (Insert yes or no):" ***Does not accomodate "YES" or "Yes"
-Follow the promt to enter a 7-element long string of numerals and letters ex. "123CDEF" or "5A9MW6Z". The prompt should read: "Enter A String Of 7 Alpha-Numeric Characters (No Spaces):" ***Does not need to be capitalized letters
-If in debug mode the processes will print when they are swapping or not swapping elements.
-The filtered reseult will be printed.