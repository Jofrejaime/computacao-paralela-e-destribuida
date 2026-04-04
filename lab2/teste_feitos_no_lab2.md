# 🔹 Problema 1 — a)

## ⚙️ Compilação

O programa `omp1.c` foi compilado utilizando o compilador `gcc` com a flag de suporte ao OpenMP:

```bash
gcc omp1.c -fopenmp -o omp1
```

A flag `-fopenmp` é necessária para ativar as diretivas de paralelismo do OpenMP durante a compilação.

---

## ▶️ Execução com diferentes números de threads

A execução do programa foi realizada com diferentes números de threads, definidos através da variável de ambiente `OMP_NUM_THREADS`.

---

### 🔸 Execução com 2 threads

```bash
export OMP_NUM_THREADS=2
./omp1
```

**Observação:**

* As iterações foram divididas entre duas threads.
* A thread 0 executou aproximadamente metade inicial das iterações.
* A thread 1 executou a metade final.
* A saída apresentou-se relativamente ordenada.

---

### 🔸 Execução com 3 threads

```bash
export OMP_NUM_THREADS=3
./omp1
```

**Observação:**

* O trabalho foi distribuído entre três threads.
* As iterações foram divididas em blocos distintos.
* A ordem de execução deixou de ser sequencial.
* Observou-se interleaving (mistura) das saídas das threads.

---

### 🔸 Execução com 7 threads

```bash
export OMP_NUM_THREADS=7
./omp1
```

**Observação:**

* As iterações foram distribuídas por várias threads em blocos menores.
* A execução ocorreu de forma altamente concorrente.
* A saída tornou-se desordenada devido à execução paralela.

---

## 🧠 Conclusão

* O OpenMP distribui automaticamente as iterações do ciclo entre as threads disponíveis.
* À medida que o número de threads aumenta, o paralelismo também aumenta.
* A ordem de execução das instruções não é determinística, podendo variar entre execuções.
* A ausência de sincronização na impressão resulta em saídas intercaladas.
* Como não existem dependências de dados entre iterações, o programa mantém-se correto independentemente do número de threads.

---

# 🔹 Problema 1 — b)

## ⚙️ Modificação realizada

Foi adicionada a cláusula `nowait` à diretiva `for`:

```c
#pragma omp for nowait
```

---

## 📊 Observação da execução

Após a execução do programa com diferentes números de threads (`OMP_NUM_THREADS = 2, 3 e 7`), observou-se que:

* Algumas threads imprimem as mensagens:

  * `Thread X, almost..`
  * `Thread X, done!`

  **antes de outras threads terminarem as suas iterações do ciclo `for`.**

### ✔️ Exemplos observados:

* Com **2 threads**:

  * A thread 0 terminou completamente (incluindo "almost" e "done")
  * Enquanto a thread 1 ainda estava a executar o loop

* Com **3 threads**:

  * A thread 1 terminou antes das threads 0 e 2
  * As mensagens finais aparecem no meio da execução das outras threads

* Com **7 threads**:

  * Cada thread termina o seu bloco de iterações e imprime imediatamente
  * A saída mostra claramente execução independente entre threads

---

## 🧠 Explicação

Por defeito, a diretiva:

```c
#pragma omp for
```

possui uma **barreira implícita no final do ciclo**, o que obriga todas as threads a sincronizarem antes de continuar.

Ao adicionar a cláusula `nowait`:

* Essa barreira é removida
* Cada thread continua a execução assim que termina o seu conjunto de iterações
* Não existe espera pelas restantes threads

---

## 🔄 Impacto na saída

* A saída tornou-se **não determinística**
* As mensagens finais (`almost` e `done`) aparecem **intercaladas com o loop**
* A ordem de execução varia entre execuções

---

## ✅ Conclusão

* Sim, a saída foi alterada.
* A alteração deve-se à remoção da sincronização implícita no final do ciclo `for`.
* O uso de `nowait` permite maior paralelismo, mas reduz o controlo sobre a ordem de execução.

---

# 🔹 Problema 1 — c)

## ⚙️ Modificação realizada

Foi adicionada uma barreira explícita após o ciclo `for`, mantendo a cláusula `nowait`:

```c
#pragma omp for nowait
for(i = 0; i < NUMITER; i++)
    printf("Thread: %d\titer=%d\n", tid, i);

#pragma omp barrier

printf("Thread %d, almost..\n", tid);
printf("Thread %d, done!\n", tid);
```

---

## 📊 Observação da execução

Após a execução com diferentes números de threads (`OMP_NUM_THREADS = 2, 3 e 7`), observou-se que:

* Todas as threads completam o ciclo `for` antes de executar as instruções seguintes

* As mensagens:

  * `Thread X, almost..`
  * `Thread X, done!`

  **só aparecem depois de todas as iterações terminarem**

* A saída volta a apresentar um comportamento semelhante ao caso (a), embora a ordem das iterações continue não determinística

---

## 🧠 Explicação

A diretiva:

```c
#pragma omp barrier
```

introduz um ponto de sincronização explícito:

* Todas as threads são obrigadas a esperar até que todas cheguem à barreira
* Só depois continuam a execução

Mesmo com a utilização de:

```c
#pragma omp for nowait
```

a barreira garante que nenhuma thread avance prematuramente.

---

## 🔄 Comparação com os casos anteriores

| Caso | Comportamento                             |
| ---- | ----------------------------------------- |
| a)   | Sincronização implícita no final do `for` |
| b)   | Sem sincronização (`nowait`)              |
| c)   | Sincronização explícita (`barrier`)       |

---

## ✅ Conclusão

* A introdução da barreira restabelece a sincronização entre threads
* O comportamento torna-se novamente controlado
* Demonstra-se a diferença entre sincronização implícita e explícita em OpenMP

---

# 🔹 Problema 2 — a)

## ⚙️ Implementação

Foi utilizada a versão serial do código fornecido, sem qualquer paralelização:

```c
for(iter = 0; iter < NUMITER; iter++) {
    for (i = 0; i < TOTALSIZE-1; i++) {
        V[i] = f(V[i], V[i+1]);
    }
}
```

com:

```c
#define f(x,y) ((x+y)/2.0)
```

---

## ▶️ Execução

O programa foi compilado e executado com:

```bash
gcc omp2.c -o omp2
./omp2
```

---

## 📊 Observação dos resultados

* O vetor foi inicializado com valores crescentes (`V[i] = i`)
* Após várias iterações, os valores do vetor foram sendo suavizados
* Observou-se que:

  * Os valores finais convergem para o valor máximo do vetor (999)
  * Existe uma propagação dos valores da direita para a esquerda
  * O comportamento é determinístico (mesmo resultado em todas as execuções)

---

## 🧠 Análise

* Cada elemento `V[i]` depende de `V[i+1]`
* A atualização ocorre diretamente sobre o vetor original
* Isso cria uma dependência de dados entre iterações do loop interno

---

## ✅ Conclusão

* A versão serial executa corretamente
* O algoritmo apresenta dependência de dados, o que dificulta a paralelização direta
* Esta dependência será responsável por erros na versão paralela

---


# 🔹 Problema 2 — b)

## ⚙️ Implementação

Foi criada uma versão paralela simples (e incorreta) do algoritmo, adicionando a diretiva OpenMP ao loop interno:

```c
for(iter = 0; iter < NUMITER; iter++) {

    #pragma omp parallel for
    for(i = 0; i < TOTALSIZE-1; i++) {
        V[i] = f(V[i], V[i+1]);
    }

}
```

---

## ▶️ Compilação e Execução

```bash
gcc omp2.c -fopenmp -o omp2
export OMP_NUM_THREADS=4
./omp2
```

---

## 📊 Observação dos Resultados

* A saída obtida difere da versão serial
* O resultado pode variar entre execuções
* O comportamento observado é não determinístico

---

## 🧠 Análise

O problema reside na existência de **dependências de dados entre iterações consecutivas** do loop.

Cada elemento é atualizado com base no seguinte:

```c
V[i] = f(V[i], V[i+1])
```

Assim, o valor de `V[i]` depende diretamente de `V[i+1]`.

Na versão paralela:

* Múltiplas threads executam iterações simultaneamente
* Uma thread pode atualizar `V[i+1]` enquanto outra ainda precisa do valor antigo
* Não existe sincronização que garanta a ordem correta de acesso aos dados

---

## ⚠️ Problema Identificado

O comportamento incorreto deve-se a uma **condição de corrida (race condition)**:

* Leituras e escritas concorrentes no vetor `V`
* Ordem de execução imprevisível entre threads
* Uso de valores inconsistentes durante o cálculo

---

## 🔄 Consequências

* Resultados incorretos
* Diferenças em relação à versão serial
* Resultados variáveis entre execuções

---

## ✅ Conclusão

A paralelização direta do loop interno não é válida, devido à presença de dependências de dados entre iterações. É necessário utilizar uma abordagem alternativa que elimine essas dependências para garantir a correção do algoritmo.

---
# 🔹 Problema 2 — c)

## ⚙️ Implementação

Para corrigir a versão paralela, foi utilizado um vetor auxiliar (`V_old`) para armazenar os valores da iteração anterior.

Antes de cada iteração, os valores do vetor principal são copiados:

```c
for(i = 0; i < TOTALSIZE; i++) {
    V_old[i] = V[i];
}
```

Em seguida, o cálculo é realizado utilizando apenas os valores copiados:

```c
#pragma omp parallel for
for(i = 0; i < TOTALSIZE-1; i++) {
    V[i] = f(V_old[i], V_old[i+1]);
}
```

---

## 🧠 Análise

* O vetor `V_old` garante que todas as threads leem dados consistentes
* Não há escrita concorrente sobre os mesmos dados utilizados para leitura
* As dependências de dados são eliminadas

---

## ✅ Resultados

* A saída é idêntica à versão serial
* O comportamento é determinístico
* A execução paralela torna-se correta

---

## 🏁 Conclusão

A utilização de um vetor auxiliar elimina as condições de corrida, permitindo a paralelização correta do algoritmo mesmo na presença de dependências de dados entre iterações.

---
# 🔹 Problema 2 — d)

## ⚙️ Implementação

Para evitar a cópia do vetor a cada iteração, foi utilizada a técnica de **double buffering**, com dois vetores auxiliares (`V1` e `V2`).

Em cada iteração:

* Um vetor é utilizado para leitura (`read`)
* O outro é utilizado para escrita (`write`)
* No final da iteração, os ponteiros são trocados

```c
double *temp = read;
read = write;
write = temp;
```

O cálculo paralelo é feito da seguinte forma:

```c
#pragma omp parallel for
for(i = 0; i < TOTALSIZE-1; i++) {
    write[i] = f(read[i], read[i+1]);
}
```

---

## 🧠 Análise

* Cada iteração utiliza apenas dados da iteração anterior
* Não há necessidade de copiar o vetor completo
* A troca de ponteiros é uma operação eficiente (O(1))
* As dependências de dados são eliminadas

---

## ✅ Resultados

* A saída permanece correta e idêntica à versão serial
* O comportamento é determinístico
* A execução é mais eficiente comparada à solução anterior

---

## 🏁 Conclusão

A utilização de double buffering permite evitar a cópia do vetor a cada iteração, mantendo a correção e melhorando a eficiência do algoritmo paralelo.

---
