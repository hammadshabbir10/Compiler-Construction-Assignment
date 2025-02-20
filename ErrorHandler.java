package Compiler;
import java.util.LinkedList;

public class ErrorHandler {
    public static void reportError(String message) {
        System.out.println("ERROR: " + message);
    }
    public static void reportWarning(String message) {
        System.out.println("Warning: " + message);
    }
}
