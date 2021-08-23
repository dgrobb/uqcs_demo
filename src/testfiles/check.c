// check.c
//
// A basic C source file to test our code analyser

#define MAGIC 10

int add(int a, int b)
{
    return a + b;
}

int main(int argc, char** argv)
{
    return add(MAGIC, MAGIC * MAGIC);
}
