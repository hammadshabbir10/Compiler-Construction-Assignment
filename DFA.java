package Compiler;

import java.util.*;

public class DFA {
    // transitions[fromDFAState][symbol] = toDFAState
    private Map<Integer, Map<Character, Integer>> transitions;
    private Set<Integer> acceptStates;
    private int startState;

    public DFA(Map<Integer, Map<Character, Integer>> transitions, int startState, Set<Integer> acceptStates) {
        this.transitions = transitions;
        this.startState = startState;
        this.acceptStates = acceptStates;
    }

    // Move from currentState on symbol; returns next state or null if no transition.
    public State move(State currentState, char symbol) {
        if (transitions.containsKey(currentState.getStateId())) {
            Map<Character, Integer> map = transitions.get(currentState.getStateId());
            if (map.containsKey(symbol)) {
                int next = map.get(symbol);
                return new State(next);
            }
        }
        return null;
    }

    public State getStartState() {
        return new State(startState);
    }

    public boolean isAccepting(State state) {
        return acceptStates.contains(state.getStateId());
    }
}
