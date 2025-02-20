package Compiler;
import java.util.*;

class Symbol {
    private String name;
    private String type;
    private int memoryAddress;

    public Symbol(String name, String type, int memoryAddress) {
        this.name = name;
        this.type = type;
        this.memoryAddress = memoryAddress;
    }

    public String getName() { return name; }
    public String getType() { return type; }
    public int getMemoryAddress() { return memoryAddress; }

    @Override
    public String toString() {
        return "Symbol{name='" + name + "', type='" + type + "', mem=" + memoryAddress + "}";
    }
}

public class SymbolTable {
    private Map<String, Symbol> table;
    private int nextMemoryAddress;

    public SymbolTable() {
        table = new LinkedHashMap<>();
        nextMemoryAddress = 1000;
    }

    public void addSymbol(String name, String type) {
        if (table.containsKey(name)) {
            System.err.println("Warning: Symbol '" + name + "' is already defined! Skipping...");
            return; // Instead of throwing an exception, just print a warning
        }
        Symbol symbol = new Symbol(name, type, nextMemoryAddress++);
        table.put(name, symbol);
    }

    public boolean contains(String name) {
        return table.containsKey(name);
    }

    public Symbol getSymbol(String name) {
        return table.get(name);
    }

    public void display() {
        if (table.isEmpty()) {
            System.out.println("Symbol Table is empty!");
            return;
        }

        System.out.println("\nSymbol Table:");
        for (Symbol s : table.values()) {
            System.out.println(s);
        }
    }
}
