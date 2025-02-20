package Compiler;
import java.util.*;

class State {
    private int id;
    private boolean isFinal;
    private List<Transition> transitions;

    public State(int id, boolean isFinal) {
        this.id = id;
        this.isFinal = isFinal;
        this.transitions = new ArrayList<>();
    }

    public void addTransition(char symbol, State nextState) {
        transitions.add(new Transition(symbol, nextState));
    }

    public List<Transition> getTransitions() {
        return transitions;
    }

    public boolean isFinal() {
        return isFinal;
    }

    public int getId() {
        return id;
    }
}
