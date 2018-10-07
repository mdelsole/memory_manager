# memory_manager

Progam to emulate an operating system’s responsibility of allocating memory to certain programs. On startup, the program 
will have  32 pages of contiguous, unused memory. Each page will be 4 KB long. It will allow the users to “run” programs 
that require chunks of this memory for some period of time. It will also allow the users to “kill” programs that are in 
progress. The pages used by programs that are killed can then be re-used for future programs.

The purpose of this project was two-fold. First, I wanted practice with linked lists and pointers. Secondly, I wanted to look 
at different algorithms of allocating memory because I'm interested in quantum computing. I wanted to know how operating 
systems worked on a basic level so I could hypothesize how it would be done on a quantum level.
