/**
    @authors: Jean Bonadeo Dal Santo, Felipe M. Fagundes (original ED2/POO version)
          Qt adaptation: Jean Bonadeo Dal Santo
*/
#pragma once
#include <iostream>
#include <cstdlib>

namespace HT{

class file_status{

public:
    file_status(std::string _original, std::wstring _comprimido):original(_original), comprimido(_comprimido){}

    /// Função para obter o tamanho original do arquivo. 
    [[nodiscard]]int tam_origin()const noexcept;
    ///Função para obter o tamanho comprimido do arquivo.
    [[nodiscard]] int inline tam_compressed() const noexcept{
        return comprimido.size() / static_cast<size_t>(8);
    }
    ///Função para pegar a diferença em bytes do arquivo original e do comprimido.
    [[nodiscard]] int inline diff() const noexcept{
        return abs(tam_origin() - tam_compressed());
    }
    ///Função para pegar a porcentagem da diferença.
    [[nodiscard]] double inline porcent() const noexcept{
        return (static_cast<double>(diff()) / static_cast<double>(tam_origin()) * 100);
    }

private:
    std::string original;
    std::wstring comprimido;
};

}