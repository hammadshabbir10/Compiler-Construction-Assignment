Compiler Construction Assignment NO 1


# CFG Processing Project
Introduction
This project focuses on processing a Context-Free Grammar (CFG) to make it suitable for LL(1) parsing. It includes functionalities for removing left recursion, performing left factoring, and computing the FIRST and FOLLOW sets for non-terminals.


---
# Approach
The grammar is read from a file where each line defines productions for a non-terminal.

Immediate and indirect left recursion is systematically removed.

Left factoring is performed to eliminate ambiguities in the grammar.

FIRST and FOLLOW sets are computed for all non-terminals using iterative methods.


---
# Features
Read and parse CFG from a text file.

Handle immediate and indirect left recursion.

Perform left factoring where needed.

Compute and display FIRST and FOLLOW sets.

Output the modified grammar after each transformation step.


---
# How to Run
Place the input grammar file in the project directory.



---
# Notes
The grammar must be properly formatted in the input file.
Empty productions are represented using a special symbol (e.g., # for epsilon).


---
# Conclusion
This project provides a complete workflow for transforming a CFG for LL(1) parsing by removing recursion, factoring the grammar, and generating the required parsing sets.



![Image](https://github.com/user-attachments/assets/dd849cc6-da8a-4848-995e-963a473aceb9)


# Left Factoring
 

![Image](https://github.com/user-attachments/assets/377152bb-9bf4-47af-b38f-56d78ad110bb)


# Left Recursion:

 
![Image](https://github.com/user-attachments/assets/1fa56ee0-c462-4c97-bcc8-e864f205add1)



# First & Follow


![Image](https://github.com/user-attachments/assets/f650054c-5b72-4f36-bc84-31e6c15bf302)


# Parsing Table:


![Image](https://github.com/user-attachments/assets/f6795aea-55c6-4313-8ce3-0ea0acddbe01)


# Parsing Stack with Error


![Image](https://github.com/user-attachments/assets/54a7480b-0e43-41e4-88cb-af78091e04a6)


# Successfully Parse


![Image](https://github.com/user-attachments/assets/320d6e58-a281-487b-b172-91a4d10719b7)




 

 

 

