# Text-Firewall Documentation

## Directions
1) Open up the command line in Ubuntu 22.04 (Linux) and make sure that the clang complier and git have been installed in your local device.
2) Make sure that the repository folder gets cloned to a designated folder in your local device.
3) Make sure that right files have been loaded, especially the header files, program files, and the Makefile.
4) Go to the designated folder and open up the terminal.
5) Once you are in the designated directory, enter the command: $ make.
6) The commands in the Makefile will make compling the header and program files in the "asgn6" directory easier.
7) There is one main executable called banhammer which reads words from a standard input.  It will identify any badspeak or oldspeak words and output appropriate punishment messages based on the severity of the situation.  It will also print all badspeak and oldspeak words that caused the punishment in the first.  If the statistics option was activated, only the statistics related to the parsing is printed instead of the warning messages and the badspeak or oldspeak words.
8) You could choose to modify badspeak.txt and newspeak.txt to filter out words of your own choosing.  There are already a few words sets in each file.
9) To run the executable, do: $ "file or standard input " | ./banhammer <-options->


## Command-line options for banhammer.c
- -t <-ht_size-> : The hash table size is set to <-ht_size->. If left alone, the default size will be set to 10000.
- -f <-bf_size-> : The bloom filter size is set to <-bf_size->. If left alone, the default size will be set to 2^19.
- -s : Enables the printing of statistics and suppresses the warning messages and offending words if any.
- -m : Enables the move-to-front rule for the hash table and any relating linked list data structures.
- -h : Displays program synopsis and usage.

## Deliverables 
- banhammer.c
- messages.h
- city.h
- city.c
- ht.h
- ht.c
- ll.h
- ll.c
- node.h
- node.c
- bf.h
- bf.c
- bv.h
- bv.c
- parser.h
- parser.c
- Makefile
- README.md
- badspeak.txt
- newspeak.txt


|Name|Email|
|----|-----|
|Nam Tran|natrtran@ucsc.edu|
