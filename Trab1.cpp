#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "array_stack.h"
#include "array_queue.h"

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
        cout << "erro" << endl;
        return 0;
    }

    // Leitura do XML completo para 'texto'
    string texto;
    string ID;
    int marcador = 0; // variável para marcar o índice final do cenário
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
                ID = "";    
            } else {
                cout << "erro" << endl;
                return 0;
            }
        }
    }
    if (!pilha.empty()) {
        cout << "erro" << endl;
        return 0;
    }

    while (marcador < texto.length() && texto.find("<nome>", marcador) != string::npos) {
        // ----------------------------
        // Sugestão de código para a PARTE 2 do projeto

        // criação do cenário a partir do texto e do marcador   
        Cenario c1(texto, marcador);
        //criação das matrizes
        int** matriz_E = new int*[c1.altura];
        int** matriz_R = new int*[c1.altura];
        int area_limpa = 0;
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
        coords pos_inicial = {static_cast<int>(c1.x), static_cast<int>(c1.y)};
        //tratamento para o caso de a posição inicial já ser um obstáculo (0)
        if (matriz_E[pos_inicial.x][pos_inicial.y] != 0) {
            fila.enqueue(pos_inicial);
            matriz_R[c1.x][c1.y] = 1; // marca a posição inicial do cenário
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
            
        }
        for (size_t i = 0; i < c1.altura; i++) {
            delete [] matriz_E[i];
            delete [] matriz_R[i];       
        }
        delete [] matriz_E;
        delete [] matriz_R;

        cout << c1.nome << " " << area_limpa << endl;

        marcador = c1.indice_final;
    }
    return 0;
}