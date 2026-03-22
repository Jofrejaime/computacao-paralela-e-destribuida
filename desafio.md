Aqui está a transcrição fiel em Markdown (imagem + texto):

---

## Figura (diagrama)

**estrutura de dados**

**processo pai
(dois fios de execução)**

teclado
écran

fork + exec → **processo filho**
exit

fork + exec → **processo filho**
exit

*Figura 1: Representação dos processos e de algumas das chamadas sistema a utilizar.*

---

## Desafio do Laboratório

Pretende-se desenvolver um terminal paralelo simples, denominado **cpd-terminal**, que executa e monitoriza um conjunto de programas em paralelo em uma máquina multi-core.

Figura 1: Representação dos processos e de algumas das chamadas sistema a utilizar.

O cpd-terminal é um programa que permite a um utilizador emitir ordens para executar programas existentes no sistema de ficheiros da máquina. A caracteristica relevante é de permitir a execução de múltiplos comandos em paralelo.

---

## Etapa 1: Comandos do teclado

Devem ser suportados os seguintes comandos:

• **pathname [arg1 arg2 ...]**, que executa o programa contido no ficheiro indicado pelo pathname, como processo filho, passando-lhe os argumentos opcionais que sejam indicados (até um máximo de 5 argumentos permitidos).
O processo filho é executado em background, ou seja o cpd-terminal não espera pela término dos processos filho que são executados e fica pronta a executar novos processos filho.

Caso a execução de algum processo falhe (por exemplo, devido a um pathname inválido ou a erro na criação de processo filho), o cpd-terminal deve apresentar uma mensagem reportando o erro no stderr, mas o cpd-terminal não deve terminar.

• **exit**, que termina o cpd-terminal de forma ordeira. Em particular, espera pelo término de todos os processos filho (incluindo aqueles que não tenham ainda terminado aquando da ordem de exit).

Após todos os processos filho terem terminado, o cpd-terminal deve apresentar no stdout o pid e o inteiro devolvido por cada processo filho.

---

## Etapa 2: Monitorização do desempenho

Deve estender o cpd-terminal com a funcionalidade de monitorização de desempenho. Para tal, deve ser criada uma tarefa (thread) adicional no cpd-terminal, que será responsável por monitorizar os tempos de execução de cada processo filho.

A tarefa de monitorização, denominada tarefa monitora, coordena-se com a tarefa principal (que existe, por omissão, sempre em qualquer processo) da seguinte forma:

• A tarefa principal deverá passar a registar o pid de cada processo filho executado e o respetivo tempo numa estrutura de dados partilhada (entre as duas tarefas em causa, i.e. a principal e a monitora).
A medição do tempo deverá ser feita usando a função **time**.

• A informação sobre processos filho, partilhada entre ambas as tarefas, deverá ser mantida numa lista simplesmente ligada.

• A tarefa monitora espera pela terminação dos processos filho; para cada processo filho terminado, a tarefa monitora mede o tempo de terminação e adiciona-o no registo correspondente na estrutura partilhada.

---

Obviamente, é necessário que ambas as tarefas acima mencionadas se coordenem. Caso contrário, a tarefa monitora não sabe se há processos filhos que justifiquem que esta se bloqueie esperando pela sua terminação. Essa coordenação deve ser feita da seguinte forma:

• Além da lista com informação sobre os processos filhos, a tarefa principal partilhará com a tarefa monitora um inteiro (denominado **numChildren**) que indica o número de processos filho em execução.

• Assim, obviamente, sempre que a tarefa principal lança um novo processo filho (chamando **fork**), incrementa o inteiro numChildren.

• A tarefa monitora consiste, basicamente, num ciclo infinito; em cada iteração do ciclo, consulta o valor de numChildren para saber se há pelo menos um processo filho em execução.
Se não há, espera 1 segundo antes da próxima iteração (invocando a função **sleep**).
Se há pelo menos um processo filho, espera pela terminação do(s) processo(s) filho, invocando **wait**.

---

Há um caso que requer especial cuidado: quando o utilizador do cpd-terminal dá ordem de terminação (através do comando **exit**), a tarefa principal deve informar a tarefa monitora para que esta, depois de esperar que todos os processos filho terminem (registando o seu tempo de terminação), também termine.

Assim, a tarefa principal deverá aguardar que a tarefa monitora termine.

Após este ponto de sincronização, antes do processo em causa terminar, a tarefa principal deverá imprimir a informação completa sobre todos os processo filho que foram lançados (e completados) ao longo da execução do cpd-terminal:

• pid
• tempo de execução em segundos (i.e. tempo de terminação - tempo de lançamento)

---

## Nota importante:

• devem ser usadas as seguintes funções da API do Unix/Linux: **fork, exec*, wait, exit** além das funções habituais da biblioteca stdio. E as funções POSIX para gestão de tarefas (threads) e trincos (locks).

• A leitura da linha comandos deve ser feita usando a biblioteca **commandlinereader**, que pode ser obtida na turma Google Classroom

---

A solução deverá incluir uma makefile com um alvo chamado **cpdterminal** que gera um ficheiro executável da solução, com o mesmo nome.
Soluções que não cumpram este requisito não serão avaliadas.

---

## Experiências

Use o programa **fibonacci** fornecido como exemplo para testar o seu cpdterminal. Executando esse programa com argumento elevado, os processos filhos demoram vários segundos a terminar, o que permite testar situações em que múltiplos processos filhos se executam em simultâneo.

Para experiências com múltiplos programas, sugerimos que componha ficheiros de texto com sequências de comandos. Por exemplo:

```
fibonacci 10000
fibonacci 10001
fibonacci 10002
exit
```

Para executar este conjunto de comandos, basta depois executar na linha de comandos:

```
cpd-terminal < input.txt
```

(em que input.txt é um ficheiro com uma sequência de comandos tal como a apresentada acima).

---

Discuta o comportamento da tarefa monitora; em particular, considere o caso em que não há processos filho. Altere o valor do tempo de sleep e relacione com o conceito de S.O sobre a noção de espera activa.
