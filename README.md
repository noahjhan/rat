# Rat Programming Language Grammar

## Introduction

Rat will be an attempt at writing simple compiler. Hopefully the lang will be turing complete. 

## Grammar


### Basic Syntax

**Hello World**: A simple rat program

    f_ main() {
        print("Hello, World"\n)
        rev
    }

f specifies a regular function - first class function with return

f_ specifies a void function - does not return a value

f! specifies a pure function - cannot modify external state

f/ specifies a lambda function - anonymous 

**Keywords**

state is sort of a class that represents the current state of the program

return state would suggest every variable labeled state also becomes scoped to the block that stores the return

    f returns_state() {
        state int x = 14
        return state
    }

    f_ main() {
        state k = returns_state()
        print(k.x)
    }

state variables cannot be modified. this way functions can have many returns, sort of on-the-fly structs. 

however non-state variables can be cast to state

    f recursive() {
        int x = 10
        int sum = 0
        if (x == 0) {
            state(x)
            state(sum)
            return state
        }
        recursive(x - 1)
    }

**Variable Assignment**

    int x = 10
    array arr = [1, 2, 3, 4, 5]
    double y = 2.20

Basically c-style with some additional types


