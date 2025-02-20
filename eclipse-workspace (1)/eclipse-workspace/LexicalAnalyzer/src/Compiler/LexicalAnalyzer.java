package Compiler;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
public class LexicalAnalyzer {
    private static final LinkedHashMap<String, TokenType> tokenPatterns = new LinkedHashMap<>();
    private SymbolTable symbolTable;
    private boolean insideFunction = false;

    public LexicalAnalyzer(SymbolTable symbolTable) { 
        this.symbolTable = symbolTable;
    }

    static {
        tokenPatterns.put("\\b(int|float|char|bool|main|print|scan|if|else|return)\\b", TokenType.KEYWORD);
        tokenPatterns.put("\\b(true|false)\\b", TokenType.BOOLEAN);
        tokenPatterns.put("[0-9]+\\.[0-9]{1,5}", TokenType.FLOAT); 
        tokenPatterns.put("[0-9]+", TokenType.INTEGER);
        tokenPatterns.put("[a-z_][a-z0-9_]*", TokenType.IDENTIFIER); 
        tokenPatterns.put("[(){};]", TokenType.PUNCTUATION);
        tokenPatterns.put("[+\\-*/=%^]", TokenType.OPERATOR);
        tokenPatterns.put("'[a-zA-Z0-9]'", TokenType.CHARACTER);
        tokenPatterns.put("\"[^\"]*\"", TokenType.STRING);
        tokenPatterns.put("//.*", TokenType.COMMENT);
        tokenPatterns.put("/\\*.*?\\*/", TokenType.COMMENT);
    }

    public List<Token> tokenize(String input) {
        List<Token> tokens = new ArrayList<>();
        int position = 0;

        while (position < input.length()) {
            if (Character.isWhitespace(input.charAt(position))) {
                position++;
                continue;
            }
            boolean matched = false;

            for (Map.Entry<String, TokenType> entry : tokenPatterns.entrySet()) {
                TokenType type = entry.getValue();
                String pattern = entry.getKey();

                java.util.regex.Pattern regex = java.util.regex.Pattern.compile(pattern);
                java.util.regex.Matcher matcher = regex.matcher(input.substring(position));

                if (matcher.lookingAt()) {
                    String tokenValue = matcher.group();
                    tokens.add(new Token(type, tokenValue));

                    if (tokenValue.equals("main")) {
                        insideFunction = true; 
                    }
                    if (tokenValue.equals("{")) { 
                        insideFunction = true; 
                    }
                    if (tokenValue.equals("}")) { 
                        insideFunction = false; 
                        symbolTable.display(); 
                        symbolTable.clearLocalSymbols(); 
                    }

                    if (type == TokenType.IDENTIFIER) { 
                        try {
                            symbolTable.addSymbol(tokenValue, "Variable", false, !insideFunction);
                        } catch (Exception e) {
                            System.out.println("SymbolTable Error: " + e.getMessage());
                        }
                    }

                    position += tokenValue.length();
                    matched = true;
                    break;
                }
            }

            if (!matched) {
                ErrorHandler.reportWarning("Skipping unrecognized character: " + input.charAt(position));
                position++;
            }
        }
        return tokens;
    }
}


