# Assembler-SemiCompiler
This project is an assembler for a 15-bit assembly language, designed as part of a course on systems programming.
<br>
The assembler processes assembly language files, translating them into machine code.
<br>
The project adheres to the guidelines set forth in the course, with an emphasis on clear, modular, and maintainable code.


## Project Structure

The project is organized into several modules, each responsible for a specific aspect of the assembly process:

- **Main Program (`main.c`)**: Orchestrates the entire assembly process, including input validation, file processing, and output generation.
- **First Pass (`first_pass.c`)**: Scans the assembly file to collect labels, process instructions, and identify errors.
- **Second Pass (`second_pass.c`)**: Completes the assembly process by resolving addresses and generating the final machine code.
- **Preprocessor (`preprocess.c`)**: Handles macro expansion and other preprocessing tasks.
- **Utils (`utils.c`)**: Provides utility functions used across different modules.
- **Logger (`logger.c`)**: Manages logging of information, warnings, and errors throughout the assembly process.
- **Hash Map (`hash_map.c`)**: Implements a hash map data structure for storing symbols and macros.
- **Output (`output.c`)**: Manages the creation of output files containing the assembled machine code.
- **Validators (`validators.c`)**: Contains functions to validate various inputs and conditions during the assembly process.

---

## How to Use

### Compilation

- Use the provided `Makefile` to compile the project. The `Makefile` is configured to use the GCC compiler with specific flags (`-Wall -ansi -pedantic`) to ensure code quality and standard compliance.
- Run the following command to compile:
  ```bash
  make
  ```

## Running the Assembler

- After compilation, the executable can be used to process assembly files.
- The command format is:
  ```bash
  ./assembler file1 file2 ... fileN
    ```
- Each file should be an assembly source file with the .as extension.


## Output
For each processed file, the assembler generates several output files:
- **(`.ob`)**: The object file containing the machine code.
- **(`.ent`)**: Contains the entry labels (if any).
- **(`.ext`)**: Contains the external labels (if any).


## Key Functions

- **`main(int argc, char** argv)`**: The entry point of the program. Validates inputs and iterates through each input file, processing them through multiple passes.
- **`process_file(char* asm_file_name)`**: Manages the entire assembly process for a given file, including preprocessing, first pass, second pass, and output file generation.
- **`reset_assembler()`**: Resets the assembler's internal state before processing a new file.
- **`prep_second_pass(FILE *file_handle)`**: Prepares the assembler for the second pass by resetting the file pointer and cleaning up the line buffer.



## Error Handling

The assembler uses a robust error-handling mechanism, with specific status codes defined in `status_codes.h`. Errors are logged using the `logger.c` module, ensuring that issues are reported clearly, helping in debugging and understanding the cause of any failures.

---

## Code Quality and Style

The project follows best practices in C programming, including:

- **Modular Design**: Code is organized into modules, each with a clear responsibility.
- **Consistent Naming Conventions**: Functions, variables, and files follow a consistent naming scheme, improving readability.
- **Comprehensive Logging**: All major actions are logged, with both info and error levels, facilitating easy debugging.
- **Memory Management**: Dynamic memory allocations are carefully managed, with appropriate cleanup to prevent memory leaks.

---

## Contributions

This project was developed as a learning exercise in systems programming. Contributions to this project are encouraged, particularly in the areas of optimization and extending functionality to support additional assembly features.

---

### End of README

This document provides an overview of the project's structure, usage, and key functions. For detailed information on the assembly process and specific implementation details, refer to the inline comments and documentation within the code files.

