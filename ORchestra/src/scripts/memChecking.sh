
valgrind --tool=memcheck --leak-check=full ./your_program
g++ -fsanitize=address *.cpp -std==c++17 -g -o Euclid