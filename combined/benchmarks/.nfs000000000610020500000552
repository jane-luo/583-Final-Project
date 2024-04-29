#include <stdio.h>

#define NUM_FUNCTIONS 10000

void generate_code() {
    printf("#include <stdio.h>\n\n");
    printf("int main() {\n");
    for (int i = 0; i < NUM_FUNCTIONS; i++) {
        printf("    void function%d() {\n", i);
        printf("        printf(\"Function %d\\n\");\n", i);
        printf("    }\n");
    }
    printf("    // Call some of the generated functions\n");
    for (int i = 0; i < NUM_FUNCTIONS; i++) {
        if (i % 1000 == 0) {
            printf("    function%d();\n", i);
        }
    }
    printf("    return 0;\n");
    printf("}\n");
}

int main() {
    generate_code();
    return 0;
}