#ifndef CONDITION_H
#define CONDITION_H

class Condition {
public:
    virtual bool test() = 0;  // Pure virtual method for testing the condition
    virtual ~Condition() = default;  // Virtual destructor for polymorphism
};

#endif // CONDITION_H
