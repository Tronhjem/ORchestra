#pragma once
#include <array>

namespace ORchestra
{
    template <typename T>
    class Stack
    {
    public:
        Stack() : mStackPointer(0) {}
        T Pop()
        {
#if _DEBUG
            assert(mStackPointer > 0);
#endif
            return mStack[--mStackPointer];
        }

        void Push(T i)
        {
#if _DEBUG
            assert(mStackPointer + 1 <= 64);
#endif
            mStack[mStackPointer++] = i;
        }

        void Clear()
        {
            mStackPointer = 0;
        }

        T Top()
        {
#if _DEBUG
            assert(mStackPointer != 0 && mStackPointer > 0);
#endif
            return mStack[mStackPointer - 1];
        }

        unsigned long mStackPointer;

    private:
        // TODO: Should we make this smaller or bigger??
        std::array<T, 64> mStack;
    };
} // namespace ORchestra
