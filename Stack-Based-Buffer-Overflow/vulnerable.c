#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void shell() {
    system("/bin/sh"); // Llamada a una shell si logramos redirigir el flujo de ejecución
}

void vulnerable() {
    char buffer[64];
    printf("Introduce tu entrada: ");
    gets(buffer); // Función insegura (puede causar desbordamiento)
    printf("Tu entrada fue: %s\n", buffer);
}

int main() {
    vulnerable();
    return 0;
}
