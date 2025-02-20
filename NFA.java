package Compiler;

import java.util.*;

public class NFA {
    private Map<Integer, Map<Character, Set<Integer>>> nfaTransitions;
    private int startState;
    private Set<Integer> acceptStates;
    private int stateCounter;
    private String pattern;

    public NFA(String pattern) 
    {
        this.nfaTransitions = new HashMap<>();
        this.acceptStates = new HashSet<>();
        this.stateCounter = 0;
        this.startState = newState();
        this.pattern = pattern;
        buildNFA(pattern);
    }

    private int newState() {
        return stateCounter++;
    }
    
    private void addTransition(int from, char symbol, int to) 
    {
        nfaTransitions
            .computeIfAbsent(from, k -> new HashMap<>())
            .computeIfAbsent(symbol, k -> new HashSet<>())
            .add(to);
    }

    private void buildNFA(String pattern) 
    {
        if (pattern.equals("[a-z]+")) {
            AlphaNFA();
        } else if (pattern.equals("[0-9]+")) 
        {
            DigitNFA();
        } else if (pattern.equals("[0-9]+\\.[0-9]{1,5}")) 
        {
            FloatNFA();
        } else if (pattern.contains("|")) {
            MultiLiteralNFA(pattern.split("\\|"));
        } else if (pattern.startsWith("[") && pattern.endsWith("]")) 
        {
            BracketRangeNFA(pattern);
        } else {
            LiteralNFA(pattern);
        }
    }

    public Map<Integer, Map<Character, Set<Integer>>> getTransitions() 
    {
        return nfaTransitions;
    }

    public int getStartState() 
    {
        return startState;
    }

    public Set<Integer> getAcceptStates() 
    {
        return acceptStates;
    }

    // Convert this NFA to a DFA via subset construction.
    public DFA conv_toDFA() 
    {
        Map<Set<Integer>, Integer> stateMapping = new HashMap<>();
        Map<Integer, Map<Character, Integer>> dfaTransitions = new HashMap<>();
        Set<Integer> dfaAcceptStates = new HashSet<>();

        Set<Integer> startSet = new HashSet<>();
        startSet.add(startState);
        int dfaStartId = 0;
        stateMapping.put(startSet, dfaStartId);

        Queue<Set<Integer>> queue = new LinkedList<>();
        queue.add(startSet);

        while (!queue.isEmpty()) {
            Set<Integer> currentSet = queue.poll();
            int dfaStateId = stateMapping.get(currentSet);
            dfaTransitions.putIfAbsent(dfaStateId, new HashMap<>());

            Map<Character, Set<Integer>> combined = new HashMap<>();
            for (int s : currentSet) {
                if (acceptStates.contains(s)) {
                    dfaAcceptStates.add(dfaStateId);
                }
                Map<Character, Set<Integer>> edges = nfaTransitions.get(s);
                if (edges != null) {
                    for (Map.Entry<Character, Set<Integer>> e : edges.entrySet()) {
                        char sym = e.getKey();
                        combined.computeIfAbsent(sym, k -> new HashSet<>()).addAll(e.getValue());
                    }
                }
            }
            for (Map.Entry<Character, Set<Integer>> entry : combined.entrySet()) 
            {
                char symbol = entry.getKey();
                Set<Integer> targetSet = entry.getValue();
                if (!stateMapping.containsKey(targetSet))
                {
                    int newId = stateMapping.size();
                    stateMapping.put(targetSet, newId);
                    queue.add(targetSet);
                }
                int toId = stateMapping.get(targetSet);
                dfaTransitions.get(dfaStateId).put(symbol, toId);
            }
        }
        return new DFA(dfaTransitions, dfaStartId, dfaAcceptStates);
    }
    
    private void AlphaNFA() 
    {
        int s0 = startState;
        int s1 = newState();
        for (char c = 'a'; c <= 'z'; c++) 
        {
            addTransition(s0, c, s1);
        }
        for (char c = 'a'; c <= 'z'; c++) 
        {
            addTransition(s1, c, s1);
        }
        acceptStates.add(s1);
    }

    private void DigitNFA() 
    {
        int s0 = startState;
        int s1 = newState();
        for (char c = '0'; c <= '9'; c++)
        {
            addTransition(s0, c, s1);
        }
        for (char c = '0'; c <= '9'; c++) 
        {
            addTransition(s1, c, s1);
        }
        acceptStates.add(s1);
    }

    private void FloatNFA() 
    {
        int s0 = startState;
        int s1 = newState(); 
        int s2 = newState(); 
        int s3 = newState(); //dig after dots

        for (char c = '0'; c <= '9'; c++) 
        {
            addTransition(s0, c, s1);
            addTransition(s1, c, s1);
        }
        addTransition(s1, '.', s2);
        for (char c = '0'; c <= '9'; c++) 
        {
            addTransition(s2, c, s3);
            addTransition(s3, c, s3);
        }
        acceptStates.add(s3);
    }

    private void MultiLiteralNFA(String[] words) 
    {
        int originalStart = startState;
        for (String w : words) {
            int current = originalStart;
            for (char c : w.toCharArray()) {
                int next = newState();
                addTransition(current, c, next);
                current = next;
            }
            acceptStates.add(current);
        }
    }

    private void BracketRangeNFA(String pattern) 
    {
        String inside = pattern.substring(1, pattern.length() - 1);
        int s0 = startState;
        int s1 = newState();
        for (char c : inside.toCharArray()) {
            addTransition(s0, c, s1);
        }
        acceptStates.add(s1);
    }

    private void LiteralNFA(String pattern) 
    {
        int current = startState;
        for (char c : pattern.toCharArray())
        {
            int next = newState();
            addTransition(current, c, next);
            current = next;
        }
        acceptStates.add(current);
    }

    
}
