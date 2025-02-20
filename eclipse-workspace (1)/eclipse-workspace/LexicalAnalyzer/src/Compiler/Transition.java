package Compiler;

class Transition {
    private char symbol;
    private State nextState;

    public Transition(char symbol, State nextState) {
        this.symbol = symbol;
        this.nextState = nextState;
    }

    public char getSymbol() {
        return symbol;
    }

    public State getNextState() {
        return nextState;
    }
}
