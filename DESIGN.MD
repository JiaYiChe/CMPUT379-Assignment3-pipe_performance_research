Variance on implementation quality:
I had done the higher quality implementation.
--To ignore the skipize number lines I had setted up two checks, one check if checking if next line are the line with address and data size. 
The second check is setted after the first check is success, the second check will compare the current line number and skipize number.
(if still need skip then do nothing)
--To read sliding I first setted an scanf that will read test program's output in real time with a loop, and then for the windows size minus1 numbers of line will be only store in the hash table and an array that keep track on what page number had been stored.
Then in the next round after windows size minus 1, after checked by if statement to see if current line number are equal to windows size, the program will first insert this line into hash table and the keep track array. Then display the unique number contain in hash table right now and delete the earlies line stored in the hash table and also remove the record in the keep track array.
By this flow, after the windows size meet first time the rest of the cycle, will insert the new line show the unique number and delete the oldest line stored in. This is how I implemente sliding down.
--To adapt the unkown windows size, I setted the hash table by malloc, this way hash table will get the right space to store data without wast and ignored the risk of not big enough. 
In case if there are too many adjacent page I also setted up an check and a function by calling to realloc the hash table memory(go by realloc two time space, three times space, four times space....etc).
--I used struct to build the hash table.

Performance study:
The following answers are a short form with some explaination, more specific explaination and prove will be given in the report.
(Because the MD cannot include any image so the plot graph and more specific comparation will be given in the report)
These answer are given based on the graph I got.
a) The performance of the given three test are mostly similar, but by the graph heap sort isn't good as the radixsort and quicksort. Heapsort had used up more cache and covered more unique pages than the other two sort.This can be prove by the plot that keeped the first 10k line, comparing with other two sort plot that skip 10k the heapsort plot shows a significantly continue large unique pages coverage. The quicksort and radixsort are almost the same except radixsort are taking more time than the quick sort.

b)The larger the input instance is the more unique pages and cache will be use (the base page use are similar but more peak shows). All three test showed more unique pages usage with the input larger, and heapsort definitely used more than the other two sort (more peaks). The group of all three plots grow their input to 100 can prove it (my normal input to test is 11 and tested more input test then choose 100 to show because its fast than the more larger input and clearly show the difference between heap and other two sort).

c)As I said in part a) quicksort's and radixsort's are similar to the their no skip plot's part after the skip size, you can fit those skip plot into the end of their general plot without many difference. The heapsort and showed a big difference to its general plot's after skip size part.

b)As windows size grow up each sort plot showing a growth at unique page usage, which is cause by the windows size grow so more unique pages had been counted at the same time. When the cardinal number growth the unique number is also grow with it. A positive linear relation.

e)When the page number growth the plot shows the usage of unique pages are getting less. This can cause by when the page size is larger there are more likely to have adjacent page.So the growth of page size and the usage of unique pages can be negative relation.
(The only exception is heapsort shows more peak when the page number growth but the unique page number amount is reduced)

Inportant to Known:
--By some reason the if w+ are used in fopen, the result won't be write into file. So, I used a+ for the fopen which mean everytime if there already exist an "workSet"(name I give for the file contain results) will need to delete that file first before the next run. Otherwise the new result will be write after the old one.
--If the output file that store the result are too big, the gnuplot will crash (something thats over 500k lines by my test).
--For the data I got I didn't input any large number (used 11 and 100) to test program because It takes very long time get the result of large input (have some time issue to do the report after the program), secondly the gnuplot crashes so I choose to use small input. By the graph I got I can tell they are very similar I think its because of the size of data I got isn't big enough to see the real difference.

Cites I used:
(modified the hash table)
https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm 

The command I used for getting all the plot data in the design.pdf:
(didn't cover all the plot graph I made, and for the windows size I had used 100, 1000, 10000, the test program had used 11 and 100 and page size had used 4096 and 8192 (theses number are the plot I used in the report, didn't include all the graph I made))
(anything write with or means can be change to that number not really include the or in the command line)
valgrind --tool=lackey --trace-mem=yes ./radixsort 11 or 100 2>&1|./valws379 4096 100 or 1000 or 10000

valgrind --tool=lackey --trace-mem=yes ./radixsort 11 2>&1|./valws379 8192 100 or 1000 or 10000

valgrind --tool=lackey --trace-mem=yes ./heapsort 11 2>&1|./valws379 4096 100

valgrind --tool=lackey --trace-mem=yes ./heapsort 11 2>&1|./valws379 8192 100

valgrind --tool=lackey --trace-mem=yes ./quicksort 11 2>&1|./valws379 4096 100

valgrind --tool=lackey --trace-mem=yes ./quicksort 11 2>&1|./valws379 8192 100

valgrind --tool=lackey --trace-mem=yes ./radixsort 11 2>&1|./valws379 100000 4096 10000

valgrind --tool=lackey --trace-mem=yes ./radixsort 11 2>&1|./valws379 100000 8192 10000

valgrind --tool=lackey --trace-mem=yes ./heapsort 11 2>&1|./valws379 100000 4096 10000

valgrind --tool=lackey --trace-mem=yes ./heapsort 11 2>&1|./valws379 100000 8192 10000

valgrind --tool=lackey --trace-mem=yes ./quicksort 11 2>&1|./valws379 100000 4096 10000

valgrind --tool=lackey --trace-mem=yes ./quicksort 11 2>&1|./valws379 100000 8192 10000

Current error:
--There isn't significant erro occur, but I notice when the program got too large or too small page number and windows size will get Segmentation fault. This error doesn't not show all the time but might happen.
--Another thing I notice is there once have a problem of infinite loop, this is caused by hash table had been entered many large unknown numbers that over write the page number cause the search function cannot find the page number's location in the hash table. I use free(malloc) solved this problem, never happened again to be but might occur once again.

Plot:
Here is an example command I used to plot my graph
/**
#turn the out put to pdf mode (should be png)
set terminal pdf 
#set up a name for the output pdf
set output "pageSize 4096 10000 input 100 radixsort.pdf"
#set up the title for the plot graph
set title "radixsort 4096 input 100"
#sign r100 to be the file to read data from, output as histogram with type 9 color and give a sign to the data
plot "r100" with boxes lt 9 title "10000 windows size"
**/


