数据结构实验

纯C语言实现

一个简易的哈夫曼编码/解码的实验程序，先根据字符和权值构造哈夫曼树，并存储在文件hfmTree中。
再根据已有的哈夫曼树从TextFile文件中读入明文，编码，将密文文件存入CodeFile。
特别地，编码中处理最后一个有效字节时会填充比特0，再添加一个字节用于标志最后一个有效字节的有效比特数，解码时也做对最后两个字节作相应特殊处理。

Data structure experiment

Coded with C

A simple huffman encoder/decoder program.
Initialise by creating the huffman tree file "hfmTree".
With huffman tree initialised, encode the "TextFile" and save in "CodeFile".
Note that the last byte of the encoded file represents how many bits are significant in the byte before the last byte.