/**
    @authors: Jean Bonadeo Dal Santo, Felipe M. Fagundes (original ED2/POO version)
          Qt adaptation: Jean Bonadeo Dal Santo
*/
#pragma once
#include <fstream>
#include <cwchar>
#include <iostream>
#include <cstdlib>
#include <bitset>
#include <unordered_map>



namespace HT{

class descompacter{
    
public:

    descompacter(){};

    ///Função que pega os arquivos gerados pelo compacter e os descompacta, gerando o arquivo original antes da compactação.
    int descompactar(const std::string &tarPath, const std::string &outPath);

private:
    std::wstring binario;
    std::unordered_map<std::wstring, wchar_t> map_cod;
    
    ///Função que casta um byte para uma string de seus bits.
    [[nodiscard]] std::wstring byte_binary_to_string(char byte) const noexcept;

    bool save(int tam, const std::string &outPath) noexcept;

};

}