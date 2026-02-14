int main() {
    int x = 42;
    float y = 3.14;
    char c = 'A';
    char *str = "Hello, lexer!";
    int sum(int a, int b) {
        return a + b;
    }

    x = sum(x, 10);
    y = y * 2.0;

    if (x > 50) {
        x = x - 1;
    } else {
        x = x + 1;
    }
    for(int i = 0; i < 10; i++) {
        x = x + i;
    }
    while(x < 100) {
        x = x + 1;
    }
    x += 5; 
    y /= 2.0;
    int z = x % 3;
    printf("%s %c %d\n", str, c, x);
    # # # # #
    @
    汉字/漢字
    
    return 0;
}
