#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <./array_stack.h>
#include <./array_queue.h>

using namespace std;


class Cenario {
  public:
    Cenario(string& texto, size_t indice_inicial) {
        size_t pos = indice_inicial;
        nome = proxima_tag_conteudo(texto, pos, "nome");
        altura = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "altura") ) );
        largura = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "largura") ) );
        x = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "x") ) );
        y = static_cast<size_t>( stoi( proxima_tag_conteudo(texto, pos, "y") ) );
        matriz = matriz_remove_espacos( proxima_tag_conteudo(texto, pos, "matriz") );
        indice_final = pos;
    }
    ~Cenario() {};
    string nome;
    size_t altura;
    size_t largura;
    size_t x;
    size_t y;
    string matriz;
    size_t indice_final;

  private:
    string proxima_tag(string& texto, size_t& pos) {
        string tag = "";
        for ( ; pos < texto.length(); pos++) {
            if (texto[pos] == '<') {
                pos++;
                while (texto[pos] != '>') {
                    tag += texto[pos];
                    pos++;
                }
                pos++;
                return tag;
            }
        }
        return tag;
    }
    string proximo_conteudo(string& texto, size_t& pos) {
        string txt = "";
        while (texto[pos] != '<') {
            txt += texto[pos];
            pos++;
        }
        while (texto[pos] != '>') {
            pos++;
        }
        pos++;
        return txt;
    }
    string proxima_tag_conteudo(string& texto, size_t& pos, string nome_tag) {
        string tag = "";
        while (tag != nome_tag) {
            tag = proxima_tag(texto, pos);
        }
        return proximo_conteudo(texto, pos);
    }
    string matriz_remove_espacos(string texto) {
        string saida;
        for (int i = 0; i < texto.length(); i++) {
            if (texto[i] == '0' || texto[i] == '1') {
                saida += texto[i];
            }
        }
        return saida;
    }
};

struct coords {
    int x; // coordenada x do cenário
    int y; // coordenada y do cenário
};


/**********************
    FUNÇÃO PRINCIPAL
***********************/
int main() {

    string filename;

    std::cin >> filename;  // nome do arquivo de entrada 
                           // (no 'executar': escrever pelo teclado;
                           //  no 'avaliar' : nome é passado pelos testes)

    // Abertura do arquivo
    ifstream filexml(filename);
    if (!filexml.is_open()) {
        cerr << "Erro ao abrir o arquivo " << filename << endl;
        throw runtime_error("Erro no arquivo XML");
    }

    // Leitura do XML completo para 'texto'
    string texto;
    string ID;
    structures::ArrayStack<string> pilha(100);
    char character;
    while (filexml.get(character)) {
        texto += character;
        if (character == '<' && filexml.peek() != '/') {
            while (filexml.get(character) && character != '>') {
                texto += character;
                ID += character;
            }
            texto += '>';
            pilha.push(ID);
            ID = "";
        } else if (character == '<' && filexml.peek() == '/') {
            while (filexml.get(character) && character != '>') {
                texto += character;
                if (character != '/') {
                    ID += character;
                }
            }
            texto += '>';
            if (ID == pilha.top()) {
                pilha.pop();
            } else {
                cerr << "Erro: tag de fechamento '" << ID << "' não corresponde à última tag de abertura." << endl;
                throw runtime_error("Erro no arquivo XML");
            }
        }
    }
    if (!pilha.empty()) {
        cerr << "Erro: existem tags de abertura sem correspondentes tags de fechamento." << endl;
        throw runtime_error("Erro no arquivo XML");
    }


    // ----------------------------
    // Sugestão de código para a PARTE 2 do projeto

    // Exemplo de leitura do primeiro cenário - REMOVER ESTAS SAÍDAS DE TELA NA VERSÃO FINAL
    Cenario c1(texto, 0);

    cout << "nome   : " << c1.nome << endl;
    cout << "altura : " << c1.altura << endl;
    cout << "largura: " << c1.largura << endl;
    cout << "x      : " << c1.x << endl;
    cout << "y      : " << c1.y << endl;
    cout << "matriz : " << c1.matriz << endl << endl;

    // Exemplo de leitura do segundo cenário (a partir do índice final de c1) - REMOVER ESTAS SAÍDAS DE TELA NA VERSÃO FINAL
    Cenario c2(texto, c1.indice_final);

    cout << "nome   : " << c2.nome << endl;
    cout << "altura : " << c2.altura << endl;
    cout << "largura: " << c2.largura << endl;
    cout << "x      : " << c2.x << endl;
    cout << "y      : " << c2.y << endl;
    cout << "matriz : " << c2.matriz << endl << endl;

int** matriz_E = new int*[c1.altura];
int** matriz_R = new int*[c1.altura];
for (size_t i = 0; i < c1.altura; i++) {
    matriz_E[i] = new int[c1.largura];
    matriz_R[i] = new int[c1.largura];
    for (size_t j = 0; j < c1.largura; j++) {
        matriz_E[i][j] = c1.matriz[i * c1.largura + j] - '0';
        matriz_R[i][j] = 0;
    }
}
// início do algoritmo de busca aqui
structures::ArrayQueue<coords> fila(c1.altura * c1.largura);
matriz_R[c1.x][c1.y] = 1; // marca a posição inicial do cenário
coords pos_inicial = {static_cast<int>(c1.x), static_cast<int>(c1.y)};
if (matriz_E[pos_inicial.x][pos_inicial.y] == 0) {
    cout << "Área limpa: 0" << endl;
    return 0;
}
fila.enqueue(pos_inicial);
int area_limpa = 0;
while (!fila.empty()) {
    coords pos_atual = fila.dequeue();
    area_limpa++;
    // Verificar as 4 direções
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    for (int d = 0; d < 4; d++) {
        int nova_x = pos_atual.x + dx[d];
        int nova_y = pos_atual.y + dy[d];
        // Verificar se a nova posição é válida
        if (nova_x >= 0 && nova_x < static_cast<int>(c1.altura) && nova_y >= 0 && nova_y < static_cast<int>(c1.largura) && matriz_E[nova_x][nova_y] == 1 && matriz_R[nova_x][nova_y] == 0) {
            coords nova_pos = {nova_x, nova_y};
            fila.enqueue(nova_pos);
            matriz_R[nova_x][nova_y] = 1; // marca a posição como visitada
        }
    }
}
free(matriz_E);
free(matriz_R);
cout << "Área limpa: " << area_limpa << endl;

    return 0;
}
