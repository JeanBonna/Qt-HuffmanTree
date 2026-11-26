/**
    @authors: Jean Bonadeo Dal Santo, Felipe M. Fagundes (original ED2/POO version)
          Qt adaptation: Jean Bonadeo Dal Santo
*/
#include "descompacter.hpp"


    int HT::descompacter::descompactar(){
        system("tar -xf arq_zipado.tar -C /tmp codificacao.txt arq_compactado.txt");
        std::wifstream codigos("/tmp/codificacao.txt");
        if(!codigos){
            // std::wcout << "Falha ao abrir meta dados\n"
            //            << L"é necessario arquivo arq_zipado.tar na pasta de execução do programa\n";
            // exit(1);
                return 1;
        }
        
        wchar_t cc;
        std::wstring bin;
        
        int tam;
        codigos >> tam;
        while(codigos.get(cc)){
            codigos.get();
            codigos >> bin;
            map_cod[bin] = cc;
            codigos.get();
        }


       std::ifstream entrada("/tmp/arq_compactado.txt", std::ios::binary);
        if(!entrada){
            // std::wcout << "Falha ao abrir arquivo compactado\n"
            //            << L"é necessario arquivo arq_zipado.tar na pasta de execução do programa\n";
            // exit(1);
            return 2;
        }

        char byte;
        while(entrada.get(byte))
            binario += byte_binary_to_string(byte);
        

        entrada.close();

        save(tam);
        return 0;

    }

    [[nodiscard]] std::wstring HT::descompacter::byte_binary_to_string(char byte) const noexcept{
        std::wstring bin_str;
        for (int i = 7; i >= 0; --i) 
            bin_str += (byte & (1 << i)) ? L'1' : L'0';
        
    return bin_str;
    }

    void HT::descompacter::save(int tam) noexcept{
        std::wofstream saida("arq_descompactado.txt");
        std::wstring bin=L"";
        int count=0;
        for(auto c:binario){
            bin+=c;
            if(map_cod.count(bin)){
                count++;
                saida << map_cod[bin];
                bin = L"";
                if(count == tam) break;
            }
        }
        saida.close();
    }