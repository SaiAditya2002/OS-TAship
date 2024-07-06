# OS Assignment

# Project structure

```bash
├── bin # Executable files will be generated here.
├── include # Header files.
├── Makefile # Makefile for building the project.
├── README.md # README
└── src # Source code.
```

# Running the code

To run the code with and without the provided Makefile, you can follow these instructions:

## Running the code with the Makefile

1. **Compile the code**:

   - Open your terminal.
   - Navigate to the root directory of your project, where the Makefile is located.
   - To compile the code, simply run the following command:

     ```bash
     make all
     ```

   This command will compile all the source code files in the `src` directory and generate executable files in the `bin` directory.

2. **Run a specific program**:

   - After running `make`, you can execute a specific program by specifying its name. For example, if you have a file called `my_program.c`, run it using:

     ```bash
     make run-my_program # For example, here to run client.c you have to use make run-client
     ```

3. **Clean the build**:

   - To remove all the generated binary files and start fresh, you can run:

     ```bash
     make clean
     ```

   This command will delete the `bin` directory and its contents.

## Running the code without the Makefile

If you prefer to compile and run the code manually without the Makefile, follow these steps:

1. **Compile the code manually**:

   - Open your terminal.
   - Navigate to the root directory of your project.
   - Compile each source file individually using the following command (replace `file_name.c` with your actual source file):

     ```bash
     gcc -Wall -Wextra -Iinclude -Wconversion -pedantic-errors -Werror -o file_name src/file_name.c
     ```

   Repeat this step for each source file in your project. This will compile the source code and generate executable files in the project root.

2. **Run a specific program**:

   - After manually compiling the code, you can execute a specific program by specifying its name. For example:

     > [!IMPORTANT]  
     > The binaries when compiled manually are supposed to be run from the project root directory.

     ```bash
     ./my_program
     ```