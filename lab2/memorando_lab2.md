# Memorando

**De:** Jofre Jaime Jamuinda Mutumbungo 
**Nº de Matrícula:** 20230394
**Disciplina:** Computação Paralela e Distribuída (CPD)  
**Assunto:** Relatório técnico dos testes do Laboratório 2 (OpenMP)  
**Data:** 04/04/2026

---

## 1. Introdução

Este memorando apresenta os testes realizados no Laboratório 2 da disciplina de CPD, com foco em sincronização de threads e paralelização com OpenMP. Os objetivos principais foram:

1. Analisar o comportamento de execução paralela com diferentes números de threads no problema de sincronização.
2. Observar o impacto de barreiras implícitas e explícitas no fluxo de execução.
3. Identificar dependências de dados e condições de corrida no problema numérico do vetor.
4. Implementar versões paralelas corretas com vetor auxiliar e com double buffering.

Restrições e observações relevantes:

1. O ambiente de compilação utilizado foi `gcc`, com `-fopenmp` para habilitar OpenMP.
2. A ordem de impressão em programas paralelos não é determinística.
3. Nem toda diretiva `parallel for` é válida sem análise de dependências de dados.

---

## 2. Experiências Realizadas

### 2.1 Configuração, build e execução

Comandos utilizados:

```bash
gcc omp1.c -fopenmp -o omp1
gcc omp2.c -fopenmp -o omp2
```

Execuções típicas:

```bash
export OMP_NUM_THREADS=2
./omp1

export OMP_NUM_THREADS=3
./omp1

export OMP_NUM_THREADS=7
./omp1

export OMP_NUM_THREADS=4
./omp2
```

### 2.2 Problema 1 - Sincronização de threads

Foi usado o arquivo `omp1.c`, com região paralela e distribuição de iterações por `#pragma omp for`.

Código (trecho principal):

```c
#pragma omp parallel private(i,tid)
{
    tid = omp_get_thread_num();

    #pragma omp for nowait
    for(i = 0; i < NUMITER; i++)
        printf("Thread: %d\titer=%d\n", tid, i);

    #pragma omp barrier
    printf("Thread %d, almost..\n", tid);
    printf("Thread %d, done!\n", tid);
}
```

#### a) Comportamento base

Observou-se divisão automática das iterações entre threads. Com aumento do número de threads, a saída ficou mais intercalada. A lógica do cálculo permaneceu correta, pois não há dependência de dados entre iterações.

#### b) Uso de `nowait`

Ao remover a barreira implícita do `for` com `nowait`, algumas threads imprimiram `almost` e `done` antes de outras terminarem as iterações. Conclusão: houve maior paralelismo, com perda de sincronização ao final do loop.

#### c) Uso de `barrier`

Com `#pragma omp barrier` após o `for nowait`, a sincronização foi restabelecida: todas as threads completam o loop antes de avançar para as mensagens finais. Isso confirmou, na prática, a diferença entre barreira implícita (padrão do `for`) e explícita.

### 2.3 Problema 2 - Dependência de dados

Foi analisado o algoritmo com atualização iterativa:

```c
#define f(x,y) ((x+y)/2.0)
V[i] = f(V[i], V[i+1]);
```

#### a) Versão serial

A versão serial produziu saída determinística e correta. Houve suavização dos valores e propagação da influência dos elementos da direita para a esquerda.

#### b) Paralelização incorreta direta

Ao aplicar `#pragma omp parallel for` diretamente no loop interno, surgiram resultados diferentes da versão serial e não determinísticos. Causa: condição de corrida, devido à dependência entre `V[i]` e `V[i+1]`.

#### c) Correção com vetor auxiliar

A correção foi feita copiando o estado anterior para `V_old` e lendo apenas de `V_old` no cálculo paralelo. Resultado: comportamento determinístico e equivalente ao serial.

#### d) Otimização com double buffering

Foi implementada a técnica com dois vetores (`read` e `write`) e troca de ponteiros por iteração, evitando cópia completa a cada passo. O resultado manteve-se correto e com melhor eficiência.

### 2.4 Figuras e evidências

Inserir capturas de tela nesta secção com legenda numerada:

![Figura 1 - Compilação do omp1 com OpenMP](imagens/figura1_compilacao_omp1.png)

Figura 1 - Compilação do programa `omp1.c` com `gcc -fopenmp`.

![Figura 2 - Execução do omp1 com 2 threads](imagens/figura2_execucao_omp1_2threads.png)

Figura 2 - Saída da execução de `omp1` com `OMP_NUM_THREADS=2`.

![Figura 3 - Execução do omp1 com 7 threads](imagens/figura3_execucao_omp1_7threads.png)

Figura 3 - Saída com alto interleaving para `OMP_NUM_THREADS=7`.

![Figura 4 - Saída incorreta da versão paralela direta em omp2](imagens/figura4_omp2_saida_incorreta.png)

Figura 4 - Evidência de comportamento não determinístico na versão incorreta.

![Figura 5 - Saída correta da versão double buffering](imagens/figura5_omp2_double_buffering.png)

Figura 5 - Saída correta e determinística com double buffering.

---

## 3. Desafios

Panorâmica do que foi concebido:

1. Estrutura em dois programas independentes: `omp1.c` (sincronização) e `omp2.c` (dependências de dados).
2. Evolução incremental das soluções: serial -> paralelo incorreto -> paralelo correto com cópia -> paralelo correto otimizado.
3. Principal desafio técnico: identificar que a dependência `V[i] <- V[i+1]` inviabiliza paralelização direta.
4. Principal decisão de engenharia: separar dados de leitura e escrita por iteração para garantir correção.

---

## 4. Referências Bibliográficas

1. OpenMP Architecture Review Board. *OpenMP API Specification*.
2. GCC Documentation. *Options for OpenMP (`-fopenmp`)*.
3. Material de apoio da disciplina CPD (ISPTEC).
4. Código base e enunciado do Laboratório 2.

---

## 5. Repositório GitHub

Link do repositório: [ \[link do repositorio\]](https://github.com/Jofrejaime/computacao-paralela-e-destribuida)

Observação: convite de colaborador para o utilizador GitHub **joaojdacosta**.

---

## Anexo - Nota de formatação para gerar DOCX

Para manter código com estilo equivalente a *Courier New* tamanho 8 no documento final, recomenda-se converter este Markdown com Pandoc usando um arquivo de referência (`reference.docx`) configurado com esse estilo para blocos de código.
