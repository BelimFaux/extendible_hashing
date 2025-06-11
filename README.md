# Extendible Hashing

This is a C++ Implementation of an Extendible Hashing Set.  
It was originally written for the ADS-Course of University of Vienna.

## Setup

To try the Set out, you can use the provided playground program.
To run the Program, compile the file `eh_playground` using CMake and run it:

```bash
mkdir build && cd build
cmake .. && make
./eh_playground

```

You can also set the program to nonverbose by setting the verbose flag

```bash
./eh_playground -v
./eh_playground --verbose
```

For more Information run

```bash
./eh_playground -h
./eh_playground --help
```

## Commands

The Program launches into a prompt, from there you have multiple commands to modify or inspect the set:

- i - insert elements in set
- r - remove elements from set
- f - find elements in set
- l - list all elements in set (using foreach loop)
- s - show size of set
- c - clear set
- p - print current set
- h - show help page
- q (or EOF) - quit

Commands like insert, remove and find, that require more input will launch a second prompt where you can type in as much elements as you would like, seperated by spaces:

```
input> 1 2 3 4 5
Extendible Hashing <j,16>, d = 0, nD = 1, sz = 5
0 --> [l = 0, offset = 1, arrsz = 5 | 1 2 3 4 5 ]
```

After executing commands that alter the set the set will be printed, if the Program isn't set to nonverbose (see Output)

## Output

If the Program isn't run nonverbose, after every change to the Set, it will be printed out (equivalent to running `p`).  
The Format for printing is definde by the `dump()` method, and includes every directory, and the bucket that it points to (-> if its the only (or first) Directory pointing to that Bucket, or ~> followed by the first Directory, pointing to that Bucket else) as well as the Elements inside the Buckets and info like size, offset, local/global depth etc.

For example a Set like this:

![https://media.geeksforgeeks.org/wp-content/uploads/20190803222615/hash73.png](https://media.geeksforgeeks.org/wp-content/uploads/20190803222615/hash73.png "Example")

might look like this:  
Extendible Hashing <j,4>, d = 2, nD = 4, sz = 9  
0 --> [l = 2, offset = 4, arrsz = 3 | 16 4 24 ]  
1 --> [l = 1, offset = 2, arrsz = 3 | 31 7 9 ]  
2 --> [l = 2, offset = 4, arrsz = 3 | 6 22 10 ]  
3 ~~> 1 --> [l = 1, offset = 2, arrsz = 3 | 31 7 9 ]
