# GFinder

This is the implementation of paper "GFinder: Approximate attributed subgraph matching".

History:
At the begining, the algorithm was named FastDynamic and I submitted the paper to Ijcai, but it was rejected by Ijcai.
Then, I renamed the algorithm to GLooker, and submitted it to CIKM, but it was rejected again.
Finally, I renamed the algorithm to GFinder, and the paper was accepted. 

If you have anyquestion about the algorithm, please open an issue. I will try to answer your problem if I have time.
But the most important thing is you should try to understand the code by yourself.
PS. There is no bug in the code.



Updated 2020-Dec-12. Another version of G-Finder which can support edge type similarity can be found at https://github.com/lihuiliullh/GFinder-Proj
It contains some docx which tell details of how to run G-Finder.
Its test data can also be used here.



------------------------------------------------------------------------------------------------------------------------

This Project (G-Looker) is implemented on visual studio C++ 2015, windows 10 64 bits.
The source code is stored in directory "./FastDynamic-edge-type-list-hash"
Some sample data files are stored in direcotry "./data"

Usage:
If you want to run G-Looker, use the command like this:

FastDynamic.exe data_file_name query_file_name top-k node-miss-cost  intermediate-vertex-cost  edge-miss-cost hop-number

For example:

.\FastDynamic.exe C:\flickr.txt.format C:\query_noise_7.txt.format 10 30 10 10 0



data_file_name: The data file
query_file_name: The query file
top-k: how many results you would like to find. 
node-miss-cost: missing node cost used in loss function (see our paper for more details)
intermediate-vertex-cost:  intermediate vertex cost used in loss function (see our paper for more details)
edge-miss-cost: missing edge cost used in loss function (see our paper for more details)
hop-number: if hop-number == 0, means 2 hop. If hop-number >=1 means 1 hop.




For example:

.\FastDynamic.exe C:\flickr.txt.format C:\query_noise_7.txt.format 10 30 10 10 0    (use two-hop distance)
.\FastDynamic.exe C:\flickr.txt.format C:\query_noise_7.txt.format 10 30 10 10 1    (use one-hop distance)
.\FastDynamic.exe C:\flickr.txt.format C:\query_noise_7.txt.format 10 30 10 10 2    (use one-hop distance)
