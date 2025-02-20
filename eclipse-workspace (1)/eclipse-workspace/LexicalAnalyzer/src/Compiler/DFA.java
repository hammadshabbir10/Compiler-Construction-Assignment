package Compiler;
import java.util.LinkedList;
import java.util.*;

public class DFA {
    private Map<Integer, Map<Character, Integer>> transitions;
    private Set<Integer> acceptStates;
    private int startState;

    public DFA(Map<Integer, Map<Character, Integer>> transitions, int startState, Set<Integer> acceptStates) {
        this.transitions = transitions;
        this.startState = startState;
        this.acceptStates = acceptStates;
    }

    public State move(State currentState, char symbol) {
        if (transitions.containsKey(currentState.getId())) {
            Map<Character, Integer> transitionMap = transitions.get(currentState.getId());
            if (transitionMap.containsKey(symbol)) {
                return new State(transitionMap.get(symbol), acceptStates.contains(transitionMap.get(symbol)));
            }
        }
        return null;
    }

    public boolean isAccepting(State state) {
        return acceptStates.contains(state.getId());
    }

    public State getStartState() {
        return new State(startState, acceptStates.contains(startState));
    }
   /*   
    void convertNFAtoDFA(NFA nfa) {
        startState = nfa.startState;
        states.add(startState);
        for (State s : nfa.states) {
            for (Transition t : s.transitions) {
                if (!states.contains(t.nextState)) {
                    states.add(t.nextState);
                }
            }
        }
    }

    boolean validate(String input) {
        State current = startState;
        for (char c : input.toCharArray()) {
            boolean found = false;
            for (Transition t : current.transitions) {
                if (t.symbol == c) {
                    current = t.nextState;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return current.isFinal;
    }*/
    
}

