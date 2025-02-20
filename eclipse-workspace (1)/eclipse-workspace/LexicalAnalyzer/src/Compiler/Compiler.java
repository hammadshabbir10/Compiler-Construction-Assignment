package Compiler;
import java.io.*;
import java.util.List;

public class Compiler {
    public static void main(String[] args) {
        String filePath = "input.cpp"; // ✅ Ensure this file exists in the working directory
        File file = new File(filePath);

        // ✅ Check if file exists
        if (!file.exists()) {
            System.err.println("Error: File '" + filePath + "' not found!");
            return; // Exit if the file does not exist
        }

        StringBuilder code = new StringBuilder();

        // ✅ Read the C++ file as input
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                code.append(line).append("\n");
            }
        } catch (IOException e) {
            System.err.println("Error reading file: " + e.getMessage());
            return; // Exit if file reading fails
        }

        // ✅ Process C++ code from file
        SymbolTable symbolTable = new SymbolTable(); 
        LexicalAnalyzer lexer = new LexicalAnalyzer(symbolTable);
        List<Token> tokens = lexer.tokenize(code.toString());

        System.out.println("Tokens:");
        for (Token t : tokens) {
            System.out.println(t);
        }

        // ✅ Display Symbol Table
        System.out.println("\nSymbol Table:");
        symbolTable.display();
    }
}
