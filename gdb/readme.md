# master_gdb_guide_version2

# Mastering GDB: The Ultimate Guide to Debugging with Detailed Explanations and Examples

This guide is a comprehensive, detailed resource for mastering GDB, the GNU Debugger, tailored for C/C++ developers. It expands on all GDB features, providing in-depth explanations, extensive options, and practical examples for each command. Whether you’re debugging segmentation faults, multi-threaded applications, or optimizing performance, this guide equips you with the knowledge and tools to become a GDB expert. Each section includes examples based on a sample program to illustrate real-world usage.

---

## Sample Program for Examples

The following program (`factorial.c`) will be used throughout for examples:

```c
#include <stdio.h>
#include <stdlib.h>
int global_var = 42;
unsigned int factorial(unsigned int n) {
    if (n == 0) return 1;
    int local_var = n * 2; // For demonstration
    return n * factorial(n - 1);
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }
    unsigned int num = atoi(argv[1]);
    unsigned int result = factorial(num);
    printf("Factorial of %u is %u\n", num, result);
    return 0;
}
```

Compile it:

```bash
$ gcc -g -o factorial factorial.c
```

---

# 1. Getting Started with GDB

## 1.1 Compiling for Debugging

To debug with GDB, compile your program with debugging symbols to include information like variable names, types, and line numbers.

**Command**:

```bash
$ gcc -g -o factorial factorial.c
```

**Options**:
- **`-g`**: Includes debugging symbols (symbol table, line numbers, variable types). Essential for mapping machine code to source code.
- *Example*: Without `-g`, GDB shows only memory addresses, not variable names.
- **`-ggdb`**: Generates GDB-specific debugging info for better compatibility.
- *Example*: `gcc -ggdb -o factorial factorial.c` ensures GDB can fully utilize debugging features.
- **`-O0`**: Disables optimizations to ensure debugging accuracy (default with `-g`).
- *Example*: `gcc -g -O0 -o factorial factorial.c` prevents code reordering that could confuse debugging.
- **`-Og`**: Enables optimizations that don’t interfere with debugging.
- *Example*: `gcc -g -Og -o factorial factorial.c` balances performance and debuggability.
- **`-Wall`**: Enables warnings to catch potential bugs during compilation.
- *Example*: `gcc -g -Wall -o factorial factorial.c` might warn about unused variables.
- **`-fsanitize=address`**: Adds AddressSanitizer to detect memory errors (e.g., buffer overflows).
- *Example*: `gcc -g -fsanitize=address -o factorial factorial.c` catches invalid memory access.
- **`-fno-omit-frame-pointer`**: Preserves frame pointers for accurate stack traces.
- *Example*: `gcc -g -fno-omit-frame-pointer -o factorial factorial.c` improves backtrace readability.
- **`-g3`**: Includes macro definitions for debugging preprocessor macros.
- *Example*: `gcc -g3 -o factorial factorial.c` allows inspecting macro expansions with `(gdb) info macro`.

**Example**:

```bash
$ gcc -g -Wall -fsanitize=address -o factorial factorial.c
$ ls -lh factorial
```

Output shows a larger binary due to debugging symbols.

**Tip**: Always use `-g` for debugging. Add `-fsanitize=address` for memory safety.

### 1.2 Starting GDB

Launch GDB to debug your program.

**Command**:

```bash
$ gdb ./factorial
```

**Options**:
- **`-q` (`--quiet`)**: Suppresses startup messages (e.g., version info).
- *Example*: `gdb -q ./factorial` starts GDB silently.
- **`-tui`**: Starts in Text User Interface mode, showing source code and command windows.
- *Example*: `gdb -tui ./factorial` opens a split-screen interface.
- **`-nh`**: Disables loading `~/.gdbinit` to avoid custom settings.
- *Example*: `gdb -nh ./factorial` ensures a clean GDB session.
- **`-ex <command>`**: Executes a GDB command at startup.
- *Example*: `gdb -ex "break main" ./factorial` sets a breakpoint on `main` immediately.
- **`--args`**: Specifies program arguments without running.
- *Example*: `gdb --args ./factorial 5` sets arguments, then `(gdb) run` uses them.

**Alternative**:
Load binary inside GDB:

```
(gdb) file ./factorial
```

**Example**:

```bash
$ gdb -tui --args ./factorial 5
(gdb) run
```

Starts GDB in TUI mode and runs with argument `5`.

### 1.3 GDB Command Line

GDB provides an interactive prompt:

```
(gdb)
```

**Features**:
- **Command completion**: Press `Tab` to auto-complete commands or symbols.
- *Example*: Type `bre` and press `Tab` to complete to `break`.
- **Repeat last command**: Press `Enter` to repeat the previous command.
- *Example*: After `(gdb) next`, press `Enter` to repeat `next`.
- **Edit command line**: Use `Ctrl-x Ctrl-a` for visual editing mode (if enabled).
- *Example*: Edit a long command like `break factorial if n == 3`.

**Example**:

```
(gdb) bre[Tab]
(gdb) break
```

## 1.4 Basic Commands

- **Run program**:
    
    ```
    (gdb) run [args]
    ```
    
    Starts execution, optionally with arguments or I/O redirection.
    
    - *Example*: `(gdb) run 5` runs with argument `5`.
    - *Example*: `(gdb) run < input.txt > output.txt` redirects I/O.
- **Quit GDB**:
    
    ```
    (gdb) quit [exit-code]
    ```
    
    Exits GDB, optionally setting an exit code.
    
    - *Example*: `(gdb) quit 0` exits cleanly.
- **Help**:
    
    ```
    (gdb) help [command]
    ```
    
    Shows help for commands or categories.
    
    - *Example*: `(gdb) help break` details breakpoint commands.
    - *Example*: `(gdb) help all` lists all commands.
- **Shell commands**:
    
    ```
    (gdb) shell <command>
    ```
    
    Runs shell commands from GDB.
    
    - *Example*: `(gdb) shell ls` lists directory contents.
- **Apropos**:
    
    ```
    (gdb) apropos <keyword>
    ```
    
    Searches help for commands matching a keyword.
    
    - *Example*: `(gdb) apropos breakpoint` lists breakpoint-related commands.

**Example**:

```
(gdb) help run
(gdb) shell pwd
(gdb) run 5
```

---

## 2. Breakpoints

Breakpoints pause execution at specific points for inspection.

### 2.1 Setting Breakpoints

- **By function**:
    
    ```
    (gdb) break factorial
    ```
    
    Pauses at the start of the `factorial` function.
    
    - *Example*: Sets a breakpoint at line 3 of `factorial.c`.
- **By line number**:
    
    ```
    (gdb) break factorial.c:5
    ```
    
    Pauses at line 5 in `factorial.c`.
    
- **By address**:
    
    ```
    (gdb) break *0x400538
    ```
    
    Pauses at a specific memory address (useful for low-level debugging).
    
- **By condition**:
    
    ```
    (gdb) break 5 if n == 3
    ```
    
    Pauses only when `n` equals 3.
    

**Options**:
- `break +N`: N lines after the current line.
- *Example*: `(gdb) break +2` sets a breakpoint two lines below the current line.
- `break -N`: N lines before the current line.
- `break file:line`: Specific file and line.
- *Example*: `(gdb) break factorial.c:10`.
- `break file:function`: Specific function in file.
- *Example*: `(gdb) break factorial.c:factorial`.
- `break if <condition>`: Conditional breakpoint with any C expression.
- *Example*: `(gdb) break 5 if local_var > 10`.

**Example**:

```
(gdb) break factorial
(gdb) break 5 if n == 3
(gdb) run 5
```

Stops when `factorial` is called with `n == 3`.

### 2.2 Temporary Breakpoints

- **Single-hit breakpoint**:
    
    ```
    (gdb) tbreak factorial
    ```
    
    Deletes after being hit once.
    
    - *Example*: `(gdb) tbreak 10` stops once at line 10.

### 2.3 Listing Breakpoints

- **View all**:
    
    ```
    (gdb) info breakpoints
    ```
    
    Shows breakpoint number, type, status, address, location, and hit count.
    
    - *Example*:
        
        ```
        Num     Type           Disp Enb Address            What
        1       breakpoint     keep y   0x000055555555518b in main at factorial.c:10
        2       breakpoint     keep y   0x0000555555555158 in factorial at factorial.c:3
        ```
        

### 2.4 Managing Breakpoints

- **Delete**:
    
    ```
    (gdb) delete [bpnumber]
    ```
    
    Removes specific or all breakpoints.
    
    - *Example*: `(gdb) delete 1` removes breakpoint 1.
    - *Example*: `(gdb) delete` removes all.
- **Disable/Enable**:
    
    ```
    (gdb) disable [bpnumber]
    (gdb) enable [bpnumber]
    ```
    
    Temporarily disables/enables breakpoints.
    
    - *Example*: `(gdb) disable 2` disables breakpoint 2.
    - *Example*: `(gdb) enable once 2` enables for one hit.
- **Ignore hits**:
    
    ```
    (gdb) ignore <bpnumber> <count>
    ```
    
    Skips breakpoint `count` times.
    
    - *Example*: `(gdb) ignore 1 5` skips breakpoint 1 five times.

### 2.5 Breakpoint Commands

Execute commands when a breakpoint is hit:

```
(gdb) command 1
  print n
  continue
  end
```

- *Example*: Prints `n` and continues each time breakpoint 1 is hit.
- **Option**: `silent` suppresses hit messages.
    
    ```
    (gdb) command 1
      silent
      print n
      continue
      end
    ```
    

### 2.6 Save Breakpoints

- **Save**:
    
    ```
    (gdb) save breakpoints breakpoints.gdb
    ```
    
    Saves breakpoints to a file.
    
- **Load**:
    
    ```
    (gdb) source breakpoints.gdb
    ```
    
    Restores saved breakpoints.
    

**Example**:

```
(gdb) break factorial
(gdb) break 5 if n == 3
(gdb) save breakpoints debug_factorial.gdb
(gdb) quit
$ gdb ./factorial
(gdb) source debug_factorial.gdb
```

---

## 3. Controlling Execution

Control how the program runs after hitting a breakpoint.

### 3.1 Commands

- **Continue**:
    
    ```
    (gdb) continue [count]
    ```
    
    Resumes execution until the next breakpoint or program end.
    
    - *Example*: `(gdb) continue 2` skips two breakpoints.
- **Next (step over)**:
    
    ```
    (gdb) next [count]
    ```
    
    Executes the next line, skipping function calls.
    
    - *Example*: `(gdb) next 3` executes three lines.
- **Step (step into)**:
    
    ```
    (gdb) step [count]
    ```
    
    Steps into functions.
    
    - *Example*: `(gdb) step` enters `factorial` from `main`.
- **Finish (step out)**:
    
    ```
    (gdb) finish
    ```
    
    Runs until the current function returns.
    
    - *Example*: `(gdb) finish` exits `factorial` back to `main`.
- **Until**:
    
    ```
    (gdb) until [location]
    ```
    
    Runs to a specific line or function end.
    
    - *Example*: `(gdb) until 7` runs to line 7.
- **Advance**:
    
    ```
    (gdb) advance <location>
    ```
    
    Runs to a specific location (line, function, or address).
    
    - *Example*: `(gdb) advance factorial.c:10`.

### 3.2 Reverse Debugging

Record and replay execution to trace bugs backward.
- **Start recording**:
`gdb   (gdb) record`
- **Reverse step**:
`gdb   (gdb) reverse-step`
Steps backward one line.
- **Reverse continue**:
`gdb   (gdb) reverse-continue`
Runs backward to the previous breakpoint.

**Example**:

```
(gdb) break factorial
(gdb) run 5
(gdb) record
(gdb) reverse-step
```

Steps backward through `factorial` calls.

**Tip**: Use `record stop` to end recording. Requires significant memory.

---

## 4. Passing Arguments

Pass command-line arguments to the program.

- **At startup**:
    
    ```bash
    $ gdb --args ./factorial 5
    (gdb) run
    ```
    
- **In GDB**:
    
    ```
    (gdb) set args 5
    (gdb) run
    ```
    
- **In GDB**:
    
    ```
    (gdb) run 5
    ```
    
- **In GDB**:
    
    ```
    (gdb) start 
    ```
    
- **Redirect I/O**:
    
    ```
    (gdb) run < input.txt > output.txt
    ```
    

**Example**:

```
(gdb) set args 4
(gdb) run
```

Runs `factorial` with input `4`.

---

## 5. Debugging Segmentation Faults

Handle crashes like segmentation faults.

### 5.1 Workflow

1. Compile with sanitizers:
    
    ```bash
    $ gcc -g -fsanitize=address -o segfault_demo segfault_demo.c
    ```
    
2. Run in GDB:
    
    ```
    (gdb) run
    ```
    
3. On segfault:
    
    ```
    (gdb) backtrace full
    ```
    
    Shows stack with variable values.
    
    - *Example*:
        
        ```
        Program started...
        
        Program received signal SIGSEGV, Segmentation fault.
        0x0000555555555154 in crash_me () at segfault_demo.c:6
        6       *ptr = 42;
        ```
        
        ### **B. Get the backtrace**
        
        ```
        gdb
        CopyEdit
        (gdb) backtrace
        
        ```
        
        Example output:
        
        ```
        less
        CopyEdit
        #0  crash_me () at segfault_demo.c:6
        #1  0x0000555555555169 in intermediate_function () at segfault_demo.c:10
        #2  0x0000555555555178 in main () at segfault_demo.c:15
        
        ```
        
        - **#0** → The exact line that caused the crash (`ptr = 42;`)
        - **#1** → Who called it (`intermediate_function`)
        - **#2** → Who called that (`main`)
        
        ---
        
4. Switch frame:
    
    ```
    (gdb) frame 0
    ```
    
5. Inspect:
    
    ```
    (gdb) print ptr
    $1 = (int *) 0x0
    ```
    

### 5.2 Source Code Viewing

- **List code**:
    
    ```
    (gdb) list [location]
    ```
    
    Options: `list 10`, `list main`, `list factorial.c:5`, `list 1,10`.
    
    - *Example*: `(gdb) list factorial` shows `factorial` function code.
- **Set list size**:
    
    ```
    (gdb) set listsize 15
    ```
    
    Changes lines displayed per `list`.
    
- **Show current line**:
    
    ```
    (gdb) list *
    ```
    

**Example**:

```
(gdb) list 5
(gdb) set listsize 20
(gdb) list main
```

---

## 6. Inspecting Data

Examine variables, memory, and types.

### 6.1 Printing Variables

- **Basic print**:
    
    ```
    (gdb) print <expression>
    ```
    
    Evaluates any C expression.
    
    - *Example*: `(gdb) print n` shows `n`’s value.
    - *Example*: `(gdb) print n + 5` computes `n + 5`.
- **Format options**:

```
(gdb) print/x n  # Hex
(gdb) print/d n  # Decimal
(gdb) print/c n  # Char
(gdb) print/t n  # Binary
```

- *Example*: `(gdb) print/x global_var` shows `0x2a`.

### Here's a quick reference of common format specifiers in GDB:

- `/x` → Hexadecimal
- `/d` → Signed decimal
- `/u` → Unsigned decimal
- `/o` → Octal
- `/t` → Binary
- `/c` → Character
- `/f` → Floating-point
- `/s` → String
- **Address**:
    
    ```
    (gdb) print &n
    ```
    
- **Size**:
    
    ```
    (gdb) print sizeof(n)
    ```
    

**Example**:

```
(gdb) print num
$1 = 42
(gdb) print pi
$2 = 3.14159
(gdb) print ch
$3 = 65 'A'
```

### **Printing strings**

```
gdb
CopyEdit
(gdb) print str
$8 = "Hello, GDB!"

```

If you want to see memory as bytes:

```
gdb
CopyEdit
(gdb) x/12bx str

```

---

### **Printing arrays**

```
gdb
CopyEdit
(gdb) print arr
$9 = {10, 20, 30, 40, 50}

```

Partial array:

```
gdb
CopyEdit
(gdb) print arr[0]
$10 = 10
(gdb) print arr[0]@3
$11 = {10, 20, 30}

```

---

### **Printing pointers**

```
gdb
CopyEdit
(gdb) print ptr
$12 = (int *) 0x7fffffffe4c0
(gdb) print *ptr
$13 = 10

```

Pointer arithmetic:

```
gdb
CopyEdit
(gdb) print *(ptr+2)
$14 = 30

```

---

### **Printing structs**

```
gdb
CopyEdit
(gdb) print p1
$15 = {name = "Alice", age = 30}

```

Specific field:

```
gdb
CopyEdit
(gdb) print p1.age
$16 = 30

```

---

### **Printing expressions**

```
gdb
CopyEdit
(gdb) print num + 5
$17 = 47
(gdb) print pi * 2
$18 = 6.28318

```

---

### 6.2 Examining Memory

- **Examine**:
    
    ```
    (gdb) x/<nuf> <address>
    ```
    
    - `n`: Number of units.
    - `u`: Unit size (`b`=byte, `h`=halfword, `w`=word, `g`=giant).
    - `f`: Format (`x`=hex, `d`=decimal, `s`=string, `i`=instruction).
    - *Example*: `(gdb) x/4xb &global_var` shows 4 bytes in hex.
    - *Example*: `(gdb) x/s argv[1]` prints argument as a string.
    - *Example*: `(gdb) x/5i $pc` shows 5 instructions.

**Example**:

```
(gdb) x/4xb &global_var
0x55555555801c: 0x2a 0x00 0x00 0x00
```

### 🔍 Basic Syntax

```
x /[N][FMT][SIZE] ADDRESS

```

Where:

- `N` = Number of units to display (optional)
- `FMT` = Display format (`x`, `d`, `u`, `t`, `f`, `c`, `s`, `i`)
- `SIZE` = Unit size (`b` = byte, `h` = halfword/2 bytes, `w` = word/4 bytes, `g` = giant/8 bytes)
- `ADDRESS` = Address or variable name to examine

---

### 📘 Format Specifiers

- `x` = hexadecimal (default)
- `d` = signed decimal
- `u` = unsigned decimal
- `t` = binary
- `f` = floating point
- `c` = character
- `s` = string
- `i` = machine instructions (disassembly)

---

### 📏 Size Specifiers

- `b` = 1 byte
- `h` = 2 bytes
- `w` = 4 bytes
- `g` = 8 bytes

---

### ✅ Examples

```
(gdb) x/4xb &var         # Show 4 bytes at var in hex
(gdb) x/10d array        # Show 10 integers from array in decimal
(gdb) x/s str            # Show string at str
(gdb) x/i $pc            # Disassemble instruction at program counter
(gdb) x/2xw 0x601040     # Show 2 words (4 bytes each) at address 0x601040
(gdb) x/8cb ptr          # Show 8 characters starting at ptr

```

---

### 3. Examples with `print_demo.c`

### **A. Look at integer in memory**

```
gdb
CopyEdit
(gdb) x/wd &num
0x7fffffffe4ac:  42

```

- `/w` → read 4 bytes at a time
- `d` → display as signed decimal

---

### **B. See raw hex**

```
gdb
CopyEdit
(gdb) x/wx &num
0x7fffffffe4ac:  0x0000002a
```

---

### **C. View multiple integers in an array**

```
gdb
CopyEdit
(gdb) x/5wd arr
0x7fffffffe490:  10  20  30  40  50
```

---

### **D. View pointer target**

```
gdb
CopyEdit
(gdb) print ptr
$1 = (int *) 0x7fffffffe490
(gdb) x/3wd ptr
0x7fffffffe490:  10  20  30
```

---

### **E. View characters in a string**

```
gdb
CopyEdit
(gdb) x/s str
0x7fffffffe460: "Hello, GDB!"
```

Raw bytes:

```
gdb
CopyEdit
(gdb) x/12cb str
0x7fffffffe460:  72 'H' 101 'e' 108 'l' 108 'l' 111 'o' 44 ',' 32 ' ' 71 'G' 68 'D' 66 'B' 33 '!'
```

---

### **F. Dump struct contents**

```
gdb
CopyEdit
(gdb) x/5wx &p1
0x7fffffffe470:  0x656c6941 0x00000063 0x00000000 0x0000001e 0x00000000
```

- That’s `"Alice"` in ASCII in memory + padding + `age=30`.

---

### **G. Disassemble instructions with `x`**

If we want to see machine code from `main`:

```
gdb
CopyEdit
(gdb) x/10i main
```

Shows the **next 10 instructions** from `main`.

---

### **H. Memory hex dump**

```
gdb
CopyEdit
(gdb) x/20bx arr
```

Shows 20 bytes starting from `arr` in hex, byte by byte.

## 6.3 Type Information

- **Variable type**:
    
    ```
    (gdb) ptype <expression>
    ```
    
    - *Example*: `(gdb) ptype n` shows `type = unsigned int`.
- **What is**:
    
    ```
    (gdb) whatis n
    ```
    
    - *Example*: `(gdb) whatis global_var` shows `type = int`.

### 6.4 Automatic Display

- **Set display**:
    
    ```
    (gdb) display/x n
    ```
    
    Shows `n` in hex at every stop.
    
- **List displays**:
    
    ```
    (gdb) info display
    ```
    
- **Remove**:
    
    ```
    (gdb) undisplay <number>
    ```
    

**Example**:

```
(gdb) display n
(gdb) step
1: n = 4
```

### 6.5 Modifying Variables

- **Set value**:
    
    ```
    (gdb) set variable n = 10
    ```
    
    - *Example*: Changes `n` to 10.
- **Patch memory**:
    
    ```
    (gdb) set {int}0x55555555801c = 100
    ```
    
    - *Example*: Sets `global_var` to 100.

---

### Display

Alright — let’s cover GDB’s **`display`** command, which is like telling GDB:

*"Hey, automatically show me this thing every time the program stops."*

It’s convenient for **tracking variables, expressions, or memory** without having to keep typing `print` each time.

---

### 1. Example program: `display_demo.c`

```c
#include <stdio.h>

int main() {
    int counter = 0;

    for (int i = 0; i < 5; i++) {
        counter += i;
        printf("i = %d, counter = %d\n", i, counter);
    }

    return 0;
}

```

---

### 2. Compile

```bash
gcc -g -o display_demo display_demo.c
```

---

### 3. Demo in GDB

```bash
gdb ./display_demo
```

Set a breakpoint inside the loop:

```
(gdb) break 6
(gdb) run
```

---

### **A. Basic usage**

```
(gdb) display i
(gdb) display counter
```

Now, **every time the program stops** (breakpoint, step, next), GDB will automatically print:

```
1: i = 0
2: counter = 0
```

The number before each variable (`1:`, `2:`) is its **display number**.

---

### **B. Display with expressions**

```
(gdb) display counter * 2
```

Shows computed values automatically.

---

### **C. Change format**

```
(gdb) display/x counter
```

Displays in **hexadecimal** every time.

Formats:

- `/d` → decimal
- `/x` → hex
- `/t` → binary
- `/o` → octal
- `/c` → char
- `/f` → float

---

### **D. Display memory**

```
(gdb) display/xw &counter

```

Shows `counter`’s memory location in hex, word-sized.

---

### **E. See what’s currently displayed**

```
(gdb) info display

```

---

### **F. Stop displaying something**

```
(gdb) undisplay 2

```

Removes display #2 from the list.

---

### **G. Disable & re-enable**

```
(gdb) disable display 1
(gdb) enable display 1

```

This lets you temporarily turn off a watch without deleting it.

---

## Why `display` is useful

- Great for **tracking variables over time** (loops, recursion).
- Saves typing — you don’t need to keep running `print`.
- Works well with **`next`** or **`step`** to watch changes happen live.

---

If you want, I can extend this into a **loop + display** example where we watch multiple variables and even **array elements** update automatically during stepping — feels like a mini live dashboard in GDB.

## 7. Stack Frames

Inspect and navigate the call stack.

### 7.1 Viewing Stack

- **Backtrace**:
    
    ```
    (gdb) backtrace [n]
    ```
    
    Shows stack frames.
    
    - *Example*: `(gdb) bt full` includes variable values.
    - *Example*: `(gdb) bt -5` shows last 5 frames.
- **Thread-specific**:
    
    ```
    (gdb) thread apply all bt
    ```
    

**Example**:

```
(gdb) break factorial
(gdb) run 5
(gdb) bt
#0  factorial (n=3) at factorial.c:3
#1  factorial (n=4) at factorial.c:7
#2  factorial (n=5) at factorial.c:7
#3  main (argc=2, argv=...) at factorial.c:12
```

### 7.2 Navigating Frames

- **Select frame**:
    
    ```
    (gdb) frame <number>
    ```
    
    - *Example*: `(gdb) frame 1` switches to `factorial (n=4)`.
- **Up/Down**:
    
    ```
    (gdb) up [n]
    (gdb) down [n]
    ```
    
    - *Example*: `(gdb) up` moves to the caller frame.

### 7.3 Frame Information

- **Frame details**:
    
    ```
    (gdb) info frame [number]
    ```
    
    Shows frame address, PC, and variables.
    
- **Locals**:
    
    ```
    (gdb) info locals
    ```
    
    - *Example*: `(gdb) info locals` shows `local_var = 6`.
- **Arguments**:
    
    ```
    (gdb) info args
    ```
    
    - *Example*: `(gdb) info args` shows `n = 3`.
- **Variables**:
    
    ```
    (gdb) info variables
    ```
    
    Lists global/static variables.
    

---

## `until` and `advance`

Alright — `until` and `advance` in GDB are both **execution control commands**, but they work a little differently.

Let’s go over them with a clear example so you can see when to use each.

---

## 1. Quick explanation

| Command | Purpose |
| --- | --- |
| **`until`** | Run until the current function returns **OR** until reaching a specified location *in the same frame*. |
| **`advance`** | Run until a **specific line or address**, stopping exactly there (even if it’s in a different function). |

---

## 2. Example program: `until_advance_demo.c`

```c
#include <stdio.h>

void step3() {
    printf("Step 3 start\n");
    printf("Step 3 middle\n");
    printf("Step 3 end\n");
}

void step2() {
    printf("Step 2 start\n");
    step3();
    printf("Step 2 end\n");
}

void step1() {
    printf("Step 1 start\n");
    step2();
    printf("Step 1 end\n");
}

int main() {
    step1();
    return 0;
}

```

---

## 3. Compile

```bash
gcc -g -o until_advance_demo until_advance_demo.c

```

---

## 4. Demo in GDB

```bash
gdb ./until_advance_demo

```

### **A. Using `until`**

```
(gdb) break step2
(gdb) run

```

When you stop at the first line of `step2()`:

```
(gdb) until

```

- This runs until **the end of `step2`** (right before it returns to `step1`).
- If you give a **line number**:

```
(gdb) until 16

```

it will run until line 16 **inside the same frame**.

⚠️ If you are inside a loop, `until` can skip ahead to after the loop if given a target line.

---

### **B. Using `advance`**

Suppose you’re in `main()` and want to jump directly to the first line of `step3()` without hitting any other breakpoints:

```
(gdb) break main
(gdb) run
(gdb) advance step3

```

- Execution runs forward and stops **exactly at `step3()`**, regardless of how many functions it needs to pass through.

You can also give it a **line number**:

```
(gdb) advance 22

```

This moves execution forward to line 22 in the current source file.

---

## 5. Key difference in behavior

- `until` → good for **finishing the current function** or **skipping part of a loop**.
- `advance` → good for **jumping ahead to a specific location**, skipping all in-between code.

---

If you want, I can now create a **loop example** where `until` skips to after the loop and `advance` jumps into a function inside the loop — that shows the subtle difference much more clearly.

## 8. CPU Registers

Inspect and modify CPU registers.

- **All registers**:
    
    ```
    (gdb) info registers
    ```
    
    Shows all registers (e.g., `rax`, `rbx`).
    
- **Specific register**:
    
    ```
    (gdb) info registers rax
    ```
    
- **Modify**:
    
    ```
    (gdb) set $rax = 0x1234
    ```
    

**Example**:

```
(gdb) info registers
rax            0x555555555158  93824992235608
```

---

## 9. Watchpoints

Pause when a variable is read or written.

### 9.1 Setting Watchpoints

- **Write**:
    
    ```
    (gdb) watch global_var
    ```
    
    Triggers when `global_var` is modified.
    
- **Read**:
    
    ```
    (gdb) rwatch global_var
    ```
    
- **Read/write**:
    
    ```
    (gdb) awatch global_var
    ```
    

**Example**:

```
(gdb) watch global_var
(gdb) run 5
Hardware watchpoint 1: global_var
```

**Note**: Variable must be in scope. Set a breakpoint first if needed.

### 9.2 Managing Watchpoints

- **List**:
    
    ```
    (gdb) info breakpoints
    ```
    
- **Disable**:
    
    ```
    (gdb) disable <number>
    ```
    

---

## 10. GDB Text User Interface (TUI)

Visual interface for source, assembly, and registers.

- **Start**:
    
    ```bash
    $ gdb -tui ./factorial
    ```
    
- **Toggle**:
    
    ```
    (gdb) tui enable
    (gdb) tui disable
    ```
    

### 10.1 TUI Layouts

- **Source**:
    
    ```
    (gdb) layout src
    ```
    
    Shows source code.
    
- **Assembly**:
    
    ```
    (gdb) layout asm
    ```
    
    Shows assembly code.
    
- **Split**:
    
    ```
    (gdb) layout split
    ```
    
    Shows source and assembly.
    
- **Registers**:
    
    ```
    (gdb) layout regs
    ```
    
    Shows registers and source.
    

### 10.2 TUI Commands

- **Refresh**:
`Ctrl-l`
- **Focus**:
`Ctrl-x o` (toggles source/command).
- **Scroll**:
`Ctrl-u` (up), `Ctrl-d` (down).

**Example**:

```
(gdb) layout split
(gdb) break main
(gdb) run 5
```

---

## **`call` Command in GDB**

### **Purpose**

The `call` command allows you to **invoke a function** in your program while it is paused in the debugger.

You can:

- Call your own functions (defined in the program).
- Call library functions (like `printf`, `puts`, `malloc`).
- Pass arguments to these functions.

---

## **Example Program: `call_example.c`**

```c
#include <stdio.h>

int multiply(int a, int b) {
    return a * b;
}

void print_message(const char *msg) {
    printf("Message: %s\n", msg);
}

int main() {
    int x = 5, y = 10;
    printf("Before function call in code\n");

    // Breakpoint will be here
    printf("x = %d, y = %d\n", x, y);

    printf("After breakpoint\n");
    return 0;
}

```

---

## **Compiling for GDB**

```bash
gcc -g -o call_example call_example.c

```

---

## **GDB Session**

```
$ gdb ./call_example
(gdb) break main
Breakpoint 1 at 0x1149: file call_example.c, line 11.

(gdb) run
Starting program: ./call_example

Breakpoint 1, main () at call_example.c:11
11        printf("Before function call in code\n");

# Step to the second printf
(gdb) until 14
14        printf("x = %d, y = %d\n", x, y);

# Call multiply directly in GDB
(gdb) call multiply(3, 4)
$1 = 12

# Call print_message without changing program source
(gdb) call print_message("Hello from GDB!")
Message: Hello from GDB!
$2 = void

# Modify variables via call
(gdb) set variable x = multiply(7, 2)
(gdb) print x
$3 = 14

```

---

## 11. Logging

Save GDB output to a file.

- **Enable**:
    
    ```
    (gdb) set logging on
    ```
    
    Saves to `gdb.txt`.
    
- **Disable**:
    
    ```
    (gdb) set logging off
    ```
    
- **Set file**:
    
    ```
    (gdb) set logging file debug.log
    ```
    
- **Redirect**:
    
    ```
    (gdb) set logging redirect on
    ```
    
    Sends output only to file.
    

**Example**:

```
(gdb) set logging file factorial.log
(gdb) set logging on
(gdb) bt
(gdb) set logging off
```

---

## 12. Attaching Processes

Debug a running process.

- **Attach**:
    
    ```
    (gdb) attach <pid>
    ```
    
    - *Example*: `(gdb) attach 12345` attaches to process 12345.
- **Detach**:
    
    ```
    (gdb) detach
    ```
    
- **Find PID**:
    
    ```bash
    $ ps aux | grep factorial
    ```
    

**Example**:

```bash
$ ./factorial 5 &$ ps aux | grep factorial
$ gdb ./factorial
(gdb) attach 12345
```

---

## 13. Disassembling Code

View assembly code.

- **Disassemble**:
    
    ```
    (gdb) disassemble main
    ```
    
    Options: `/m` (with source), `/r` (raw bytes).
    
    - *Example*: `(gdb) disassemble /m factorial` shows source and assembly.
- **Next instruction**:
    
    ```
    (gdb) x/i $pc
    ```
    

**Example**:

```
(gdb) disassemble factorial
Dump of assembler code for function factorial:
   0x0000555555555158 <+0>:     push   %rbp
   ...
```

---

## 🔹 Option 1: Print all general-purpose registers

```
gdb
CopyEdit
(gdb) info registers

```

This shows values of:

- General-purpose registers: `rax`, `rbx`, `rcx`, `rdx`, `rsi`, `rdi`, `rsp`, `rbp`, `rip`, etc.
- Flags
- Instruction pointer
- Stack pointer, etc.

---

## 🔹 Option 2: Print a specific register

```
gdb
CopyEdit
(gdb) print $rbp
(gdb) print $rip
(gdb) print $rsp

```

Registers are accessed using a **dollar sign (`$`)**.

Example:

```
gdb
CopyEdit
(gdb) p/x $rbp
$1 = 0x7fffffffda80

```

You can format the output:

- `/x` → hexadecimal
- `/d` → decimal
- `/t` → binary

---

## 🔹 Option 3: Use `x` (examine memory) at the register address

Once you know the value of a register, you can use it to inspect memory:

```
gdb
CopyEdit
(gdb) x/4x $rsp

```

This will show 4 words at the stack pointer.

## 14. Starting Execution

- **Start**:
    
    ```
    (gdb) start
    ```
    
    Sets a temporary breakpoint at `main` and runs.
    
- **With args**:
    
    ```
    (gdb) start 5
    ```
    

**Example**:

```
(gdb) start 5
Breakpoint 1 at 0x55555555518b: file factorial.c, line 10.
```

---

## 15. Signal Handling

Control how GDB handles signals (e.g., `SIGSEGV`).

- **List signals**:
    
    ```
    (gdb) info signals
    ```
    
- **Handle signal**:
    
    ```
    (gdb) handle SIGSEGV stop print
    ```
    
    Options: `stop`, `nostop`, `print`, `noprint`, `pass`, `nopass`.
    

**Example**:

```
(gdb) handle SIGSEGV nostop
(gdb) run
```

---

## 16. Advanced Debugging

### 16.1 Core Dumps

Debug crashes using core dumps.

- **Debug core**:
    
    ```bash
    $ gdb ./factorial core
    ```
    
- **Generate core**:
    
    ```bash
    $ ulimit -c unlimited
    $ ./factorial 5
    ```
    

**Example**:

```bash
$ gdb ./factorial core
(gdb) bt
```

### 16.2 Multi-Threading

- **List threads**:
    
    ```
    (gdb) info threads
    ```
    
- **Switch thread**:
    
    ```
    (gdb) thread <id>
    ```
    
- **Apply command**:
    
    ```
    (gdb) thread apply all bt
    ```
    

**Example** (for a multi-threaded program):

```
(gdb) info threads
(gdb) thread 2
(gdb) bt
```

### 16.3 Python Scripting

Automate tasks with Python.

- **Execute Python**:
    
    ```
    (gdb) python print(gdb.parse_and_eval("n"))
    ```
    
- **Load script**:
    
    ```
    (gdb) source script.py
    ```
    

**Example Script** (`pretty_print.py`):

```python
import gdb
class PrettyPrint(gdb.Command):
    def __init__(self):
        super(PrettyPrint, self).__init__("pp", gdb.COMMAND_USER)
    def invoke(self, arg, from_tty):
        val = gdb.parse_and_eval(arg)
        print(f"Value of {arg}: {val}")
PrettyPrint()
```

**Usage**:

```
(gdb) source pretty_print.py
(gdb) pp n
Value of n: 5
```

### 16.4 GDB Dashboard

Enhance GDB with a dashboard:

```bash
$ wget -P ~ git.io/.gdbinit
```

Customizes GDB with sections for registers, stack, and source.

---

## 17. Customization

### 17.1 .gdbinit

Create `~/.gdbinit` for custom settings:

```
set pagination off
set print pretty on
set print array on
define hook-stop
  info locals
  info args
end
```

**Options**:
- `set print pretty on`: Formats structures.
- `set print array on`: Formats arrays cleanly.
- `set history save on`: Saves command history to `~/.gdb_history`.
- `set confirm off`: Disables confirmation prompts.

### 17.2 Aliases

Define shortcuts in `.gdbinit`:

```
define bp
  break $arg0
end
```

**Example**:

```
(gdb) bp main
```

---

## 18. Practical Debugging Session

**Steps**:
1. Compile:
`bash    $ gcc -g -fsanitize=address -o factorial factorial.c`
2. Start GDB:
`bash    $ gdb -tui ./factorial`
3. Set breakpoints:
`gdb    (gdb) break main    (gdb) break factorial if n == 3    (gdb) watch global_var`
4. Run:
`gdb    (gdb) run 5`
5. Inspect:
`gdb    (gdb) display n    (gdb) step    (gdb) print local_var    $1 = 6`
6. Backtrace:
`gdb    (gdb) bt full`
7. Modify:
`gdb    (gdb) set variable n = 2`
8. Continue:
`gdb    (gdb) continue`

---

## 19. Tips for Mastery

- **Practice**: Debug small programs to master stack frames, registers, and memory.
- **Use TUI/Dashboard**: Improves visibility of code and state.
- **Learn Python scripting**: Automates complex debugging tasks.
- **Reverse debugging**: Tracks bugs backward in time.
- **Read GDB manual**:
    
    ```bash
    $ info gdb
    ```
    
- **Experiment with core dumps**: Simulate crashes for practice.
- **Join communities**: Stack Overflow, GDB mailing lists.
- **Use `gdb --tutor`**: Interactive tutorial (if available).

---

This guide provides exhaustive coverage of GDB’s features with detailed explanations and examples. Practice with the provided program, customize your GDB setup, and explore advanced features like scripting and reverse debugging to master GDB and tackle any debugging challenge.