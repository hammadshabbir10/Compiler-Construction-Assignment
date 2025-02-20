package Compiler;
import java.util.LinkedHashMap;
import java.util.Map;

class Symbol {
    private String name;
    private String type;
    private int memoryAddress;
    private boolean isConstant;
    private boolean isGlobal; 

    public Symbol(String name, String type, int memoryAddress, boolean isConstant, boolean isGlobal) {
        this.name = name;
        this.type = type;
        this.memoryAddress = memoryAddress;
        this.isConstant = isConstant;
        this.isGlobal = isGlobal; 
    }

    public String getName() { return name; }
    public String getType() { return type; }
    public int getMemoryAddress() { return memoryAddress; }
    public boolean isConstant() { return isConstant; }
    public boolean isGlobal() { return isGlobal; } 

    @Override
    public String toString() {
        return "Symbol{name='" + name + "', type='" + type + "', mem=" + memoryAddress + 
               ", constant=" + isConstant + ", scope=" + (isGlobal ? "Global" : "Local") + "}";
    }
}
public class SymbolTable {
    private Map<String, Symbol> globalSymbols; 
    private Map<String, Symbol> localSymbols;  
    private int nextMemoryAddress;

    public SymbolTable() {
        globalSymbols = new LinkedHashMap<>();
        localSymbols = new LinkedHashMap<>();
        nextMemoryAddress = 1000;
    }

    public void addSymbol(String name, String type, boolean isConstant, boolean isGlobal) throws Exception {
        Map<String, Symbol> targetTable = isGlobal ? globalSymbols : localSymbols; // ✅ Choose correct table

        if (targetTable.containsKey(name)) {
            throw new Exception("Symbol already defined in " + (isGlobal ? "Global" : "Local") + " scope: " + name);
        }

        Symbol symbol = new Symbol(name, type, nextMemoryAddress++, isConstant, isGlobal);
        targetTable.put(name, symbol);
    }

    public boolean contains(String name, boolean isGlobal) {
        Map<String, Symbol> targetTable = isGlobal ? globalSymbols : localSymbols;
        return targetTable.containsKey(name);
    }

    public void display() {
        System.out.println("\nGlobal Symbol Table:");
        for (Symbol s : globalSymbols.values()) {
            System.out.println(s);
        }

        System.out.println("\nLocal Symbol Table:");
        for (Symbol s : localSymbols.values()) {
            System.out.println(s);
        }
    }

    public void clearLocalSymbols() {
        localSymbols.clear();
    }
}

