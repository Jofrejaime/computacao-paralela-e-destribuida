# 📘 Computação Paralela e Distribuída

## Aula de Laboratório Nº 2

**ISPTEC**
Coordenação de Engenharia Informática
Departamento de Engenharias e Tecnologias

---

## 🎯 Objectivo

* Introduzir o OpenMP

---

## 🧠 Introdução

Ao longo desta aula, o estudante é encorajado a consultar a documentação do OpenMP sempre que tiver dúvidas sobre as directivas ou APIs:

👉 http://www.openmp.org/wp-content/uploads/openmp-4.5.pdf

---

# 🔹 Problema 1

Este problema tenta familiarizar os estudantes com o ambiente de trabalho e a compilação do código OpenMP.

Considere o seguinte programa (`omp1.c`):

---

### a)

Comece por compilar o código acima, não esqueça de adicionar a flag de compilação:

```bash
-fopenmp
```

Execute este programa com um número diferente de threads (definindo `OMP_NUM_THREADS`).

---

### b)

Adicione a cláusula `nowait` à directiva `for` na linha 9.

* A saída foi alterada?
* Porquê?

---

### c)

Adicione uma barreira:

```c
#pragma omp barrier
```

entre as linhas 13 e 14.

Compare a saída com as alíneas a) e b).

---

# 🔹 Problema 2

No segundo problema, o estudante é solicitado a paralelizar um loop interno com dependências de dados.

O código básico que deve ser executado em paralelo é o seguinte:

```c
for(iter = 0; iter < numiter; iter++) 
{
    for (i = 0; i < size-1; i++)  
    {
        V[i] = f(V[i], V[i+1]);
    }
}
```

Onde definimos uma função simples para manipular os dados:

```c
#define f(x,y) ((x+y)/2.0)
```

---

### a)

Compile e execute a versão serial do código (`omp2.c`).

---

### b)

Crie uma versão paralela simples (e incorreta), adicionando uma directiva paralela ao loop interno.

* Tente encontrar uma execução com uma saída diferente da versão serial.
* Porquê isso pode acontecer?

---

### c)

Faça a correcção da versão paralela:

* Copiando o vector `V` da iteração `i-1` antes de executar a iteração `i`
* Usando os valores copiados como argumentos para a função `f` na iteração `i`

---

### d)

Escreva uma modificação na versão paralela que evite copiar o vector `V` em todas as iterações.

---
