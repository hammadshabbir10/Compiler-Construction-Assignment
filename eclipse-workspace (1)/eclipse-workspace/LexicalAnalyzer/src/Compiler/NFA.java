package Compiler;
import java.util.LinkedList;
import java.util.*;

public class NFA {
    private Map<Integer, Map<Character, Set<Integer>>> transitions;
    private int startState;
    private Set<Integer> acceptStates;
    private int stateCounter;

    public NFA(String pattern) {
        this.transitions = new HashMap<>();
        this.acceptStates = new HashSet<>();
        this.stateCounter = 0;
        this.startState = newState();
        buildNFAFromPattern(pattern);
    }

    private int newState() {
        return stateCounter++;
    }

    private void addTransition(int from, char symbol, int to) {
        transitions.computeIfAbsent(from, k -> new HashMap<>()).computeIfAbsent(symbol, k -> new HashSet<>()).add(to);
    }

    private void buildNFAFromPattern(String pattern) {
        int s0 = startState;
        int s1 = newState();
        for (char c : pattern.toCharArray()) {
            addTransition(s0, c, s1);
            s0 = s1;
        }
        acceptStates.add(s1);
    }

    public DFA toDFA() {
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
                Map<Character, Set<Integer>> edges = transitions.get(s);
                if (edges != null) {
                    for (Map.Entry<Character, Set<Integer>> e : edges.entrySet()) {
                        char symbol = e.getKey();
                        combined.computeIfAbsent(symbol, k -> new HashSet<>()).addAll(e.getValue());
                    }
                }
            }
            for (Map.Entry<Character, Set<Integer>> entry : combined.entrySet()) {
                char symbol = entry.getKey();
                Set<Integer> targetSet = entry.getValue();
                if (!stateMapping.containsKey(targetSet)) {
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
    
    public boolean validateIdentifier(String input) {
        for (char c : input.toCharArray()) {
            if (c < 'a' || c > 'z') {
                return false;
            }
        }
        return true;
    }

    public boolean validateInteger(String input) {
        for (char c : input.toCharArray()) {
            if (c < '0' || c > '9') {
                return false;
            }
        }
        return true;
    }

    public boolean validateOperator(String input) {
        return input.equals("+") || input.equals("-") || input.equals("*") || input.equals("/") || input.equals("%");
    }
}

