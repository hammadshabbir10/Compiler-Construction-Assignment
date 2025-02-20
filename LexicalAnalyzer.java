package Compiler;
import java.util.*;

public class LexicalAnalyzer {
    private Map<TokenType, String> tokenPatterns;
    private SymbolTable symbolTable; 

    public LexicalAnalyzer(SymbolTable symbolTable) {
        this.symbolTable = symbolTable; 
        tokenPatterns = new LinkedHashMap<>();
        
        tokenPatterns.put(TokenType.KEYWORD, "int|float|char|bool|main|print|scan|if|else|return");
        tokenPatterns.put(TokenType.FLOAT, "[0-9]+\\.[0-9]{1,5}");
        tokenPatterns.put(TokenType.INTEGER, "[0-9]+");
        tokenPatterns.put(TokenType.IDENTIFIER, "[a-z]+");
        tokenPatterns.put(TokenType.PUNCTUATION, "[(){};]");
        tokenPatterns.put(TokenType.OPERATOR, "[+\\-*/=><]");
        tokenPatterns.put(TokenType.BOOLEAN, "true|false");
        tokenPatterns.put(TokenType.CHARACTER, "[a-z]");  
        tokenPatterns.put(TokenType.LITERAL, "\"[^\"]*\"");
        tokenPatterns.put(TokenType.COMMENT, "//.*|/\\*.*?\\*/");
    }

    public List<Token> tokenize(String input) 
    {
        List<Token> tokens = new ArrayList<>();
        int position = 0;
        String lastKeyword = null; 
        
        while (position < input.length()) {
            if (Character.isWhitespace(input.charAt(position))) 
            {
                position++;
                continue;
            }
            
            boolean matched = false;

            for (Map.Entry<TokenType, String> entry : tokenPatterns.entrySet()) 
            {
                TokenType type = entry.getKey();
                String pattern = entry.getValue();
                
                NFA nfa = new NFA(pattern);
                DFA dfa = nfa.conv_toDFA();
                
                int start = position;
                
                State curr_State = dfa.getStartState();
                State l_AcceptingState = null;
                
                int lastAcceptingPos = start;

                while (position < input.length()) 
                {
                    State nextState = dfa.move(curr_State, input.charAt(position));
                    if (nextState == null) break;
                    curr_State = nextState;
                    position++;
                    if (dfa.isAccepting(curr_State)) 
                    {
                        l_AcceptingState = curr_State;
                        lastAcceptingPos = position;
                    }
                }

                if (l_AcceptingState != null) {
                    String tokenValue = input.substring(start, lastAcceptingPos);
                    tokens.add(new Token(type, tokenValue));

                   
                    if (type == TokenType.KEYWORD) 
                    {
                        lastKeyword = tokenValue;
                    } 
                    else if (type == TokenType.IDENTIFIER) 
                    {
                        if (lastKeyword != null) 
                        { 
                            symbolTable.addSymbol(tokenValue, lastKeyword); 
                            lastKeyword = null; 
                        }
                    }

                    position = lastAcceptingPos;
                    matched = true;
                    break;
                } else 
                {
                    position = start;
                }
            }

            if (!matched) 
            {
                System.err.println("Unrecognized character: " + input.charAt(position));
                position++;
            }
        }

        return tokens;
    }
}
