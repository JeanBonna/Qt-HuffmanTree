/**
    @authors: Jean Bonadeo Dal Santo, Felipe M. Fagundes (original ED2/POO version)
          Qt adaptation: Jean Bonadeo Dal Santo
*/
#include "descompacter.hpp"


    int HT::descompacter::descompactar(const std::string &tarPath, const std::string &outPath){
        // Limpa o estado, caso o mesmo objeto seja usado mais de uma vez
        map_cod.clear();
        binario.clear();

        // 1. Extrai os dois arquivos do .tar para /tmp
        const std::string cmd = "tar -xf '" + tarPath + "' -C /tmp codificacao.txt arq_compactado.txt";
        if (system(cmd.c_str()) != 0)
            return 3;

        // 2. Lê a tabela de códigos: primeiro o total de caracteres, depois "<char> <código>\n"
        std::wifstream codigos("/tmp/codificacao.txt");
        if (!codigos)
            return 1;

        int tam = 0;
        if (!(codigos >> tam) || tam <= 0)
            return 1;

        wchar_t cc;
        std::wstring bin;
        while (codigos.get(cc)) {       // o caractere (pode ser ' ', '\n'...)
            codigos.get();              // pula o espaço separador
            if (!(codigos >> bin))      // o código binário
                break;
            map_cod[bin] = cc;
            codigos.get();              // pula o '\n' do fim da linha
        }
        codigos.close();

        if (map_cod.empty())
            return 1;

        // 3. Lê os bytes compactados e converte para uma string de bits
        std::ifstream entrada("/tmp/arq_compactado.txt", std::ios::binary);
        if (!entrada)
            return 2;

        entrada.seekg(0, std::ios::end);
        binario.reserve(static_cast<size_t>(entrada.tellg()) * 8);   // evita realocações
        entrada.seekg(0, std::ios::beg);

        char byte;
        while (entrada.get(byte))
            binario += byte_binary_to_string(byte);
        entrada.close();

        // 4. Decodifica e grava
        if (!save(tam, outPath))
            return 4;

        return 0;
    }

    bool HT::descompacter::save(int tam, const std::string &outPath) noexcept{
        std::wofstream saida(outPath);
        if (!saida)
            return false;

        std::wstring bin;
        int count = 0;

        for (const wchar_t c : binario) {
            bin += c;
            auto it = map_cod.find(bin);        // uma busca só, em vez de count() + []
            if (it != map_cod.end()) {
                saida << it->second;
                bin.clear();
                if (++count == tam)             // para antes dos bits de preenchimento do último byte
                    break;
            }
        }

        saida.close();
        return count == tam && !saida.fail();   // false se faltou caractere ou a escrita falhou
    }

    std::wstring HT::descompacter::byte_binary_to_string(char byte) const noexcept{
        std::wstring bin_str;
        for (int i = 7; i >= 0; --i)
            bin_str += (byte & (1 << i)) ? L'1' : L'0';
        return bin_str;
    }