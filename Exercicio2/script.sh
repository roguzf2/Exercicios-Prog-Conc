#!/bin/bash

threads=(1 2 4)
repeticoes=(1 2 3 4 5 6 7 8 9 10)

# Nome do arquivo CSV temporário
csv_file="tempos_results.csv"

# Escrever cabeçalho no arquivo CSV
echo "Valor_de_N,Nthreads,Repeticao,Inicialização,Busca_de_Primos,Finalização,Total" > $csv_file

for i in ${threads[@]}
do
    for j in ${repeticoes[@]}
    do
        # echo "Executando: Threads=$i, Repetição=$j"
        
        # Executa o programa e captura a saída formatada
        ./primos $i 1000 | awk -v threads=$i -v rep=$j -v n_val=1000 '
        /Valor_de_N/ {next}  # Ignora o cabeçalho
        {
            gsub(/ /, "", $0);  # Remove todos os espaços
            split($0, fields, ",");  # Divide pelos separadores
            print n_val "," threads "," rep "," fields[3] "," fields[4] "," fields[5] "," fields[6]
        }' >> $csv_file
    done
done

# Converter para XLSX (usando Python com pandas)
if command -v python3 &> /dev/null; then
    python3 -c "
import pandas as pd
import sys

try:
    df = pd.read_csv('$csv_file')
    lista_media_total = []
    total = df['Total']
    media_acumulada = 0
    for i in range(len(total)):
        media_acumulada += total[i]
        if (i + 1) % 10 == 0:
            media_acumulada += total[i]
            lista_media_total.append(media_acumulada)
            media_acumulada = 0
    print(lista_media_total)
    df.to_excel('primos_results_10e3.xlsx', index=False)
    print('\nArquivo XLSX gerado com sucesso: primos_results_10e3.xlsx')
except Exception as e:
    print(f'Erro ao converter para XLSX: {str(e)}')
    sys.exit(1)
"
else
    echo ""
    echo "Python3 não encontrado. O arquivo CSV foi gerado: $csv_file"
    echo "Você pode abrir este arquivo no Excel e salvar como XLSX."
fi

rm -f $csv_file
