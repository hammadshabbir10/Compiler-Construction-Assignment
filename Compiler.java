package Compiler;
import java.io.*;
import java.util.List;

public class Compiler {
    public static void main(String[] args) {
        String filePath = "src/Compiler/input.cpp"; 
        File file = new File(filePath);

        // ✅ Check if file exists
        if (!file.exists()) {
            System.err.println("Error: File '" + filePath + "' not found!");
            return; 
        }

        StringBuilder code = new StringBuilder();

        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                code.append(line).append("\n");
            }
        } catch (IOException e) {
            System.err.println("Error reading file: " + e.getMessage());
            return; 
        }

        // ✅ Pass the symbol table to the LexicalAnalyzer
        SymbolTable symbolTable = new SymbolTable(); 
        LexicalAnalyzer lexer = new LexicalAnalyzer(symbolTable); 
        List<Token> tokens = lexer.tokenize(code.toString());

        System.out.println("Tokens:");
        for (Token t : tokens) {
            System.out.println(t);
        }

        // ✅ Print symbol table after processing
        System.out.println("\nSymbol Table:");
        symbolTable.display();
    }
}
