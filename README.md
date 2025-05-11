

# JSON to Relational CSV Converter

This project takes JSON input, parses it using **Flex** (scanner) and **Yacc/Bison** (parser), constructs an **Abstract Syntax Tree (AST)**, and then generates one or more **CSV files** representing the structured data.

---

## 🔧 Setup and Installation on Ubuntu

To install Flex and Yacc/Bison:

### Install Flex:

sudo apt-get update
sudo apt-get install flex

### Install Bison:

sudo apt-get install byacc
sudo apt-get install bison
sudo apt-get install bison++
sudo apt-get install byacc-j

### Verify Installation:

flex --version
yacc --version

---

### Project Structure:

------------------------------------------------------------------------
| File/Folder   | Description                                          |
| ------------- | ---------------------------------------------------- |
| `scanner.l`   | Lexical analyzer for recognizing JSON tokens         |
| `parser.y`    | YACC/Bison grammar file for parsing JSON structure   |
| `ast.h`       | AST node structure and utility functions             |
| `tables.h`    | Functions to transform AST into relational tables    |
| `Makefile`    | Build configuration to compile the entire project    |
| `tests/`      | Folder containing test JSON input files              |
| `outputs/`    | Output directory for generated CSV files             |
| `json2relcsv` | Final compiled binary for JSON to CSV transformation |

---

### Complie Project 

make 



### How to USE!

# Generate CSV Output from JSON:

./json2relcsv --out-dir outputs/array_scalars tests/array_scalars.json
cat outputs/array_scalars/main.csv

![Image](https://github.com/user-attachments/assets/7844ffa2-c2be-49ac-afba-c09395756f01)


# Print Tokens:

./json2relcsv --print-tokens tests/array_objects.json

![Image](https://github.com/user-attachments/assets/7f19b77b-fc12-4f63-a2b8-4ddbfef5a633)


# Print AST tree

./json2relcsv --print-ast tests/complex.json

![Image](https://github.com/user-attachments/assets/8dfea09a-541a-4071-8303-f989ed84d11f)


# Print Error 

![Image](https://github.com/user-attachments/assets/ae0b7111-1a7e-47b7-a620-7bf8ca215050)


### Theory & Approach (Key Concepts)

# 1. Scanner (Flex - scanner.l)

    Identifies lexemes like {, }, [, ], :, ,, string, number, etc.
    Converts source input into tokens passed to the parser

# 2. Parser (Yacc/Bison - parser.y)

    Defines grammar rules for JSON
    Builds an Abstract Syntax Tree (AST) during parsing

# 3. AST (ast.h)

    Custom tree structure representing the JSON data hierarchy
    Nodes for objects, arrays, literals, etc.

# 4. Relational Table Generator (tables.h)

    Walks the AST and transforms hierarchical JSON into flat relational CSV format
    Each JSON object/array corresponds to a table

# 5. Makefile

    Automates the build process (flex + yacc + compilation)
    
Compiler Construction Assignment NO 1


![Image](https://github.com/user-attachments/assets/dd849cc6-da8a-4848-995e-963a473aceb9)

    
