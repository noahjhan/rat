# Rat Programming Language

## Introduction

Rat will be an attempt at writing simple compiler. Hopefully the lang will be turing complete. 

## Grammar


### Basic Syntax

**Hello World**: A simple rat program

    fn_ main() {
        print("Hello, World"\n)
        rev                         // return void
    }

fn specifies a regular function - first class function with static return type

fn_ specifies a void function - no return 

fn? specifies an optional function - can return NULL or object cast to optional type

fn/ specifies a lambda function - anonymous 
