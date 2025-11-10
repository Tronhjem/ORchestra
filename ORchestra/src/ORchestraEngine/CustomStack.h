#pragma once

template <typename T>
class Stack
{
public:
    Stack() {};
    T Pop()
    {
#if _DEBUG
        assert(stackPointer > 0);
#endif
        return mStack[--stackPointer];
    }
    
    void Push(T i)
    {
#if _DEBUG
        assert(stackPointer + 1 <= 64);
#endif
        mStack[stackPointer++] = i;
    }
    
    void Clear()
    {
        stackPointer = 0;
    }
    
    T Top()
    {
        return mStack[stackPointer];
    }

private:
    // TODO: Should we make this smaller or bigger??
    std::array<T, 64> mStack;
    int stackPointer = 0;
};
