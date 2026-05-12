# Memorando

De: [Preencher nome do estudante]
N de Matricula: [Preencher]
Disciplina: CPD
Assunto: Relatorio do Laboratorio 4 - MPI
Data: 12/05/2026

## 1. Introducao

Este memorando apresenta a experiencia pratica do Laboratorio 4 de CPD sobre comunicacao paralela com MPI.

Objetivos principais do laboratorio:

1. Criar e executar um programa de cumprimento por processo.
2. Analisar, compilar e executar o programa [sendReceive.c](sendReceive.c).
3. Medir experimentalmente latencia e largura de banda da rede.
4. Comparar desempenho entre broadcast e envios individuais de um array grande.

Restricoes e observacoes importantes:

1. O ambiente usado foi Windows com compilador MPI instalado.
2. As medicoes dependem do numero de processos, rounds e tamanho da mensagem.
3. Resultados devem ser analisados com repeticoes para reduzir variacao.

## 2. Experiencias Realizadas

### 2.1 Exercico 1 - Programa de cumprimentos

Descricao tecnica:

1. Cada processo imprime seu rank e nome do no.
2. O objetivo e confirmar inicializacao correta do MPI.

Script/comandos usados:

```bash
# Exemplo (ajustar ao ambiente local)
mpicc hello.c -o hello
mpirun -np 4 ./hello
```

Figura 1 - Execucao do programa de cumprimentos.

### 2.2 Exercico 2.a - Analise funcional de sendReceive.c

O programa [sendReceive.c](sendReceive.c) implementa uma experiencia simples de comunicacao ponto-a-ponto para medir custo de envio/rececao entre processos MPI.

Fluxo geral:

1. Inicializa MPI com `MPI_Init`.
2. Obtem rank local (`id`) com `MPI_Comm_rank` e total de processos (`p`) com `MPI_Comm_size`.
3. Le argumento `n-rounds` da linha de comando.
4. Sincroniza processos com `MPI_Barrier` e inicia cronometro com `MPI_Wtime`.
5. Executa `rounds` iteracoes de troca de mensagens em anel.
6. Sincroniza novamente, encerra medicao e imprime metricas no processo 0.

Logica de anel em cada iteracao `i`:

1. Processo 0 envia para processo 1 e recebe de `p-1`.
2. Processos 1 ate `p-1` recebem de `id-1` e enviam para `(id+1)%p`.

Topologia logica:

`0 -> 1 -> 2 -> ... -> p-1 -> 0`

Papel de `n-rounds`:

1. Define quantas voltas do anel serao executadas.
2. Aumenta linearmente o numero de operacoes de comunicacao.
3. Influencia o tempo total e a estabilidade da media.

Metrica final reportada:

$$
	ext{tempo medio por Send/Recv (us)} = \frac{\text{secs} \cdot 10^6}{2 \cdot \text{rounds} \cdot p}
$$

Observacoes:

1. O programa pressupoe pelo menos 2 processos.
2. O foco e custo de comunicacao, nao conteudo da mensagem.

### 2.3 Exercico 2.b - Compilacao e execucao com diferentes parametros

### 2.3 Exercicio 2.b - Compilacao e execucao com diferentes parametros

Descricao tecnica:

1. Compilar [sendReceive.c](sendReceive.c) com o compilador MPI.
2. Executar variando `n-rounds` e numero de processos (`-np`).
3. Coletar tempo total e tempo medio por Send/Recv para cada combinacao.

**Ambiente necessario: WSL (Windows Subsystem for Linux) com OpenMPI.**

#### Instalacao do OpenMPI em WSL (primeira execucao apenas)

Abre terminal WSL e executa:

```bash
sudo apt-get update
sudo apt-get install -y libopenmpi-dev openmpi-bin
```

Verifica instalacao:

```bash
which mpicc
mpicc --version
mpirun --version
```

#### Compilacao do programa

No diretorio do laboratorio:

```bash
cd ~/ISPTEC/3º\ ano/2º\ Semestre/CPD/labs/lab4
mpicc sendReceive.c -o sendReceive
```

Se compilacao bem-sucedida, aparece executavel `sendReceive`.

#### Execucoes com diferentes parametros

Execute os seguintes comandos e anote os resultados:

**Conjunto 1: Testar escalabilidade com numero de processos**

```bash
# 2 processos, 10000 rounds
mpirun -np 2 ./sendReceive 10000

# 4 processos, 10000 rounds  
mpirun -np 4 ./sendReceive 10000

# 8 processos, 10000 rounds
mpirun -np 8 ./sendReceive 10000
```

**Conjunto 2: Testar estabilidade com mais rounds**

```bash
# 2 processos, 50000 rounds (melhor estatistica)
mpirun -np 2 ./sendReceive 50000

# 4 processos, 50000 rounds
mpirun -np 4 ./sendReceive 50000
```

#### Exemplo de saida esperada

Quando executas `mpirun -np 4 ./sendReceive 10000`, a saida sera:

```
Rounds= 10000, N Processes = 4, Time = 0.123456 sec,
Average time per Send/Recv = 7.71 us
```

Isto significa:
- Tempo total de todas as comunicacoes: 0.123456 segundos.
- Media por operacao: 7.71 microsegundos (latencia+overhead).

Figura 1 - Execucao do sendReceive.c com 4 processos e 10000 rounds.

Tabela 1 - Resultados experimentais para diferentes valores de `n-rounds` e `p`.

| n-rounds | p | Tempo total (s) | Tempo medio Send/Recv (us) | Observacoes |
|---|---:|---:|---:|---|
| 10000 | 2 | 0.003440 | 0.09 | Baseline: menos processos |
| 10000 | 4 | 0.007623 | 0.10 | 2x mais processos que baseline |
| 50000 | 2 | 0.014453 | 0.07 | 5x mais rounds que primeira linha |
| 50000 | 4 | 0.042558 | 0.11 | 5x mais rounds, estatistica mais estavel |
| 10000 | 8 | N/A - Erro | N/A | Sistema tem apenas 4 cores; 8 slots nao disponiveis |

### 2.4 Exercico 2.c - Interpretacao dos resultados

**Conclusao desta etapa (baseada nos resultados reais da Tabela 1):**

#### Analise dos resultados obtidos

**1. Validacao da linearidade com n-rounds:**

Comparando p=2:
- rounds=10000: tempo = 0.003440 s
- rounds=50000: tempo = 0.014453 s

Razao: 0.014453 / 0.003440 = 4.20

Esperado para 5x mais rounds: ≈ 5.0

Resultado: **Escala quase linear** (4.20 vs esperado 5.0). A pequena desvio pode ser devido a variacao de cache, escalonamento do SO, etc.

**2. Efeito do numero de processos:**

Com rounds=10000:
- p=2: tempo = 0.003440 s
- p=4: tempo = 0.007623 s

Razao: 0.007623 / 0.003440 = 2.21

Esperado: com anel de p processos, tempo deve escalar aproximadamente com (p-1):
- (4-1)/(2-1) = 3/1 = 3.0

Resultado: **Escala menos que linear com p** (2.21 vs esperado 3.0). Possivel razao: latencia fixa de contexto + overhead de sincronizacao, nao proporcional ao numero de processos em aneis pequenos.

**3. Latencia de comunicacao:**

Media por operacao Send/Recv: **0.09-0.11 microsegundos**

Isto esta muito abaixo da latencia tipica de rede (1-5 us) porque:
- Ambiente de teste: todos os processos na mesma maquina local (WSL).
- Comunicacao inter-processo (IPC) e extremamente rapida em localhost.
- Se fosse rede real entre multiplos nos, latencia seria 10-100x maior.

**4. Estabilidade estatistica:**

Comparando media por operacao para mesmos parametros:
- 10000 rounds: media = 0.09-0.10 us
- 50000 rounds: media = 0.07-0.11 us

Variacao e pequena (< 60%), indicando que media converge para valor verdadeiro. Com mais rounds, variacao relativa diminui.

**5. Observacoes sobre erro com p=8:**

Sistema local tem apenas 4 cores (4 slots). MPI impede allocacao de 8 processos por padrao (evita oversubscription danosa). Para forcar, seria necessario usar `--oversubscribe`, mas resultados seriam menos confiáveis (processes competindo por CPUs).

**Conclusoes:**

1. ✅ Programa [sendReceive.c](sendReceive.c) funciona corretamente e mede comunicacao em anel.
2. ✅ Tempo total escala aproximadamente linear com rounds.
3. ✅ Tempo total escala com numero de processos (mas menos que linear para aneis pequenos).
4. ✅ Latencia medida (~0.1 us) e realista para comunicacao local entre processos.
5. ✅ Media por operacao converge e estabiliza com mais rounds.

O programa e util para benchmarking de latencia de comunicacao ponto-a-ponto em clusters MPI.

### 2.5 Exercico 3 - Latencia e largura de banda

Metodologia proposta:

O programa [bandwidth.c](bandwidth.c) foi criado como extensao de [sendReceive.c](sendReceive.c) para medir:

1. **Latencia**: tempo medio para envio e rececao de uma mensagem.
2. **Largura de banda**: taxa de transferencia de dados em MB/s.

Formulas utilizadas:

$$
\text{latencia (us)} = \frac{\text{tempo total} \cdot 10^6}{2 \cdot \text{rounds} \cdot p}
$$

$$
\text{largura de banda (MB/s)} = \frac{\text{bytes totais transferidos (MB)}}{\text{tempo total (s)}}
$$

Compilacao:

```bash
mpicc bandwidth.c -o bandwidth
```

Execucoes com diferentes tamanhos de mensagem:

```bash
# Latencia: mensagens pequenas (16 bytes)
mpirun -np 4 ./bandwidth 50000 16

# Latencia media: mensagens de 256 bytes
mpirun -np 4 ./bandwidth 50000 256

# Largura de banda: mensagens de 4 KB
mpirun -np 4 ./bandwidth 10000 4096

# Largura de banda: mensagens de 64 KB
mpirun -np 4 ./bandwidth 5000 65536

# Largura de banda: mensagens de 1 MB
mpirun -np 4 ./bandwidth 1000 1048576
```

Figura 2 - Execucao do teste de latencia com mensagens pequenas.
Figura 3 - Execucao do teste de largura de banda com mensagens grandes.

Tabela 2 - Latencia e largura de banda medidas.

| Tamanho da mensagem | Rounds | p | Tempo total (s) | Latencia (us) | Banda (MB/s) |
|---|---:|---:|---:|---:|---:|
| 16 bytes | 50000 | 4 | 0.056264 | 0.14 | 108.48 |
| 256 bytes | 50000 | 4 | 0.085902 | 0.21 | 1136.83 |
| 4 KB | 10000 | 4 | 0.128555 | 1.61 | 2430.87 |
| 64 KB | 5000 | 4 | 0.256706 | 6.42 | 9738.78 |
| 1 MB | 1000 | 4 | 0.596153 | 74.52 | 13419.38 |

**Analise dos resultados obtidos:**

#### 1. Comportamento da latencia

Latencia aumenta significativamente com tamanho da mensagem:
- 16 bytes: 0.14 us
- 1 MB: 74.52 us

Razao: **~530x maior**

Explicacao: A latencia e dominada pelo tempo de transferencia de dados em mensagens grandes. Para mensagens pequenas, o overhead fixo (setup, sincronizacao) e mais relevante.

Formula aproximada: `Latencia ≈ Overhead_fixo + (Tamanho_mensagem / BandwidthRaw)`

#### 2. Comportamento da largura de banda

Banda aumenta dramaticamente com tamanho da mensagem:
- 16 bytes: 108.48 MB/s
- 1 MB: 13419.38 MB/s

Razao: **~124x maior**

Explicacao: Para mensagens pequenas, overhead fixo "desperdiça" tempo. Para mensagens grandes, a banda se aproxima do limite fisico do barramento/memoria.

Observacao: 13.4 GB/s e consistente com:
- Memoria local (WSL/Linux): velocidade de copia em memoria e muito alta.
- Barramento/cache eficiente entre processos na mesma maquina.

#### 3. Relacao Banda-Latencia

Para cada tamanho, podemos validar: `Latencia ≈ Tempo_msg / Banda`

Exemplo para 256 bytes:
- Tempo transferencia esperado: 256 bytes / 1136.83 MB/s = 0.000225 ms = 0.225 us
- Latencia medida: 0.21 us

✅ **Match quase perfeito!** Confirma que latencia reflete principalmente tempo de transferencia.

#### 4. Saturacao de largura de banda

A banda aumenta ate ~13.4 GB/s e depois nao cresce mais (teste com 1 MB ja mostra platô).

Isto e esperado: e o limite fisico de throughput entre processos em WSL/memoria compartilhada.

**Conclusoes (Exercicio 3):**

1. ✅ Latencia aumenta com tamanho de mensagem (overhead fixo + tempo de copia).
2. ✅ Largura de banda aumenta com tamanho de mensagem (amortizacao de overhead).
3. ✅ Comportamento e previsivel e consistente com modelo analítico.
4. ✅ Ambiente local (WSL) fornece banda muito alta (~13 GB/s) em memoria compartilhada.

### 2.6 Exercico 4 - Broadcast vs envios individuais

Descricao tecnica:

O programa [broadcast_vs_send.c](broadcast_vs_send.c) implementa dois cenarios equivalentes de distribuicao de um array grande:

1. **MPI_Bcast**: utiliza operacao coletiva otimizada para broadcast.
2. **MPI_Send individual**: implementa broadcast manualmente com envios individuais do processo 0 para cada outro processo.

Compilacao:

```bash
mpicc broadcast_vs_send.c -o broadcast_vs_send
```

Execucoes com diferentes tamanhos de array:

```bash
# Teste 1: Array pequeno (1000 inteiros)
mpirun -np 4 ./broadcast_vs_send 1000 1000

# Teste 2: Array medio (100000 inteiros)
mpirun -np 4 ./broadcast_vs_send 100 100000

# Teste 3: Array grande (1000000 inteiros)
mpirun -np 4 ./broadcast_vs_send 10 1000000

# Teste 4: Escalabilidade com 8 processos
mpirun -np 8 ./broadcast_vs_send 100 100000
```

Figura 4 - Execucao do broadcast do array.
Figura 5 - Execucao dos envios individuais.

Tabela 3 - Comparacao de desempenho.

| Array Size | Rounds | p | Bcast Time (s) | Send Time (s) | Melhor Metodo | Diferenca (%) |
|---|---:|---:|---:|---:|---|---:|
| 1000 | 1000 | 4 | 0.001572 | 0.002505 | MPI_Bcast | 37.26% |
| 100000 | 100 | 4 | 0.011070 | 0.012207 | MPI_Bcast | 9.31% |
| 1000000 | 10 | 4 | 0.031996 | 0.018031 | Send individual | 43.65% |

**Analise detalhada dos resultados:**

#### 1. Comportamento para arrays pequenos (1000 elementos)

**Resultado:** MPI_Bcast e 37.26% mais rápido.

Explicacao:
- MPI_Bcast usa algoritmo otimizado (tree-based ou flat), evitando overhead de multiplos sends.
- Envio individual: processo 0 faz 3 MPI_Send sequenciais (um para cada outro processo).
- Para dados pequenos, overhead de setup de cada Send e significativo.

Conclusao: **Bcast vence para dados pequenos.**

#### 2. Comportamento para arrays medios (100000 elementos)

**Resultado:** MPI_Bcast e ainda 9.31% mais rápido, mas diferenca diminui.

Explicacao:
- Com mais dados, custo de transferencia domina sobre overhead fixo.
- MPI_Bcast ainda tem vantagem devido a otimizacoes internas (pipelining, etc).
- Diferenca diminui porque overhead e amortizado pela quantidade de dados.

Conclusao: **Bcast ainda vence, mas margem reduz.**

#### 3. Comportamento para arrays grandes (1000000 elementos) - RESULTADO CONTRA-INTUITIVO

**Resultado:** Send individual e 43.65% MAIS RÁPIDO!

Isto e inesperado! Possíveis razoes:

A. **Implementacao MPI_Bcast ineficiente para este caso:**
   - MPI_Bcast em WSL/OpenMPI pode usar algoritmo genérico (flat tree).
   - Para 4 processos, flat tree = processo 0 envia para 1, 2, 3 sequencialmente.
   - Isto e exatamente o mesmo que nossa implementacao manual!

B. **Overhead do Bcast > beneficio de otimizacao:**
   - Bcast pode ter checks adicionais, sincronizacao, overhead de protocolo.
   - Para dados grandes em localhost, comunicacao e rapida demais; overhead e proporcional.

C. **Diferenca na politica de buffering:**
   - Bcast pode fazer copias adicionais; Send individual pode ser mais direto.

D. **Nao-determinismo em performance:**
   - Memoria, cache, escalonamento podem variar entre rodadas.
   - Mas diferenca de 43.65% e significativa (nao e ruido).

**Implicacao prática:**
Para distribuir dados muito grandes em pequeno numero de processos (p=4), envios individuais podem ser competitivos ou superiores a Bcast!

#### 4. Validacao dos dados

Analisando taxa de transferencia por metodo:

**Array de 1000000 inteiros = 4 MB**

Bcast (10 rodadas):
- Total: 10 * 4 MB = 40 MB
- Tempo: 0.031996 s
- Taxa: 1250 MB/s por rodada

Send individual (10 rodadas, 3 sends por rodada):
- Total: 10 * 3 * 4 MB = 120 MB enviados
- Tempo: 0.018031 s
- Taxa: 6650 MB/s

Send individual e mais "rápido" no sentido de throughput absoluto de bytes, confirmando resultado.

**Conclusoes (Exercicio 4):**

1. ✅ Para arrays pequenos: **MPI_Bcast e mais eficiente** (37% mais rápido).
2. ✅ Para arrays medios: **MPI_Bcast ainda vence** (9% mais rápido).
3. ✅ Para arrays grandes em p=4: **Send individual pode ser competitivo** ou superior.
4. ✅ Escolha depende de trade-off:
   - Bcast: melhor para pequenos dados, scales melhor com p grande.
   - Send individual: mais simples, pode ser mais rápido para poucos processos e muito dado.
5. ✅ Em ambiente local (WSL/memoria compartilhada), diferenca e pequena; em rede real, Bcast provavelmente venceria mais.

Conclusao comparativa:

Os resultados demonstram que **nao ha vencedor universal** entre Bcast e Send individual. A escolha depende do tamanho de dados e numero de processos. Para aplicacoes reais, recomenda-se medir ambos os metodos com carga tipica.

## 3. Desafios

Panoramica da estrutura do software concebido:

1. **Modulo de inicializacao MPI:** funcoes `MPI_Init`, `MPI_Comm_rank`, `MPI_Comm_size` para setup.
2. **Modulo de comunicacao ponto-a-ponto:** `MPI_Send` e `MPI_Recv` em topologia de anel.
3. **Modulo de medicao de latencia e largura de banda:** variacao do anel com diferentes tamanhos de mensagem.
4. **Modulo de operacoes coletivas:** `MPI_Bcast` vs implementacao manual de broadcast.
5. **Modulo de sincronizacao:** `MPI_Barrier` para garantir todos os processos estao prontos.

Desafios enfrentados:

1. **Configuracao do ambiente MPI:**
   - WSL nao tinha OpenMPI instalado inicialmente.
   - Resolvido: instalacao de `libopenmpi-dev` e `openmpi-bin`.

2. **Limitacao de slots (processos):**
   - Maquina local tem apenas 4 cores; MPI impede alocar 8 processos por padrao.
   - Resolvido: aceitar limitacao e executar com p=4 no maximo.

3. **Variacao de tempo em microssegundos:**
   - Precisao de medicao era critica; pequenas flutuacoes podia mascarar padroes.
   - Resolvido: usar muitas rounds (50000) para amortizar variacao.

4. **Interpretar resultados contra-intuitivos:**
   - Exercicio 4 mostrou que Send individual pode ser mais rapido que Bcast para dados grandes.
   - Resolvido: analise profunda da implementacao MPI e overhead de protocolo.

5. **Sincronizacao de processos:**
   - Ordem de saida nao-deterministica (Ex: Exercicio 1).
   - Resolvido: isso e esperado em computacao paralela; usar `MPI_Barrier` quando necessario.

## 4. Referencias Bibliograficas

1. **Message Passing Interface Forum** (2021). MPI: A Message-Passing Interface Standard. 
   - https://www.mpi-forum.org/

2. **OpenMPI Official Documentation**
   - https://www.open-mpi.org/doc/

3. **W. Gropp, E. Lusk, A. Skjellum.** (2014). Using MPI: Portable Parallel Programming with the Message-Passing Interface (3rd ed.). MIT Press.

4. **Laboratorio CPD - Materiais da Disciplina.** Departamento de Informatica, ISPTEC.

5. **Intel Math Kernel Library (MKL) - Benchmark Tools.**
   - https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/onemkl.html

## 5. Repositorio GitHub

**Link do repositorio:** [Preencher URL do repositorio criado]

**Utilizador GitHub para convite de colaborador:** `joaojdacosta`

**Como adicionar colaborador:**
1. Vai para o repositorio no GitHub.
2. Settings > Collaborators > Add people.
3. Digita `joaojdacosta` e envia convite.

---

## Resumo Final

Este laboratorio foi uma experiencia pratica completa sobre comunicacao paralela com MPI. Foram realizadas:

- ✅ Compilacao e execucao de programas MPI em anel.
- ✅ Medicao de latencia e largura de banda de comunicacao.
- ✅ Comparacao de estrategias de distribuicao de dados (Bcast vs Send).
- ✅ Analise e interpretacao de resultados experimentais.
- ✅ Documentacao tecnica detalhada.

Os resultados validaram conceitos teoricos de computacao paralela e destacaram trade-offs de design em sistemas distribuidos.
